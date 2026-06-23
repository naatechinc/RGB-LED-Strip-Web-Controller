/*
  Cat Chase LED Game — NodeMCU ESP-32S + WS2812B (265 LEDs)
  ──────────────────────────────────────────────────────────
  Hardware:
    • NodeMCU ESP-32S (Image 2)
    • BTF-Lighting WS2812B ECO, 5V, 265 LEDs
    • GPIO 18 → 330Ω resistor → DAT (green wire)
    • 5V shared rail, common GND
    • 1000µF capacitor across VCC/GND at strip

  Web controller (rgb-led-strip-web-controller.vercel.app) sends to:
    GET /color?r=&g=&b=          — solid color (existing endpoint)
    GET /speed?value=            — animation speed (existing endpoint)

  New cat game endpoints:
    GET /game/start              — start the chase
    GET /game/stop               — stop, all off
    GET /game/set?speed=&length=&color=&brightness=
                                 — set parameters
    GET /state                   — JSON status (polled by web app)

  All brightness is software-capped at 40% (MAX_BRIGHTNESS below)
  to stay within USB power budget (~2.1A for 265 LEDs).
  Raise MAX_BRIGHTNESS if you add a dedicated 5V PSU.
*/

#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ── Hardware config ───────────────────────────────────────────
#define DATA_PIN     18
#define NUM_LEDS     265
#define LED_TYPE     WS2812B
#define COLOR_ORDER  GRB
#define MAX_BRIGHTNESS 100   // 0–255; 100 ≈ 40% — safe for USB
                              // Raise to 255 with a 5V 10A+ PSU

// ── WiFi credentials ─────────────────────────────────────────
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ── LED array ────────────────────────────────────────────────
CRGB leds[NUM_LEDS];

// ── Game state ───────────────────────────────────────────────
bool  gameRunning  = false;
int   chaseSpeed   = 60;     // ms between steps (lower = faster)
int   chaseLength  = 6;      // number of LEDs in the group
uint8_t chaseR     = 0;      // 0 = auto rainbow, else fixed color
uint8_t chaseG     = 0;
uint8_t chaseB     = 0;
bool  autoColor    = true;   // true = rainbow cycle per step
uint8_t brightness = MAX_BRIGHTNESS;

int   chasePos     = 0;      // current head position
uint8_t hue        = 0;      // rainbow hue counter
unsigned long lastStep = 0;

WebServer server(80);

// ─────────────────────────────────────────────────────────────
// Startup flash — all LEDs blink 3× so user knows it's alive
// ─────────────────────────────────────────────────────────────
void startupFlash() {
  for (int i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, CRGB(0, 180, 255));
    FastLED.show();
    delay(200);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(200);
  }
}

// ─────────────────────────────────────────────────────────────
// Chase step — advance the comet one position
// Each step cycles through rainbow hues automatically,
// or uses the fixed color if autoColor is false.
// ─────────────────────────────────────────────────────────────
void chaseStep() {
  // Fade all LEDs toward black (comet tail effect)
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(180);   // 180/255 ≈ 70% retained each frame
  }

  // Determine the color of this group
  CRGB groupColor;
  if (autoColor) {
    groupColor = CHSV(hue, 255, brightness);
    hue += 8;  // shift hue each step — one full cycle per ~32 steps
  } else {
    // Scale fixed color by brightness
    groupColor = CRGB(
      map(chaseR, 0, 255, 0, brightness),
      map(chaseG, 0, 255, 0, brightness),
      map(chaseB, 0, 255, 0, brightness)
    );
  }

  // Light up the group at current position
  for (int j = 0; j < chaseLength; j++) {
    int idx = chasePos + j;
    if (idx >= 0 && idx < NUM_LEDS) {
      leds[idx] = groupColor;
    }
  }

  FastLED.show();

  // Advance position — bounce at ends to keep cats interested
  chasePos++;
  if (chasePos + chaseLength > NUM_LEDS) {
    chasePos = 0;   // wrap to start
  }
}

// ─────────────────────────────────────────────────────────────
// HTTP handlers
// ─────────────────────────────────────────────────────────────

// Existing endpoint: solid color (keeps web app compatibility)
void handleColor() {
  uint8_t r = server.arg("r").toInt();
  uint8_t g = server.arg("g").toInt();
  uint8_t b = server.arg("b").toInt();
  gameRunning = false;
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.setBrightness(brightness);
  FastLED.show();
  server.send(200, "text/plain", "OK");
}

