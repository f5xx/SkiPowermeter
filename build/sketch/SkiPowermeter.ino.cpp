#include <Arduino.h>
#line 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino"
#include <bluefruit.h>

// BLE service and characteristic for Cycling Power
BLEService cyclePowerService = BLEService(0x1818);  // Cycling Power Service UUID
BLECharacteristic cyclePowerFeature = BLECharacteristic(0x2A65);  // Cycling Power Feature characteristic
BLECharacteristic cyclePowerMeasurement = BLECharacteristic(0x2A63);  // Cycling Power Measurement characteristic
BLECharacteristic cyclePowerSensorLocation = BLECharacteristic(0x2A5D);  // Sensor Location characteristic

// BLE service and characteristic for Battery Level
BLEService batteryService = BLEService(0x180F);  // Battery Service UUID
BLECharacteristic batteryLevel = BLECharacteristic(0x2A19, BLERead | BLENotify, 1);  // Battery Level characteristic

unsigned char bleBuffer[8];
unsigned char slBuffer[1];
unsigned char fBuffer[4];
uint8_t batteryLevelValue = 100;  // Initial battery level (100%)

short power;
unsigned short revolutions = 0;
unsigned short timestamp = 0;
unsigned short flags = 0x20;
byte sensorlocation = 0x0D;

#line 24 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino"
void setup();
#line 67 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino"
void loop();
#line 95 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino"
void startAdvertising();
#line 24 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino"
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting BLE Power Meter...");

  // Initialize Bluefruit BLE module
  Bluefruit.begin();
  Bluefruit.setTxPower(8);  // Max transmission power
  Bluefruit.setName("BikePowerMeter");

  // Start the Cycling Power Service
  cyclePowerService.begin();

  // Configure the Cycling Power Feature characteristic
  cyclePowerFeature.setProperties(CHR_PROPS_READ);
  cyclePowerFeature.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  cyclePowerFeature.setFixedLen(4);
  cyclePowerFeature.begin();

  // Configure the Cycling Power Measurement characteristic
  cyclePowerMeasurement.setProperties(CHR_PROPS_NOTIFY);
  cyclePowerMeasurement.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  cyclePowerMeasurement.setFixedLen(8);
  cyclePowerMeasurement.begin();

  // Configure the Sensor Location characteristic
  cyclePowerSensorLocation.setProperties(CHR_PROPS_READ);
  cyclePowerSensorLocation.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  cyclePowerSensorLocation.setFixedLen(1);
  cyclePowerSensorLocation.begin();

  // Start the Battery Service
  batteryService.begin();

  // Configure the Battery Level characteristic
  batteryLevel.setFixedLen(1);
  batteryLevel.begin();
  batteryLevel.write(&batteryLevelValue, sizeof(batteryLevelValue));  // Initialize battery level

  // Start advertising
  startAdvertising();
}

void loop() {
  // Simulate dynamic power data
  power = 200 + (millis() / 1000) % 100;
  revolutions += 1;  // Increment revolutions
  timestamp += 1024 / 2;  // Increment timestamp

  // Write variables into the buffers for Cycling Power Measurement
  bleBuffer[0] = flags & 0xff;
  bleBuffer[1] = (flags >> 8) & 0xff;
  bleBuffer[2] = power & 0xff;
  bleBuffer[3] = (power >> 8) & 0xff;
  bleBuffer[4] = revolutions & 0xff;
  bleBuffer[5] = (revolutions >> 8) & 0xff;
  bleBuffer[6] = timestamp & 0xff;
  bleBuffer[7] = (timestamp >> 8) & 0xff;

  // Notify the connected device with the updated power data
  cyclePowerMeasurement.notify(bleBuffer, 8);

  // Simulate battery level decrease over time
  if (batteryLevelValue > 0) {
    batteryLevelValue--;  // Decrease battery level
  }
  batteryLevel.write(&batteryLevelValue, sizeof(batteryLevelValue));  // Update battery level

  delay(1000);  // Send data every second
}

