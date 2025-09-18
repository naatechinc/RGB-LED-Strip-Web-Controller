# 🌐 API Documentation

## 📡 ESP32 Endpoints

### Base URL
```
http://[ESP32_IP_ADDRESS]/
```

## 🎨 Color Control

### POST /setcolor
Send color and animation data to LED strip.

**Request:**
```http
POST /setcolor
Content-Type: application/json

{
  "r": 255,           // Red value (0-255)
  "g": 128,           // Green value (0-255)
  "b": 64,            // Blue value (0-255)
  "speed": 50,        // Animation speed (1-100)
  "mode": "cosmic",   // Animation mode
  "timestamp": 1640995200000
}
```

**Response (Success):**
```json
{
  "status": "success",
  "r": 255,
  "g": 128,
  "b": 64,
  "speed": 50
}
```

**Response (Error):**
```json
{ "error": "Invalid JSON" }
```

## 📊 Status Information

### GET /status
**Request:**
```http
GET /status
```
**Response:**
```json
{
  "connected": true,
  "led_count": 144,
  "current_color": [255, 128, 64],
  "speed": 50,
  "mode": "solid",
  "uptime": 123456789,
  "free_heap": 234567,
  "ip": "192.168.1.100"
}
```

## 🎭 Animation Modes

|Mode       |Description                     |
|-----------|--------------------------------|
|`solid`    |Static color display            |
|`rainbow`  |Smooth rainbow animation        |
|`breathing`|Fade in/out effect              |
|`cosmic`   |Special cosmic-themed animations|
|`strobe`   |Fast flashing effects           |

## ⚡ Speed Control

Speed values range from 1-100:
- **1**: Slowest animation (100ms delay)
- **50**: Medium speed (55ms delay)
- **100**: Fastest animation (10ms delay)

## 🔧 CORS Headers
```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Headers: Content-Type
```

## 💡 Usage Examples

### JavaScript Fetch
```javascript
const colorData = { r: 255, g: 0, b: 128, speed: 75, mode: "cosmic" };
fetch('http://192.168.1.100/setcolor', {
  method: 'POST', headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify(colorData)
});
fetch('http://192.168.1.100/status')
  .then(r => r.json())
  .then(console.log);
```

### Python Example
```python
import requests
color_data = {"r": 255, "g": 0, "b": 128, "speed": 75, "mode": "cosmic"}
requests.post('http://192.168.1.100/setcolor', json=color_data)
print(requests.get('http://192.168.1.100/status').json())
```

### cURL Examples
```bash
curl -X POST http://192.168.1.100/setcolor -H "Content-Type: application/json" -d '{"r":255,"g":0,"b":128,"speed":75,"mode":"cosmic"}'
curl http://192.168.1.100/status
```
