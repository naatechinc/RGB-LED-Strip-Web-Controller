# 🤖 ESP32 Arduino Setup Guide

## 🔧 Hardware Requirements
- ESP32 Development Board (DevKit V1 recommended)
- WS2812B LED Strip (any length)
- 5V Power Supply (for strips >30 LEDs)
- Jumper wires

## 📋 Step-by-Step Setup

### 1. Install Arduino IDE
- Download from: https://www.arduino.cc/en/software
- Install ESP32 board support:
  - File → Preferences
  - Add to Additional Board Manager URLs:
    `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
  - Tools → Board → Boards Manager
  - Search "ESP32" and install

### 2. Install Required Libraries
See `libraries_needed.txt` for complete list and installation instructions.

### 3. Hardware Wiring
```
LED Strip Wire    ESP32 Pin
🔴 VCC (Red)   →  VIN (5V)
⚪ GND (White)  →  GND
🟢 DAT (Green) →  GPIO 18
```
### 4. Code Configuration
Open `esp32_led_controller.ino` and update:

```cpp
// WiFi Settings
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// LED Settings
#define NUM_LEDS 144  // Change to your LED count
#define LED_PIN 18    // Data pin (GPIO 18)
```

### 5. Upload Code

1. Connect ESP32 via USB
1. Select Board: “ESP32 Dev Module”
1. Select correct COM Port
1. Click Upload (arrow button)
1. Wait for “Done uploading” message

### 6. Find ESP32 IP Address

1. Open Serial Monitor (Tools → Serial Monitor)
1. Set baud rate to 115200
1. Reset ESP32 (press EN button)
1. Note the IP address displayed

### 7. Update Web App

In `public/index.html`, change this line:

```javascript
const response = await fetch('http://192.168.1.100/setcolor', {
```

Replace `192.168.1.100` with your ESP32’s IP address.

## 🎯 Testing

1. Upload code to ESP32
1. Open Serial Monitor to see startup sequence
1. Visit ESP32’s IP in browser to see status page
1. Use web interface to control LEDs

## 🔍 Troubleshooting

- **No WiFi**: Check SSID/password, ensure 2.4GHz network
- **LEDs not working**: Check wiring, power supply, LED count
- **Wrong colors**: Try different COLOR_ORDER (RGB, GRB, BRG)
- **Web app can’t connect**: Update IP address, check firewall

## 📡 API Endpoints

- `http://ESP32_IP/` - Status page
- `http://ESP32_IP/setcolor` - POST color data
- `http://ESP32_IP/status` - GET current status