void startAdvertising() {
  // Clear previous advertising data
  Bluefruit.Advertising.clearData();
  Bluefruit.ScanResponse.clearData();

  // Add BLE flags for General Discoverable Mode
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

  // Add the Cycling Power Service and Battery Service to the advertising packet
  Bluefruit.Advertising.addService(cyclePowerService);
  Bluefruit.Advertising.addService(batteryService);

  // Include the device name in the scan response
  Bluefruit.ScanResponse.addName();

  // Set the advertising interval (100ms - 200ms)
  Bluefruit.Advertising.setInterval(160, 320);  // 100ms - 200ms

  // Start advertising in general discoverable mode with a timeout for fast mode
  Bluefruit.Advertising.setFastTimeout(30);  // Fast advertising mode timeout
  Bluefruit.Advertising.restartOnDisconnect(true);  // Restart advertising on disconnect
  Bluefruit.Advertising.start(0);  // Advertising indefinitely
}

#line 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\Backup.ino"
//#include <bluefruit.h>

// // BLE service and characteristic for Cycling Speed and Cadence (CSC)
// BLEService cscService = BLEService(0x1816);  // CSC service UUID
// BLECharacteristic cscMeasurementChar = BLECharacteristic(0x2A5B);  // CSC Measurement characteristic UUID

// uint16_t crankRevolutions = 0;  // Total crank revolutions
// uint16_t cadence = 90;          // Initial cadence value (in RPM)
// bool isCadence90 = true;        // Toggle between 90 and 80 RPM

// void setup() {
//   Serial.begin(115200);  // Start serial communication for debugging
//   delay(2000);           // Wait for Serial to initialize
//   Serial.println("Starting BLE Cadence Sensor...");

//   // Initialize Bluefruit BLE module
//   Bluefruit.begin();
//   Bluefruit.setTxPower(8);    // Max transmission power
//   Bluefruit.setName("BikeCadenceSensor");

//   // Start the Cycling Speed and Cadence (CSC) service
//   cscService.begin();
//   Serial.println("CSC Service started...");

//   // Configure the CSC Measurement characteristic
//   cscMeasurementChar.setProperties(CHR_PROPS_NOTIFY);  // Notify property
//   cscMeasurementChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);  // No authentication needed
//   cscMeasurementChar.setFixedLen(5);  // 5 bytes for CSC measurement data (flags + 16-bit crank data)
//   cscMeasurementChar.begin();
//   Serial.println("CSC Measurement characteristic started...");

//   // Start advertising the service
//   startAdvertising();
//   Serial.println("Advertising started...");
// }

// void loop() {
//   // Toggle cadence between 90 and 80 RPM every 2 seconds
//   if (isCadence90) {
//     cadence = 90;
//   } else {
//     cadence = 80;
//   }

//   // Update the crank revolutions based on cadence (this is a simplified model)
//   crankRevolutions += cadence / 30;  // Update crank revolution count based on cadence

//   // Prepare CSC measurement packet (5 bytes total)
//   uint8_t cscMeasurementData[5];

//   // Byte 0: Flags (0x02 means only crank data present)
//   cscMeasurementData[0] = 0x02;

//   // Byte 1-2: Cumulative crank revolutions (16-bit)
//   cscMeasurementData[1] = crankRevolutions & 0xFF;        // Lower byte of crank revolutions
//   cscMeasurementData[2] = (crankRevolutions >> 8) & 0xFF; // Upper byte of crank revolutions

//   // Byte 3-4: Last crank event time in 1/1024 seconds (simplified)
//   uint16_t eventTime = (millis() / 1000.0) * 1024;
//   cscMeasurementData[3] = eventTime & 0xFF;         // Lower byte of event time
//   cscMeasurementData[4] = (eventTime >> 8) & 0xFF;  // Upper byte of event time

//   // Notify the connected device with the CSC data
//   cscMeasurementChar.notify(cscMeasurementData, sizeof(cscMeasurementData));

//   // Toggle cadence value
//   isCadence90 = !isCadence90;

//   // Wait for 2 seconds before sending the next value
//   delay(2000);
// }

// void startAdvertising() {
//   // Set up the advertising packet with flags and service information
//   Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
//   Bluefruit.Advertising.addTxPower();  // Include TX power level in the advertising packet
//   Bluefruit.Advertising.addService(cscService);  // Include the CSC service

//   // Include the device name in the scan response
//   Bluefruit.ScanResponse.addName();

//   // Start advertising
//   Bluefruit.Advertising.restartOnDisconnect(true);  // Restart advertising on disconnect
//   Bluefruit.Advertising.setInterval(32, 64);        // Advertising interval (32ms to 64ms)
//   Bluefruit.Advertising.setFastTimeout(30);         // Fast advertising mode timeout
//   Bluefruit.Advertising.start(0);                   // Start advertising indefinitely

//   Serial.println("Started advertising CSC service.");
// }
