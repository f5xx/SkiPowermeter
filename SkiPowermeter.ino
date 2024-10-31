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
