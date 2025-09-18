# 🌈✨ Cosmic LED Controller ✨🌈

A stunning web-based controller for WS2812B LED strips with cosmic-themed UI featuring cotton candy sunset colors and electric neon aesthetics.

## 🚀 Quick Start

### 1. Hardware Wiring
- **🔴 VCC (Red)** → ESP32 **VIN**
- **⚪ GND (White)** → ESP32 **GND** 
- **🟢 DAT (Green)** → ESP32 **GPIO 18**

### 2. Upload Arduino Code
1. Open `arduino/esp32_led_controller.ino` in Arduino IDE
2. Install required libraries (see `arduino/libraries_needed.txt`)
3. Update WiFi credentials in the code
4. Upload to ESP32

### 3. Deploy Web App
1. Upload files to GitHub repository
2. Connect to Vercel for automatic deployment
3. Update ESP32 IP address in web app

## 📱 Features
- **Cosmic UI** with animated gradients
- **RGB Control** sliders (0-255)
- **Speed Control** for animations
- **500+ Color Palette** with cosmic presets
- **Mobile Responsive** design
- **Real-time LED Control** via WiFi

## 🔧 Configuration
Update these values in Arduino code:
```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
#define NUM_LEDS 144  // Your LED count
```

## 📚 Documentation

- `WIRING_GUIDE.md` - Detailed wiring instructions
- `docs/SETUP.md` - Complete setup guide
- `docs/API.md` - API documentation
- `docs/TROUBLESHOOTING.md` - Common issues and fixes

## 🎯 LED Strip Modes

- **Solid Color** - Static color display
- **Rainbow** - Smooth rainbow animation
- **Breathing** - Fade in/out effect
- **Cosmic** - Special cosmic animations
- **Strobe** - Fast flashing effects

Made with 💖 and ✨ cosmic energy ✨

Deployed via Vercel 🚀
