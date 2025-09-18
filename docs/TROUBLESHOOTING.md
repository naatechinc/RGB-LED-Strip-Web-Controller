# 🔧 Troubleshooting Guide

## 🚨 Common Issues

### ESP32 Won't Connect to WiFi
**Solutions:**
- Verify SSID/password (2.4GHz only)
- Move ESP32 closer to router
- Try mobile hotspot for testing
- Reset ESP32

### LEDs Not Responding
**Electrical:**
- Check wiring and common ground
- Ensure 5V PSU capacity (≈60mA/LED)
- Test with fewer LEDs
**Code:**
- `NUM_LEDS` matches strip
- `LED_PIN` correct (GPIO 18 default)
- Try different `COLOR_ORDER` (RGB/GRB/BRG)

### Wrong Colors
Change `COLOR_ORDER`:
```cpp
#define COLOR_ORDER RGB  // or GRB, BRG, BGR
```

### Web App Can’t Connect
- Device and ESP32 on same WiFi
- Update IP in `public/index.html`
- Consider static DHCP reservation
- Check firewall

### Quick Test Snippet
```cpp
fill_solid(leds, NUM_LEDS, CRGB::Red); FastLED.show(); delay(1000);
fill_solid(leds, NUM_LEDS, CRGB::Green); FastLED.show(); delay(1000);
fill_solid(leds, NUM_LEDS, CRGB::Blue); FastLED.show();
```

## 📋 Checklists

**Hardware**
- [ ] 5V power OK  •  [ ] Common GND  •  [ ] Data to GPIO 18

**Software**
- [ ] Libraries installed  •  [ ] NUM_LEDS correct  •  [ ] COLOR_ORDER correct

**Network**
- [ ] ESP32 has IP  •  [ ] Browser can reach `/status`

