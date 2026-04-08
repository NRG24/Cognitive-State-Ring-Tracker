/*
  Basic XIAO Pin and I2C Test
  Tests fundamental XIAO functionality without assuming pin assignments
*/

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("=== BASIC XIAO DIAGNOSTIC ===");
  Serial.println("Testing fundamental functionality...");
  
  // Test 1: Basic pin functionality
  Serial.println("\n1. Testing basic pin functionality:");
  testAllPins();
  
  // Test 2: GSR (known working)
  Serial.println("\n2. Testing GSR (should work):");
  int gsr = analogRead(A0);
  Serial.print("GSR reading: ");
  Serial.println(gsr);
  if (gsr > 0) {
    Serial.println("✓ GSR working - XIAO is functional");
  } else {
    Serial.println("✗ GSR not working - possible XIAO problem");
  }
  
  // Test 3: Check if Wire library compiles and initializes
  Serial.println("\n3. Testing Wire library:");
  testWireLibrary();
  
  // Test 4: Manual pin testing
  Serial.println("\n4. Manual pin state testing:");
  manualPinTest();
  
  Serial.println("\nDiagnostic complete. Check results above.");
}

void loop() {
  // Simple blink to show XIAO is alive
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    lastBlink = millis();
    
    // Try to blink built-in LED
    static bool ledState = false;
    ledState = !ledState;
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledState);
    
    Serial.print(".");  // Show we're alive
  }
}

void testAllPins() {
  Serial.println("Testing all digital pins:");
  for (int pin = 0; pin <= 10; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(1);
    pinMode(pin, INPUT);
    int state = digitalRead(pin);
    
    Serial.print("Pin ");
    Serial.print(pin);
    Serial.print(": ");
    Serial.println(state);
  }
}

void testWireLibrary() {
  Serial.println("Attempting to include and initialize Wire...");
  
  // This will fail to compile if Wire library has issues
  #include <Wire.h>
  
  Serial.println("Wire library included successfully");
  
  try {
    Wire.begin();
    Serial.println("✓ Wire.begin() executed without error");
  } catch (...) {
    Serial.println("✗ Wire.begin() failed");
  }
  
  // Try a simple I2C operation
  Serial.println("Testing basic I2C operation...");
  Wire.beginTransmission(0x00);  // Invalid address, should return error
  byte error = Wire.endTransmission();
  Serial.print("I2C test error code: ");
  Serial.println(error);
  if (error != 0) {
    Serial.println("✓ I2C controller responding (error expected for address 0x00)");
  } else {
    Serial.println("✗ Unexpected I2C response");
  }
}

void manualPinTest() {
  Serial.println("Manual pin control test:");
  
  // Test the pins we think are SDA/SCL
  int testPins[] = {4, 5, 6, 7};  // Test a few pins around the expected ones
  
  for (int i = 0; i < 4; i++) {
    int pin = testPins[i];
    
    Serial.print("Testing pin ");
    Serial.print(pin);
    Serial.print(": ");
    
    // Set as output and toggle
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(1);
    int lowState = digitalRead(pin);
    
    digitalWrite(pin, HIGH);
    delay(1);
    int highState = digitalRead(pin);
    
    // Set as input with pullup
    pinMode(pin, INPUT_PULLUP);
    delay(1);
    int pullupState = digitalRead(pin);
    
    Serial.print("LOW:");
    Serial.print(lowState);
    Serial.print(" HIGH:");
    Serial.print(highState);
    Serial.print(" PULLUP:");
    Serial.print(pullupState);
    
    if (lowState == 0 && highState == 1 && pullupState == 1) {
      Serial.println(" ✓ Pin working normally");
    } else {
      Serial.println(" ✗ Pin not responding correctly");
    }
  }
}
