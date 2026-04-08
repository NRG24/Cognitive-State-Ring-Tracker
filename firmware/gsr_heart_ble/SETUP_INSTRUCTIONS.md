# XIAO nRF52840 Complete Biometric System Setup

## 📋 Overview

This is the **complete Arduino sketch** that provides ALL features for the GSR Streamer Flutter app:
- ✅ GSR (Galvanic Skin Response) monitoring
- ✅ Heart Rate with pulse detection
- ✅ Heart Rate Variability (HRV/RMSSD)
- ✅ Finger Temperature (DS18B20)
- ✅ SpO2 Blood Oxygen (optional MAX30102)
- ✅ Battery level monitoring
- ✅ Real-time waveform data
- ✅ Power optimization with auto-sleep

---

# XIAO nRF52840 BLE GSR Sensor Setup Instructions

## Hardware Requirements

### Required Components:
1. **XIAO nRF52840 Board** - Main microcontroller
2. **GSR Sensor** - Galvanic Skin Response sensor
3. **Pulse Sensor** - Heart rate detection
4. **DS18B20 Temperature Sensor** - Waterproof version for finger temp
5. **4.7kΩ Resistor** - Pull-up for DS18B20
6. **Battery Components** (for battery monitoring):
   - 2x 10kΩ resistors (voltage divider)
   - 110-500mAh LiPo battery
7. **Breadboard and jumper wires**

### Optional Components:
- **MAX30102** - For accurate SpO2 blood oxygen measurement

---

## Complete Wiring Diagram

```
XIAO nRF52840 Pin Connections:
╔════════════════════════════════════════════════════════════╗
║ SENSOR CONNECTIONS                                         ║
╠════════════════════════════════════════════════════════════╣
║ A0 ──────► GSR Sensor (Signal)                             ║
║ A1 ──────► Pulse Sensor (Signal)                           ║
║ D6 ──────► DS18B20 (Data) + 4.7kΩ pull-up to 3.3V         ║
║ A5 ──────► Battery Voltage (via voltage divider)           ║
║ SDA ─────► MAX30102 SDA (optional)                         ║
║ SCL ─────► MAX30102 SCL (optional)                         ║
║ 3V3 ─────► All sensors VCC                                 ║
║ GND ─────► All sensors GND                                 ║
║ LED ─────► Built-in (connection indicator)                 ║
╚════════════════════════════════════════════════════════════╝
```

### Battery Voltage Monitor Circuit

```
Battery Voltage Divider (for battery % monitoring):

    Battery+ (4.2V max)
        │
       ┌┴┐
       │R│ 10kΩ
       └┬┘
        ├────────► A5 (XIAO) [measures ~2.1V max]
        │
       ┌┴┐
       │R│ 10kΩ
       └┬┘
        │
       GND

This divides battery voltage by 2, so XIAO can safely measure it.
```

### Detailed Connections

#### GSR Sensor
- **VCC** → XIAO 3.3V
- **GND** → XIAO GND
- **Signal** → XIAO A0

#### Pulse Sensor
- **Red Wire (VCC)** → XIAO 3.3V
- **Black Wire (GND)** → XIAO GND
- **Purple Wire (Signal)** → XIAO A1

#### DS18B20 Temperature Sensor
- **Red Wire (VCC)** → XIAO 3.3V
- **Black Wire (GND)** → XIAO GND
- **Yellow Wire (Data)** → XIAO D6
- **4.7kΩ Resistor** → Between Data (D6) and VCC (3.3V)

```
Temperature Sensor Pull-up Resistor:
       3.3V
        │
       ┌┴┐
       │R│ 4.7kΩ
       └┬┘
        │
        ├────────► D6 (XIAO)
        │
      ──┴──  DS18B20 Data Pin
```

## Required Arduino Libraries

Install these libraries via Arduino IDE Library Manager:

1. **ArduinoBLE** (version 1.3.6 or later)
   - For: BLE communication
   - Install: `Sketch > Include Library > Manage Libraries > Search "ArduinoBLE"`

2. **OneWire** (version 2.3.7 or later)
   - For: DS18B20 communication protocol
   - Install: `Sketch > Include Library > Manage Libraries > Search "OneWire"`

3. **DallasTemperature** (version 3.9.0 or later)
   - For: DS18B20 temperature sensor interface
   - Install: `Sketch > Include Library > Manage Libraries > Search "DallasTemperature"`

## Board Setup in Arduino IDE

1. **Install XIAO nRF52840 Board Support**
   - Open Arduino IDE
   - Go to `File > Preferences`
   - Add to "Additional Boards Manager URLs":
     ```
     https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
     ```
   - Go to `Tools > Board > Boards Manager`
   - Search for "Seeed nRF52 Boards"
   - Install "Seeed nRF52 Boards" by Seeed Studio

