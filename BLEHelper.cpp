// BLEHelper.cpp
#include "BLEHelper.h"

void BLEHelper::begin() {
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
  cyclePowerMeasurement.setFixedLen(20);
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

void BLEHelper::updatePowerData(short power, unsigned short revolutions, unsigned short timestamp, uint8_t batteryLevel, unsigned short crankRevolutionData, unsigned short cumulativeCrankRevolutions, unsigned short lastCrankEventTime, unsigned short maxForceMagnitude, unsigned short minForceMagnitude, unsigned short maxAngle, unsigned short minAngle) {
  // Write variables into the buffers for Cycling Power Measurement
  flags = 0;
  bleBuffer[0] = flags & 0xff;
  bleBuffer[1] = (flags >> 8) & 0xff;
  bleBuffer[2] = power & 0xff;
  bleBuffer[3] = (power >> 8) & 0xff;
  bleBuffer[4] = revolutions & 0xff;
  bleBuffer[5] = (revolutions >> 8) & 0xff;
  bleBuffer[6] = timestamp & 0xff;
  bleBuffer[7] = (timestamp >> 8) & 0xff;

  // Set battery level
  bleBuffer[19] = batteryLevel;

  // Set optional fields based on method inputs

  // Crank Revolution Data (used for cadence calculation)
  if (crankRevolutionData) {
    flags |= (1 << 5);  // Set the appropriate bit for crank revolution data present

    // Cumulative Crank Revolutions
    bleBuffer[8] = cumulativeCrankRevolutions & 0xff;
    bleBuffer[9] = (cumulativeCrankRevolutions >> 8) & 0xff;

    // Last Crank Event Time
    bleBuffer[10] = lastCrankEventTime & 0xff;
    bleBuffer[11] = (lastCrankEventTime >> 8) & 0xff;
  }

  // Maximum and Minimum Force Magnitudes
  if (maxForceMagnitude && minForceMagnitude) {
    flags |= (1 << 6);

    bleBuffer[12] = maxForceMagnitude & 0xff;
    bleBuffer[13] = (maxForceMagnitude >> 8) & 0xff;
    bleBuffer[14] = minForceMagnitude & 0xff;
    bleBuffer[15] = (minForceMagnitude >> 8) & 0xff;
  }

  // Maximum and Minimum Angles
  if (maxAngle && minAngle) {
    flags |= (1 << 8);

    bleBuffer[16] = maxAngle & 0xff;
    bleBuffer[17] = (maxAngle >> 8) & 0xff;
    bleBuffer[18] = minAngle & 0xff;
    bleBuffer[19] = (minAngle >> 8) & 0xff;
  }

  // Update flags in the buffer
  bleBuffer[0] = flags & 0xff;
  bleBuffer[1] = (flags >> 8) & 0xff;

  // Notify the connected device with the updated power data
  cyclePowerMeasurement.notify(bleBuffer, 20);
}

void BLEHelper::startAdvertising() {
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