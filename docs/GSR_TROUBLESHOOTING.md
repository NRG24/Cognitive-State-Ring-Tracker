# GSR Sensor Troubleshooting Guide

## Symptom: Sensor Reading ~30 and Not Responsive

### Possible Causes

#### 1. **Poor Electrode Contact** (Most Common)
- **Symptoms**: Constant low reading (~30), no variation with skin contact
- **Solutions**:
  - Clean the electrode surfaces with isopropyl alcohol
  - Ensure electrodes are making firm contact with skin
  - Check that electrode gel/paste hasn't dried out
  - Try applying slight moisture to fingertips (not too wet)
  - Verify electrodes are touching two different fingers

#### 2. **Dry Skin**
- **Symptoms**: Very low baseline reading, minimal response
- **Solutions**:
  - Wash hands with water (no soap) to add moisture
  - Wait 2-3 minutes for natural skin moisture to return
  - Apply small amount of electrode gel if available
  - Avoid hand sanitizer/lotion before use

#### 3. **Wire Connection Issues**
- **Symptoms**: Constant reading, no change even when touching wires
- **Solutions**:
  - Check all wire connections to XIAO board
  - Verify A0 pin connection is secure
  - Test continuity with multimeter if available
  - Look for broken/frayed wires
  - Ensure ground connection is solid

#### 4. **Sensor Circuit Problem**
- **Symptoms**: Fixed reading regardless of contact
- **Solutions**:
  - Verify 3.3V power supply to sensor circuit
  - Check resistor values (should be 2-10MΩ for voltage divider)
  - Ensure no short circuits on breadboard
  - Test with multimeter: should read 0-3.3V on A0 pin

#### 5. **Analog Pin Damage**
- **Symptoms**: Always reads same value
- **Solutions**:
  - Try different analog pin (A1, A2, A3)
  - Update Arduino code to use different pin
  - Test A0 with simple potentiometer to verify pin works

### Quick Diagnostic Tests

#### Test 1: Touch Test
```
1. Touch electrodes together directly
2. Should read very low value (near 0)
3. Separate electrodes
4. Should read high value (>1000)
```

#### Test 2: Resistance Test
```
With multimeter:
1. Measure resistance between electrodes
2. Should be >10MΩ in air
3. Touch electrodes to fingertips
4. Should drop to 50-500kΩ
```

#### Test 3: Voltage Test
```
With multimeter on A0 pin:
1. No contact: Should read ~3.3V
2. Light contact: Should read 1-2V
3. Firm contact: Should read 0.3-1V
```

### Hardware Verification Checklist

- [ ] Electrodes are clean and dry
- [ ] All wires are securely connected
- [ ] No visible damage to sensor/wires
- [ ] XIAO board is powered (LED on)
- [ ] A0 pin not damaged (test with potentiometer)
- [ ] Ground connection is solid
- [ ] 3.3V supply is stable
- [ ] No short circuits on breadboard

### Code-Based Diagnostics

I can add a diagnostic mode to help identify the issue. This will:
1. Show raw ADC values (0-1023)
2. Display voltage readings
3. Show baseline stability
4. Flag abnormal readings

Would you like me to add a diagnostic mode to the Arduino code?

### Temporary Workarounds

While troubleshooting, you can:
1. **Use simulated data**: I can add a demo mode that generates realistic GSR patterns
2. **Test with other sensors**: Focus on heart rate/SpO2/temperature monitoring
3. **Log sensor readings**: Capture data to identify if issue is intermittent

### Replacement Options

If sensor is damaged:
- **Basic GSR sensor**: $5-15 on Amazon/AliExpress
- **Grove GSR sensor**: ~$20 (plug-and-play)
- **AD8232 alternative**: Can measure skin conductance

### My Recommendations

**Try in this order:**
1. **Clean electrodes** - Most likely fix
2. **Check connections** - Look for loose wires
3. **Test with wet fingers** - Rule out dry skin
4. **Try different analog pin** - Rule out pin damage
5. **Use diagnostic mode** - Identify if hardware or software issue

Would you like me to:
1. Add diagnostic mode to Arduino code?
2. Add simulated data mode for testing without hardware?
3. Modify code to use different analog pin?
4. Something else?
