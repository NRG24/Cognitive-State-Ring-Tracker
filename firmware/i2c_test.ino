/*
  I2C and MAX30102 Focused Diagnostic
  Tests I2C communication and MAX30102 specifically
  Since GSR works, we know power and XIAO are fine
*/

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("=== Enhanced I2C and MAX30102 Diagnostic ===");
  Serial.println("GSR works, so testing I2C communication...");
  
  // Test GSR first to confirm everything basic works
  Serial.println("\n1. Testing GSR (should work):");
  int gsr = analogRead(A0);
  Serial.print("GSR reading: ");
  Serial.println(gsr);
  
  // Check pin states BEFORE I2C init
  Serial.println("\n2. Pin states BEFORE I2C init:");
  Serial.print("SDA (Pin 4) state: ");
  Serial.println(digitalRead(4));
  Serial.print("SCL (Pin 5) state: ");
  Serial.println(digitalRead(5));
  
  // Initialize I2C with explicit pins for XIAO nRF52840
  Serial.println("\n3. Initializing I2C...");
  Wire.begin();
  Serial.println("Standard I2C initialized");
  
  // Check pin states AFTER I2C init
  Serial.println("\n4. Pin states AFTER I2C init:");
  Serial.print("SDA (Pin 4) state: ");
  Serial.println(digitalRead(4));
  Serial.print("SCL (Pin 5) state: ");
  Serial.println(digitalRead(5));
  
  // Try different I2C configurations
  Serial.println("\n5. Testing different I2C speeds...");
  testDifferentI2CSpeeds();
  
  // Scan I2C bus
  Serial.println("\n6. Scanning I2C bus for devices...");
  scanI2C();
  
  // Try to communicate with MAX30102 directly
  Serial.println("\n7. Testing MAX30102 direct communication...");
  testMAX30102Direct();
  
  // Test pullup resistors
  Serial.println("\n8. Testing I2C pullup resistors...");
  testPullups();
}

void loop() {
  // Keep testing every 5 seconds
  static unsigned long lastTest = 0;
  
  if (millis() - lastTest > 5000) {
    lastTest = millis();
    
    Serial.println("\n--- Continuous Test ---");
    
    // GSR should always work
    int gsr = analogRead(A0);
    Serial.print("GSR: ");
    Serial.println(gsr);
    
    // Quick I2C scan
    Wire.beginTransmission(0x57); // MAX30102 address
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.println("✓ MAX30102 responding on I2C!");
      
      // Try to read device ID
      Wire.beginTransmission(0x57);
      Wire.write(0xFF); // Device ID register
      Wire.endTransmission(false);
      
      Wire.requestFrom(0x57, 1);
      if (Wire.available()) {
        byte deviceID = Wire.read();
        Serial.print("Device ID: 0x");
        Serial.println(deviceID, HEX);
        if (deviceID == 0x15) {
          Serial.println("✓ This is a MAX30102!");
        } else {
          Serial.println("✗ Wrong device ID for MAX30102");
        }
      }
    } else {
      Serial.print("✗ MAX30102 not responding. I2C error: ");
      Serial.println(error);
    }
  }
}

void scanI2C() {
  byte error, address;
  int devicesFound = 0;
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // Check known addresses
      if (address == 0x57) {
        Serial.print(" <- MAX30102!");
      } else if (address == 0x48) {
        Serial.print(" (Unknown device)");
      }
      
      Serial.println();
      devicesFound++;
    }
  }
  
  if (devicesFound == 0) {
    Serial.println("✗ No I2C devices found!");
    Serial.println("Check SDA (Pin 4) and SCL (Pin 5) connections");
  } else {
    Serial.print("Found ");
    Serial.print(devicesFound);
    Serial.println(" device(s)");
  }
}

void testMAX30102Direct() {
  Serial.println("Attempting direct MAX30102 communication...");
  
  // Try to reset the device first
  Wire.beginTransmission(0x57);
  Wire.write(0x09); // Mode register
  Wire.write(0x40); // Reset bit
  Wire.endTransmission();
  
  delay(100); // Wait for reset
  
  // Try to read device ID
  Wire.beginTransmission(0x57);
  Wire.write(0xFF); // Device ID register
  byte result = Wire.endTransmission();
  
  if (result != 0) {
    Serial.print("✗ Cannot communicate with MAX30102. Error: ");
    Serial.println(result);
    return;
  }
  
  Wire.requestFrom(0x57, 1);
  if (Wire.available()) {
    byte deviceID = Wire.read();
    Serial.print("Device ID: 0x");
    Serial.println(deviceID, HEX);
    
    if (deviceID == 0x15) {
      Serial.println("✓ MAX30102 is responding correctly!");
    } else {
      Serial.println("✗ Wrong device ID - not MAX30102");
    }
  } else {
    Serial.println("✗ No response from device");
  }
}

void testDifferentI2CSpeeds() {
  Serial.println("Testing 100kHz (standard)...");
  Wire.setClock(100000);
  delay(100);
  
  Wire.beginTransmission(0x57);
  byte error1 = Wire.endTransmission();
  Serial.print("100kHz result: ");
  Serial.println(error1 == 0 ? "SUCCESS" : "FAIL");
  
  Serial.println("Testing 400kHz (fast)...");
  Wire.setClock(400000);
  delay(100);
  
  Wire.beginTransmission(0x57);
  byte error2 = Wire.endTransmission();
  Serial.print("400kHz result: ");
  Serial.println(error2 == 0 ? "SUCCESS" : "FAIL");
  
  Serial.println("Testing 50kHz (slow)...");
  Wire.setClock(50000);
  delay(100);
  
  Wire.beginTransmission(0x57);
  byte error3 = Wire.endTransmission();
  Serial.print("50kHz result: ");
  Serial.println(error3 == 0 ? "SUCCESS" : "FAIL");
  
  // Reset to standard
  Wire.setClock(100000);
}

void testPullups() {
  // Test if pullup resistors are working
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  delay(10);
  
  int sda_pullup = digitalRead(4);
  int scl_pullup = digitalRead(5);
  
  Serial.print("SDA with pullup: ");
  Serial.println(sda_pullup == 1 ? "HIGH (good)" : "LOW (bad - short to ground?)");
  Serial.print("SCL with pullup: ");
  Serial.println(scl_pullup == 1 ? "HIGH (good)" : "LOW (bad - short to ground?)");
  
  // Re-initialize I2C after pin mode changes
  Wire.begin();
}