2. **Select Board**
   - `Tools > Board > Seeed nRF52 Boards > Seeed XIAO BLE - nRF52840`

3. **Select Port**
   - Connect XIAO via USB-C
   - `Tools > Port > [Select your XIAO port]`

## Upload Instructions

1. **Open the sketch**
   - Open `gsr_heart_temp_complete.ino` in Arduino IDE

2. **Verify Hardware is Connected**
   - Connect all sensors as per wiring diagram
   - Ensure 4.7kΩ pull-up resistor is in place for DS18B20

3. **Upload**
   - Click "Upload" button (→) in Arduino IDE
   - Wait for "Done uploading" message

4. **Monitor Serial Output**
   - Open Serial Monitor (`Tools > Serial Monitor`)
   - Set baud rate to **115200**
   - You should see:
     ```
     ═══════════════════════════════════════════════
     GSR Streamer - Complete Biometric System v2.0
     ═══════════════════════════════════════════════
     Initializing DS18B20 temperature sensor... Found 1 device(s)
     Initializing BLE... OK
     BLE advertising started as 'GSR_HEART'
     Waiting for connection...
     ═══════════════════════════════════════════════
     ```

---

## 📊 What Data Does This Send?

The complete sketch sends **6 data streams** to your Flutter app:

### 1. **GSR (Galvanic Skin Response)**
- **UUID:** 19B10002-E8F2-537E-4F6C-D104768A1214
- **Type:** Integer (0-1023)
- **Update Rate:** 10 times/second
- **What it measures:** Skin conductance (stress, arousal, emotion)
- **Used for:** Stress detection, trigger identification, baseline tracking

### 2. **Heart Rate**
- **UUID:** 19B10003-E8F2-537E-4F6C-D104768A1214
- **Type:** Integer (BPM)
- **Range:** 30-200 BPM
- **Update Rate:** Real-time (on each heartbeat)
- **What it measures:** Beats per minute with 5-beat smoothing
- **Used for:** Activity recognition, stress analysis, waveform display

### 3. **Temperature**
- **UUID:** 19B10004-E8F2-537E-4F6C-D104768A1214
- **Type:** Float (°Celsius)
- **Range:** 20-45°C typical
- **Update Rate:** Every 2 seconds
- **What it measures:** Finger temperature via DS18B20
- **Used for:** Stress patterns, health monitoring, waveform display

### 4. **HRV (Heart Rate Variability)**
- **UUID:** 19B10005-E8F2-537E-4F6C-D104768A1214
- **Type:** Float (milliseconds)
- **Calculation:** RMSSD (Root Mean Square of Successive Differences)
- **Update Rate:** After each heartbeat (calculated from last 10 RR intervals)
- **What it measures:** Variation between heartbeats
- **Used for:** Stress level, recovery status, autonomic balance
- **Note:** Higher HRV = better (more relaxed/recovered)

### 5. **SpO2 (Blood Oxygen)**
- **UUID:** 19B10006-E8F2-537E-4F6C-D104768A1214
- **Type:** Integer (%)
- **Range:** 70-100%
- **Update Rate:** Continuous (if MAX30102 connected)
- **What it measures:** Blood oxygen saturation
- **Used for:** Health monitoring, activity recognition
- **Note:** Currently simulated (98%) - requires MAX30102 for real data

### 6. **Battery Level**
- **UUID:** 19B10007-E8F2-537E-4F6C-D104768A1214
- **Type:** Integer (%)
- **Range:** 0-100%
- **Update Rate:** Every 10 seconds
- **What it measures:** LiPo battery charge percentage
- **Used for:** Low battery warnings, power management

---

## ⚡ Power Management Features

The sketch includes **smart power optimization**:

### When Connected (Active Mode):
- All sensors active
- Data sent every 100ms
- LED on solid
- Power draw: ~15-20mA

### When Disconnected (Idle Mode):
- Light sleep after 0 seconds
- Still advertising for connections
- Slower polling
- Power draw: ~5-10mA

### Deep Sleep (After 60 seconds idle):
- Ultra-low power mode
- Only BLE advertising active
- Slow "heartbeat" LED (2 second blink)
- Power draw: ~0.5-2mA
- **Battery life:** Days to weeks!

---

## LED Status Indicators

| Pattern | Meaning |
|---------|---------|
| **3 Fast Blinks** (startup) | System ready, BLE advertising |
| **Solid ON** | Connected to app, streaming data |
| **Quick pulse with heartbeat** | Heart beat detected |
| **Slow blink (2s)** | Deep sleep mode, waiting for connection |
| **Rapid flashing** | Error - check Serial Monitor |

---

## 🔧 Calibration & Tuning

