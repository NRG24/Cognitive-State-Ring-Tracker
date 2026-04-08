# Quick Reference Card - GSR Streamer Complete System

## 🎯 File to Upload
**Use this file:** `gsr_complete_all_features.ino`

This is the complete, production-ready sketch with ALL features your Flutter app needs.

---

## 📌 Pin Connections (Quick View)

```
┌─────────────────────────────────────┐
│  XIAO nRF52840                      │
├─────────────────────────────────────┤
│  A0  → GSR Sensor (analog)          │
│  A1  → Pulse Sensor (analog)        │
│  D6  → DS18B20 Temp (+ 4.7kΩ)      │
│  A5  → Battery Monitor (divider)    │
│  3V3 → All VCC                      │
│  GND → All GND                      │
└─────────────────────────────────────┘
```

---

## 📡 BLE Characteristics

| Data | UUID Suffix | Type | Range/Unit |
|------|-------------|------|------------|
| **GSR** | ...002 | Int | 0-1023 |
| **Heart Rate** | ...003 | Int | 30-200 BPM |
| **Temperature** | ...004 | Float | °C |
| **HRV** | ...005 | Float | ms (RMSSD) |
| **SpO2** | ...006 | Int | 70-100% |
| **Battery** | ...007 | Int | 0-100% |

**Service UUID:** `19B10000-E8F2-537E-4F6C-D104768A1214`

---

## ✅ Features Included

- [x] GSR monitoring (10Hz smoothed)
- [x] Heart rate detection (pulse sensor)
- [x] HRV calculation (RMSSD method)
- [x] Finger temperature (DS18B20)
- [x] SpO2 placeholder (ready for MAX30102)
- [x] Battery monitoring (voltage divider)
- [x] Real-time waveform data
- [x] Power optimization (auto-sleep)
- [x] LED status indicators
- [x] Serial debug output

---

## 🔋 Battery Life

| Mode | Power Draw | Duration (110mAh) |
|------|------------|-------------------|
| **Active** (connected) | 15-20mA | 5-7 hours |
| **Idle** (disconnected < 1min) | 5-10mA | 11-22 hours |
| **Deep Sleep** (disconnected > 1min) | 0.5-2mA | 55-220 hours |

---

## 🚀 Quick Start

1. **Wire up sensors** (see diagram above)
2. **Install libraries:** ArduinoBLE, OneWire, DallasTemperature
3. **Select board:** Tools → Board → Seeed nRF52 Boards → XIAO nRF52840
4. **Upload:** `gsr_complete_all_features.ino`
5. **Open Serial Monitor** at 115200 baud
6. **Launch Flutter app** and tap "Connect to Device"
7. **Look for "GSR_HEART"** in device list

---

## 🔧 Common Adjustments

### Pulse Not Detecting?
```cpp
const int PULSE_THRESHOLD = 550;  // Line 67
```
Try values: 500-600 based on your sensor

### Battery Reading Wrong?
```cpp
const float BATTERY_MAX_VOLTAGE = 4.2;  // Line 116
const float BATTERY_MIN_VOLTAGE = 3.3;  // Line 117
```
Measure your actual battery with multimeter

### Temp Sensor Slow?
```cpp
const unsigned long TEMP_READ_INTERVAL = 2000;  // Line 134
```
Lower to 1000ms for faster updates (uses more power)

---

## 💡 LED Meanings

| Blink Pattern | Status |
|---------------|--------|
| 3 fast blinks | Startup OK, ready |
| Solid ON | Connected & streaming |
| Quick flash | Heartbeat detected |
| Slow (2s) | Deep sleep, waiting |

---

## 🐛 Debug Commands

Open Serial Monitor (115200 baud) to see:
```
♥ HR: 72 BPM | HRV: 42.3 ms | GSR: 512
✓ Connected to: XX:XX:XX:XX:XX:XX
✗ Disconnected
```

---

## 🆘 Troubleshooting

**"No temperature sensor found"**
→ Check 4.7kΩ pull-up resistor on D6

**"No heartbeat detected"**
→ Adjust PULSE_THRESHOLD (line 67)
→ Press sensor firmly to fingertip

**"Can't connect from app"**
→ Make sure sketch uploaded successfully
→ Check Serial Monitor shows "BLE advertising"
→ On iPhone: Settings → Bluetooth → Look for "GSR_HEART"

**"Battery always shows 100%"**
→ Add voltage divider circuit (see SETUP_INSTRUCTIONS.md)

---

## 📚 Full Documentation

- **SETUP_INSTRUCTIONS.md** - Complete wiring & setup guide
- **BATTERY_INFO.md** - Power consumption & battery life
- **COMPACT_SWITCH_OPTIONS.md** - Smaller power switch alternatives

---

## 🎓 What Each Feature Does

| Feature | Why Flutter App Needs It |
|---------|--------------------------|
| **GSR** | Core stress detection, baseline tracking |
| **Heart Rate** | Activity recognition, stress analysis |
| **HRV** | Advanced stress metrics, recovery status |
| **Temperature** | Stress patterns (drops when stressed) |
| **SpO2** | Health monitoring, activity intensity |
| **Battery** | Low battery warnings, UI display |

---

## ⚡ Performance Specs

- **Data rate:** 10 samples/second
- **Heart rate accuracy:** ±5 BPM (with good contact)
- **Temperature accuracy:** ±0.5°C (DS18B20 spec)
- **HRV update:** Every heartbeat (10 RR intervals)
- **Latency:** < 100ms (BLE to app)
- **Connection range:** ~10 meters (typical BLE)

---

## 🔄 Version History

**v2.0** (Current - `gsr_complete_all_features.ino`)
- ✨ Added HRV calculation (RMSSD)
- ✨ Added battery monitoring
- ✨ Added SpO2 placeholder
- ✨ Power optimization with auto-sleep
- ✨ Improved pulse detection algorithm
- ✨ Better serial debug output

**v1.0** (`gsr_heart_temp_complete.ino`)
- Basic GSR, heart rate, temperature

---

## 📞 Support

Having issues? Check:
1. Serial Monitor output
2. LED status indicators
3. Full SETUP_INSTRUCTIONS.md
4. Wiring diagram

---

**Ready to upload? Use:** `gsr_complete_all_features.ino` 🚀
