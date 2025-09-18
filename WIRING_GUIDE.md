# 🔌 ESP32 Wiring Guide

## Your LED Strip Wire Colors:
- **🔴 VCC (Red wire)** = Power (5V)
- **⚪ GND (White wire)** = Ground (0V)  
- **🟢 DAT (Green wire)** = Data signal

## ESP32 Connections:

### Method 1: USB Powered (for small LED strips <30 LEDs)
```
LED Strip          ESP32 Board
🔴 VCC (Red)   →   VIN (5V input)
⚪ GND (White)  →   GND (any ground pin)
🟢 DAT (Green) →   GPIO 18 (labeled as “18” or “D18”)
```

### Method 2: External Power (recommended for >30 LEDs)
```
LED Strip          Power Supply    ESP32 Board
🔴 VCC (Red)   →   +5V         →   VIN
⚪ GND (White)  →   GND         →   GND
🟢 DAT (Green) →               →   GPIO 18

Power Supply: 5V, minimum 2A (calculate 60mA per LED)
```

## 📍 ESP32 Pin Locations:
```
ESP32 DevKit V1 Layout:
┌─────────────┐
│    ESP32    │
3V3 │1          30│ VIN  ← 🔴 Red wire here
GND │2          29│ GND  ← ⚪ White wire here  
15 │3          28│ 13
2  │4          27│ 12
4  │5          26│ 14
16 │6          25│ 27
17 │7          24│ 26
5  │8          23│ 25
18 │9          22│ 33   ← 🟢 Green wire here (GPIO 18)
19 │10         21│ 32
└─────────────┘
```

## ⚠️ Important Notes:
1. **GPIO 18** is pin #9 on the left side
2. **VIN** accepts 5V when ESP32 is USB powered
3. **Always connect GND** between ESP32 and LED strip
4. For many LEDs, use external 5V power supply
5. Test with few LEDs first, then scale up

## 🔧 Alternative Data Pins:
If GPIO 18 doesn't work, try these pins and update the Arduino code:
- GPIO 16, 17, 19, 21, 22, 23, 25, 26, 27

Change this line in the Arduino code:
```cpp
#define LED_PIN 18  // Change to your chosen pin
```