### Pulse Sensor Threshold
If heart rate is not detecting properly:

```cpp
const int PULSE_THRESHOLD = 550;  // Line 67 - adjust this!
```

- **Too low (< 500):** False beats, erratic readings
- **Too high (> 650):** Misses beats
- **Sweet spot:** Usually 520-580 depending on your sensor

**How to calibrate:**
1. Open Serial Monitor
2. Watch raw pulse values
3. Set threshold to ~70% of peak value

### Battery Voltage Calibration

## Troubleshooting

### Temperature Sensor Not Detected
**Symptoms**: "Temperature sensors found: 0"

**Solutions**:
1. Check 4.7kΩ pull-up resistor is connected between Data (D6) and 3.3V
2. Verify DS18B20 wiring (Red=VCC, Black=GND, Yellow=Data)
3. Try a different DS18B20 sensor (they can fail)
4. Measure resistance between Data and VCC pins (should be ~4.7kΩ)

### No Heart Rate Detection
**Symptoms**: BPM stays at 0

**Solutions**:
1. Ensure Pulse Sensor is firmly pressed against fingertip
2. Clean the sensor surface
3. Adjust `PULSE_THRESHOLD` in code (line 21) - try values 500-600
4. Check Serial Monitor for "♥ Beat detected!" messages
5. Ensure good skin contact with the sensor

### GSR Values Stuck or Not Changing
**Symptoms**: GSR reads constant value

**Solutions**:
1. Attach GSR electrodes to clean, dry skin (fingers work best)
2. Ensure electrodes have good contact
3. Wait 30-60 seconds for sensor to stabilize
4. Check that GSR sensor is powered (3.3V)

### BLE Connection Issues
**Symptoms**: Flutter app can't find device

**Solutions**:
1. Verify device name is "GSR_HEART" in Serial Monitor
2. Check UUIDs match between Arduino and Flutter app
3. Reset XIAO (press reset button)
4. Move phone/computer closer to XIAO
5. Turn Bluetooth off/on on phone
6. Close other BLE apps that might interfere

### Temperature Reading Errors
**Symptoms**: "Warning: Temperature sensor read error"

**Solutions**:
1. Check DS18B20 is genuine (many clones exist)
2. Verify pull-up resistor value (4.7kΩ)
3. Shorten wires if connection is long (>1 meter)
4. Try a different digital pin
5. Power cycle the XIAO

## Sensor Calibration

### Pulse Sensor Auto-Calibration
The sketch automatically calibrates the pulse threshold every 5 seconds based on signal range. For manual tuning:
- Open Serial Monitor
- Observe signal min/max values
- Set `PULSE_THRESHOLD` to midpoint between min and max

### GSR Baseline
GSR values vary by person and conditions. Typical ranges:
- **Dry skin**: 200-400
- **Normal**: 400-600
- **Sweaty/stressed**: 600-800+

### Temperature Reference
- **Normal finger temp**: 30-35°C (86-95°F)
- **Cold hands**: 25-30°C (77-86°F)
- **Warm/active**: 35-37°C (95-99°F)

## Data Stream Format

### BLE Characteristics

| Characteristic | UUID | Data Type | Range | Update Rate |
|---------------|------|-----------|-------|-------------|
| GSR | 19B10002 | Int32 (LE) | 0-1023 | 20 Hz |
| Heart Rate | 19B10003 | Int32 (LE) | 30-200 BPM | 20 Hz |
| Temperature | 19B10004 | Float32 (LE) | 20-45°C | 0.5 Hz |

### Serial Debug Output Format
```
GSR: 512 | BPM: 72 | Temp: 32.5°C
♥ Beat detected! BPM: 75
```

## Power Consumption

- **Scanning**: ~15 mA
- **Connected & Streaming**: ~20 mA
- **Battery Life** (500 mAh LiPo): ~20-25 hours continuous use

## Safety Notes

⚠️ **Important Safety Information**:
- This is a DIY educational device, **NOT** medical equipment
- Do not use for medical diagnosis or treatment
- Ensure all connections are insulated to prevent shorts
- Use only 3.3V power supply (XIAO's voltage)
- Do not submerge electronics (only DS18B20 probe if waterproof)

## Next Steps

Once hardware is set up and sketch is uploaded:

1. **Verify Serial Output**: Confirm all sensors are detected
2. **Test Flutter App**: Run `flutter run -d iPhone` or `flutter run -d macos`
3. **Connect**: Tap "Connect to Device" in app
4. **Monitor Data**: Watch real-time waveforms update
5. **Calibrate**: Allow 2-3 minutes for sensors to stabilize

## Support

For issues or questions:
- Check Serial Monitor output for error messages
- Verify all wiring matches diagram
- Ensure libraries are up to date
- Try each sensor individually to isolate problems
