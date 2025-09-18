/*
 * 🌈✨ Cosmic LED Controller - ESP32 Code ✨🌈
 * 
 * This code creates a WiFi-enabled LED controller for WS2812B strips
 * Compatible with the Cosmic LED Controller web interface
 * 
 * WIRING:
 * 🔴 VCC (Red wire)   → ESP32 VIN
 * ⚪ GND (White wire)  → ESP32 GND  
 * 🟢 DAT (Green wire) → ESP32 GPIO 18
 * 
 * Required Libraries (install via Arduino Library Manager):
 * - FastLED
 * - ESPAsyncWebServer
 * - AsyncTCP
 * - ArduinoJson
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <FastLED.h>

// ========== CONFIGURATION - UPDATE THESE! ==========
const char* ssid = "YOUR_WIFI_SSID";           // ← CHANGE THIS
const char* password = "YOUR_WIFI_PASSWORD";   // ← CHANGE THIS

// LED Strip Configuration
#define LED_PIN 18          // Data pin (🟢 Green wire to GPIO 18)
#define NUM_LEDS 144        // ← CHANGE TO YOUR LED COUNT
#define LED_TYPE WS2812B    // Type of LED strip
#define COLOR_ORDER GRB     // Color order (try RGB if colors are wrong)
#define BRIGHTNESS 255      // Maximum brightness (0-255)

// Debug Mode
#define DEBUG_MODE 1        // Set to 1 to enable serial debug output

// ========== GLOBAL VARIABLES ==========
CRGB leds[NUM_LEDS];
AsyncWebServer server(80);

// Current LED state
struct LEDState {
  uint8_t r = 255;
  uint8_t g = 255; 
  uint8_t b = 255;
  uint8_t speed = 50;
  String mode = "solid";
  unsigned long lastUpdate = 0;
};

LEDState currentState;

// Animation variables
uint8_t hue = 0;
uint8_t brightness = BRIGHTNESS;
bool breathingDirection = true;

// ========== SETUP FUNCTION ==========
void setup() {
  Serial.begin(115200);
  Serial.println("\n🌈✨ Cosmic LED Controller Starting... ✨🌈");
  
  // Initialize LED Strip
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  
  // Startup animation
  cosmicStartup();
  
  // Connect to WiFi
  connectToWiFi();
  
  // Setup web server routes
  setupWebServer();
  
  // Start the server
  server.begin();
  
  Serial.println("✨ Cosmic LED Controller Ready! ✨");
  Serial.print("🌐 Web Interface: http://");
  Serial.println(WiFi.localIP());
  Serial.println("📝 Update this IP in your web app!");
}

// ========== MAIN LOOP ==========
void loop() {
  handleAnimations();
  delay(10);
}

// ========== WIFI CONNECTION ==========
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("🔗 Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Connecting animation
    leds[attempts % NUM_LEDS] = CRGB::Blue;
    FastLED.show();
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Success animation - green wave
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();
    delay(1000);
    FastLED.clear();
    FastLED.show();
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
    // Error animation - red blink
    for (int i = 0; i < 3; i++) {
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      FastLED.show();
      delay(200);
      FastLED.clear();
      FastLED.show();
      delay(200);
    }
  }
}

// ========== WEB SERVER SETUP ==========
void setupWebServer() {
  // Enable CORS for web interface
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
  
  // Handle preflight requests
  server.on("/setcolor", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    request->send(200);
  });
  
  // Main color control endpoint
  server.on("/setcolor", HTTP_POST, [](AsyncWebServerRequest *request){
    handleColorRequest(request);
  });
  
  // Status endpoint
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    handleStatusRequest(request);
  });
  
  // Root endpoint
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = R"(
    <!DOCTYPE html>
    <html>
    <head><title>🌈✨ Cosmic LED Controller ✨🌈</title></head>
    <body style='font-family: Arial; text-align: center; padding: 50px; background: linear-gradient(45deg, #FF69B4, #8A2BE2);'>
      <h1 style='color: white;'>🌈✨ Cosmic LED Controller ✨🌈</h1>
      <p style='color: white; font-size: 18px;'>ESP32 is running and ready!</p>
      <p style='color: white;'>💡 LEDs: )" + String(NUM_LEDS) + R"(</p>
      <p style='color: white;'>📡 IP: )" + WiFi.localIP().toString() + R"(</p>
      <p style='color: white;'>Use the Cosmic LED web interface to control your lights!</p>
    </body>
    </html>
    )";
    request->send(200, "text/html", html);
  });
}

// ========== COLOR REQUEST HANDLER ==========
void handleColorRequest(AsyncWebServerRequest *request) {
  if (request->hasParam("body", true)) {
    String body = request->getParam("body", true)->value();
    
    // Parse JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, body);
    
    if (error) {
      if (DEBUG_MODE) {
        Serial.println("❌ JSON Parse Error: " + String(error.c_str()));
      }
      request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }
    
    // Extract values
    currentState.r = doc["r"] | currentState.r;
    currentState.g = doc["g"] | currentState.g;
    currentState.b = doc["b"] | currentState.b;
    currentState.speed = doc["speed"] | currentState.speed;
    currentState.mode = doc["mode"] | "solid";
    currentState.lastUpdate = millis();
    
    if (DEBUG_MODE) {
      Serial.printf("🎨 Color: R:%d G:%d B:%d Speed:%d Mode:%s\n", 
                    currentState.r, currentState.g, currentState.b, 
                    currentState.speed, currentState.mode.c_str());
    }
    
    // Apply color immediately if solid mode
    if (currentState.mode == "solid") {
      setSolidColor(currentState.r, currentState.g, currentState.b);
    }
    
    // Send success response
    String response = "{\"status\":\"success\",\"r\":" + String(currentState.r) + 
                     ",\"g\":" + String(currentState.g) + 
                     ",\"b\":" + String(currentState.b) + 
                     ",\"speed\":" + String(currentState.speed) + "}";
    request->send(200, "application/json", response);
  } else {
    request->send(400, "application/json", "{\"error\":\"No body data\"}");
  }
}

// ========== STATUS REQUEST HANDLER ==========
void handleStatusRequest(AsyncWebServerRequest *request) {
  String status = "{";
  status += "\"connected\":true,";
  status += "\"led_count\":" + String(NUM_LEDS) + ",";
  status += "\"current_color\":[" + String(currentState.r) + "," + String(currentState.g) + "," + String(currentState.b) + "],";
  status += "\"speed\":" + String(currentState.speed) + ",";
  status += "\"mode\":\"" + currentState.mode + "\",";
  status += "\"uptime\":" + String(millis()) + ",";
  status += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
  status += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
  status += "}";
  
  request->send(200, "application/json", status);
}

// ========== LED ANIMATION FUNCTIONS ==========
void handleAnimations() {
  static unsigned long lastAnimationUpdate = 0;
  unsigned long now = millis();
  
  // Calculate delay based on speed (1-100 maps to 100ms-10ms)
  int delayTime = map(currentState.speed, 1, 100, 100, 10);
  
  if (now - lastAnimationUpdate > delayTime) {
    lastAnimationUpdate = now;
    
    if (currentState.mode == "rainbow") {
      rainbowAnimation();
    } else if (currentState.mode == "breathing") {
      breathingAnimation();
    } else if (currentState.mode == "cosmic") {
      cosmicAnimation();
    } else if (currentState.mode == "strobe") {
      strobeAnimation();
    }
    // "solid" mode is handled immediately when color is set
  }
}

void setSolidColor(uint8_t r, uint8_t g, uint8_t b) {
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.show();
}

void rainbowAnimation() {
  fill_rainbow(leds, NUM_LEDS, hue, 255 / NUM_LEDS);
  FastLED.show();
  hue += 3; // Speed of rainbow rotation
}

void breathingAnimation() {
  static uint8_t breathBrightness = 0;
  
  if (breathingDirection) {
    breathBrightness += 5;
    if (breathBrightness >= 255) {
      breathBrightness = 255;
      breathingDirection = false;
    }
  } else {
    breathBrightness -= 5;
    if (breathBrightness <= 20) {
      breathBrightness = 20;
      breathingDirection = true;
    }
  }
  
  CRGB color = CRGB(currentState.r, currentState.g, currentState.b);
  color.nscale8(breathBrightness);
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}

void cosmicAnimation() {
  static uint8_t cosmicHue = 0;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t pixelHue = cosmicHue + (i * 3);
    leds[i] = CHSV(pixelHue, 255, 255);
  }
  
  FastLED.show();
  cosmicHue += 2;
}

void strobeAnimation() {
  static bool strobeState = false;
  
  if (strobeState) {
    fill_solid(leds, NUM_LEDS, CRGB(currentState.r, currentState.g, currentState.b));
  } else {
    FastLED.clear();
  }
  
  FastLED.show();
  strobeState = !strobeState;
}

void cosmicStartup() {
  Serial.println("✨ Cosmic Startup Sequence ✨");
  
  // Cosmic color wave
  for (int wave = 0; wave < 3; wave++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t hue = (i * 255 / NUM_LEDS) + (wave * 85);
      leds[i] = CHSV(hue, 255, 255);
      FastLED.show();
      delay(20);
    }
    delay(200);
  }
  
  // Fade to black
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(20);
  }
  
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}
