# GSR Sensor Issue - Quick Fix Guide

## Your Symptom: Reading ~30 and Not Responsive

This is a common issue! Here's what to do:

## Immediate Steps to Try (In Order)

### 1. Clean the Electrodes (5 minutes) ⭐ MOST LIKELY FIX
```
Materials needed: Isopropyl alcohol (rubbing alcohol) or water
1. Disconnect sensor from XIAO
2. Wipe electrodes with alcohol/damp cloth
3. Let dry completely
4. Reconnect and test
```

### 2. Check Wire Connections (2 minutes)
```
Look for:
- Loose wires at A0 pin
- Loose ground connection  
- Frayed/broken wires
- Secure all connections and test
```

### 3. Test with Wet Fingers (1 minute)
```
1. Slightly dampen fingertips with water
2. Place on electrodes with firm pressure
3. Reading should change significantly
4. If no change → hardware issue
```

### 4. Upload Diagnostic Code (10 minutes)
```
File: xiao_ble_gsr_diagnostic.ino

This will:
✓ Show raw ADC values (0-1023)
✓ Display voltage readings
✓ Detect stuck readings
✓ Provide specific troubleshooting advice
✓ Check electrode stability

Upload it and watch Serial Monitor at 9600 baud
```

## What the Diagnostic Output Tells You

### If you see:
```
Raw ADC: 30 | Voltage: 0.096V
⚠️  POSSIBLE ISSUE: Reading ~30 suggests poor contact or damaged sensor
Range: 28 - 32 | Stability: STUCK - No variation!
```
**= Sensor is definitely malfunctioning**

**Most likely causes:**
1. **Dirty electrodes** (80% of cases)
2. **Broken wire** (15% of cases)
3. **Damaged sensor** (5% of cases)

### Normal readings look like:
```
Raw ADC: 450 | Voltage: 1.445V  
Range: 420 - 510 | Stability: Normal variation
```

## Hardware Test Without Arduino

### Multimeter Test:
1. Set multimeter to resistance (Ω) mode
2. Touch probes to electrodes
3. **Should read**: >10MΩ (mega-ohms)
4. Touch electrodes to fingertips
5. **Should drop to**: 50-500kΩ (kilo-ohms)

**If resistance doesn't change = Bad sensor**

## Quick Workaround Options

While you troubleshoot or wait for replacement:

### Option 1: Use Diagnostic Mode
- Shows all sensor data in Serial Monitor
- Can still test heart rate and SpO2
- GSR will show as faulty but won't crash

### Option 2: Disable GSR Temporarily
I can modify the code to:
- Skip GSR readings
- Focus on heart rate + SpO2 + temperature
- Still get useful biometric data

### Option 3: Simulate GSR Data
- Generate realistic GSR patterns
- Test the Flutter app fully
- Good for development/demos

## Replacement Sensors

If sensor is truly damaged:

**Budget Option ($5-10)**:
- Generic GSR sensor on Amazon/AliExpress
- Search: "GSR galvanic skin response sensor"

**Quality Option ($15-25)**:
- Grove GSR Sensor (Seeed Studio)
- Plug-and-play with example code

**DIY Option ($2-5)**:
- 2x stainless steel screws as electrodes
- 10MΩ resistor
- Simple voltage divider circuit

## Next Steps

**Tell me which you want to try:**
1. Upload diagnostic code and share Serial Monitor output
2. Try a different analog pin (A1, A2, A3)
3. Add simulated GSR data for testing
4. Disable GSR temporarily to focus on other sensors
5. Something else

I'm here to help troubleshoot!
