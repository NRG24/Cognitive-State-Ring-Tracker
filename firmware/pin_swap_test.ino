/*
  Pin Swap and Wiring Test for MAX30102
  Tests if SDA/SCL pins are swapped or if there are wiring issues
*/

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("=== PIN SWAP DIAGNOSTIC TEST ===");
  Serial.println("Testing if SDA/SCL pins are swapped...");
  
  // Test GSR first
  Serial.println("\n1. GSR Test (should work):");
  int gsr = analogRead(A0);
  Serial.print("GSR reading: ");
  Serial.println(gsr);
  
  // Test normal pin configuration (Pin 4=SDA, Pin 5=SCL)
  Serial.println("\n2. Testing NORMAL pin config (Pin 4=SDA, Pin 5=SCL):");
  testI2CConfig(4, 5, "NORMAL");
  
  // Test swapped pin configuration (Pin 5=SDA, Pin 4=SCL)  
  Serial.println("\n3. Testing SWAPPED pin config (Pin 5=SDA, Pin 4=SCL):");
  testI2CConfig(5, 4, "SWAPPED");
  
  // Test other possible pin combinations
  Serial.println("\n4. Testing other pin combinations:");
  testOtherPins();
  
  Serial.println("\n5. Final recommendation:");
  giveRecommendation();
}

void loop() {
  // Simple continuous test
  static unsigned long lastTest = 0;
  
  if (millis() - lastTest > 3000) {
    lastTest = millis();
    
    Serial.println("\n--- Quick Test ---");
    int gsr = analogRead(A0);
    Serial.print("GSR: ");
    Serial.println(gsr);
    
    // Test both configurations quickly
    Wire.begin();
    Wire.beginTransmission(0x57);
    byte error1 = Wire.endTransmission();
    
    if (error1 == 0) {
      Serial.println("✓ Normal config working!");
    } else {
      Serial.println("✗ Normal config not working");
    }
  }
}

void testI2CConfig(int sdaPin, int sclPin, String configName) {
  Serial.print("Testing ");
  Serial.print(configName);
  Serial.print(" config - SDA:");
  Serial.print(sdaPin);
  Serial.print(" SCL:");
  Serial.println(sclPin);
  
  Wire.begin();
  delay(100);
  
  // Test specific MAX30102 address with detailed error reporting
  Serial.print("  Trying MAX30102 at 0x57... ");
  Wire.beginTransmission(0x57);
  byte error = Wire.endTransmission();
  
  Serial.print("Raw error code: ");
  Serial.print(error);
  Serial.print(" = ");
  
  switch(error) {
    case 0:
      Serial.println("SUCCESS - Device ACK'd");
      
      // Try to read device ID to double-confirm
      Wire.beginTransmission(0x57);
      Wire.write(0xFF); // Device ID register
      byte writeError = Wire.endTransmission(false);
      
      if (writeError == 0) {
        Wire.requestFrom(0x57, 1);
        if (Wire.available()) {
          byte deviceID = Wire.read();
          Serial.print("    Device ID: 0x");
          Serial.print(deviceID, HEX);
          if (deviceID == 0x15) {
            Serial.println(" ✓ CONFIRMED MAX30102!");
          } else {
            Serial.println(" ✗ Wrong device ID");
          }
        } else {
          Serial.println("    No data received");
        }
      } else {
        Serial.print("    Write error: ");
        Serial.println(writeError);
      }
      break;
      
    case 1:
      Serial.println("ERROR - Data too long for buffer");
      break;
      
    case 2:
      Serial.println("ERROR - NACK on address (device not found)");
      break;
      
    case 3:
      Serial.println("ERROR - NACK on data");
      break;
      
    case 4:
      Serial.println("ERROR - Other I2C error");
      break;
      
    default:
      Serial.println("ERROR - Unknown error code");
      break;
  }
  
  // Full bus scan with error details
  Serial.println("  Full I2C bus scan:");
  int devicesFound = 0;
  for(byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte scanError = Wire.endTransmission();
    
    if (scanError == 0) {
      Serial.print("    Device at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      if (address == 0x57) Serial.print(" (MAX30102)");
      Serial.println();
      devicesFound++;
    }
  }
  
  Serial.print("  Total devices found: ");
  Serial.println(devicesFound);
  Serial.println();
}

void testOtherPins() {
  Serial.println("Testing if MAX30102 is on different pins...");
  
  // Test pin states to see if there are shorts
  Serial.println("Pin state analysis:");
  for (int pin = 0; pin <= 10; pin++) {
    pinMode(pin, INPUT_PULLUP);
    delay(1);
    int state = digitalRead(pin);
    Serial.print("Pin ");
    Serial.print(pin);
    Serial.print(": ");
    Serial.print(state);
    if (state == 0) {
      Serial.print(" (LOW - possible short to ground)");
    }
    Serial.println();
  }
}

void giveRecommendation() {
  Serial.println("RECOMMENDATIONS:");
  Serial.println("1. Check which test found the MAX30102");
  Serial.println("2. If SWAPPED config worked, swap your SDA/SCL wires");
  Serial.println("3. If no config worked, check these:");
  Serial.println("   - Power connections (VCC to 3.3V, GND to GND)");
  Serial.println("   - Solder joints on SDA/SCL pins");
  Serial.println("   - Try different MAX30102 module");
  Serial.println("4. If device found when wire disconnected:");
  Serial.println("   - There's likely a short circuit");
  Serial.println("   - Check for solder bridges");
  Serial.println("   - Verify pin connections");