// Existing endpoint: speed (maps 0–100 slider to ms delay)
void handleSpeed() {
  int v = server.arg("value").toInt();
  // Slider value 0–100 → speed 200ms (slow) to 10ms (fast)
  chaseSpeed = map(v, 0, 100, 200, 10);
  server.send(200, "text/plain", "OK");
}

// Game: start
void handleGameStart() {
  gameRunning = true;
  chasePos    = 0;
  hue         = 0;
  server.send(200, "text/plain", "Game started");
}

// Game: stop
void handleGameStop() {
  gameRunning = false;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  server.send(200, "text/plain", "Game stopped");
}

// Game: set parameters
// /game/set?speed=60&length=6&color=auto&brightness=100
// color=auto for rainbow, or color=FF8800 hex for fixed
void handleGameSet() {
  if (server.hasArg("speed")) {
    chaseSpeed = constrain(server.arg("speed").toInt(), 5, 500);
  }
  if (server.hasArg("length")) {
    chaseLength = constrain(server.arg("length").toInt(), 1, 30);
  }
  if (server.hasArg("brightness")) {
    int b = constrain(server.arg("brightness").toInt(), 1, 100);
    brightness = map(b, 1, 100, 5, MAX_BRIGHTNESS);
    FastLED.setBrightness(brightness);
  }
  if (server.hasArg("color")) {
    String col = server.arg("color");
    if (col == "auto") {
      autoColor = true;
    } else {
      // Expect 6-char hex: e.g. FF8800
      autoColor = false;
      long hex = strtol(col.c_str(), NULL, 16);
      chaseR = (hex >> 16) & 0xFF;
      chaseG = (hex >> 8)  & 0xFF;
      chaseB =  hex        & 0xFF;
    }
  }
  server.send(200, "text/plain", "OK");
}

// State endpoint — polled by web app
void handleState() {
  StaticJsonDocument<256> doc;
  doc["running"]    = gameRunning;
  doc["speed"]      = chaseSpeed;
  doc["length"]     = chaseLength;
  doc["autoColor"]  = autoColor;
  doc["colorR"]     = chaseR;
  doc["colorG"]     = chaseG;
  doc["colorB"]     = chaseB;
  doc["brightness"] = map(brightness, 0, MAX_BRIGHTNESS, 0, 100);
  doc["numLeds"]    = NUM_LEDS;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

// CORS preflight (needed so Vercel app can talk to ESP32)
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin",  "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(204);
}

// ─────────────────────────────────────────────────────────────
// Setup
// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // FastLED init
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  // Startup flash
  startupFlash();

  // WiFi connect
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 30) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    // Flash green twice to confirm WiFi connected
    for (int i = 0; i < 2; i++) {
      fill_solid(leds, NUM_LEDS, CRGB(0, 200, 0));
      FastLED.show(); delay(200);
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show(); delay(200);
    }
  } else {
    Serial.println("\nWiFi failed — running in standalone mode");
    // Flash red twice to signal no WiFi
    for (int i = 0; i < 2; i++) {
      fill_solid(leds, NUM_LEDS, CRGB(200, 0, 0));
      FastLED.show(); delay(200);
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show(); delay(200);
    }
  }

  // Register HTTP routes
  server.on("/color",      HTTP_GET,     handleColor);
  server.on("/speed",      HTTP_GET,     handleSpeed);
  server.on("/game/start", HTTP_GET,     handleGameStart);
  server.on("/game/stop",  HTTP_GET,     handleGameStop);
  server.on("/game/set",   HTTP_GET,     handleGameSet);
  server.on("/state",      HTTP_GET,     handleState);

  // Handle CORS preflight for every route
  server.onNotFound([]() {
    if (server.method() == HTTP_OPTIONS) {
      handleOptions();
    } else {
      server.send(404, "text/plain", "Not found");
    }
  });

  // Add CORS headers to every response
  server.enableCORS(true);

  server.begin();
  Serial.println("HTTP server started");

  // Auto-start the game on boot — cat is ready!
  gameRunning = true;
}

// ─────────────────────────────────────────────────────────────
// Loop
// ─────────────────────────────────────────────────────────────
void loop() {
  server.handleClient();

  if (gameRunning) {
    unsigned long now = millis();
    if (now - lastStep >= (unsigned long)chaseSpeed) {
      lastStep = now;
      chaseStep();
    }
  }
}
