#line 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\BLEHelper.cpp"
#include "BLEHelper.h"

void BLEHelper::begin() {
  // Initialize Bluefruit BLE module
  Bluefruit.begin();  
  Bluefruit.setTxPower(8);  // Max transmission power
  Bluefruit.setName("SkiPowermeter"); // Name as its discoverable through "Master Device"

  // Start the Cycling Power Service
  cyclePowerService.begin();

  // --- configure features that are mandatory according to ble docs ---
  
  // Configure the Cycling Power Feature characteristic
  // Contains information about the supported features
  cyclePowerFeature.setProperties(CHR_PROPS_READ); // prpos are read only
  cyclePowerFeature.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); // open access --> no  password
  cyclePowerFeature.setFixedLen(4); // Fixed length of 4 bytes
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

  // --- optional Battery Service ---
  // Start the Battery Service
  // batteryService.begin();

  // // Configure the Battery Level characteristic
  // batteryLevel.setFixedLen(1);
  // batteryLevel.begin();
  // batteryLevel.write(&batteryLevelValue, sizeof(batteryLevelValue));  // Initialize battery level

  // one could also include the "Cycling Power Control Point" and use it to implement calibration and setup procedures...

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

void BLEHelper::updatePowerOnly(short power)
{
  // clear the buffer
  memset(bleBuffer, 0, sizeof(bleBuffer));

  // indicate presence of ONLY power
  flags = 0;

  // write flags to buffer
  bleBuffer[0] = flags & 0xFF;
  bleBuffer[1] = (flags >> 8) & 0xFF;

  // power
  bleBuffer[2] = power & 0xFF;
  bleBuffer[3] = (power >> 8) & 0xFF;

  // send the notification / publish the data
  cyclePowerMeasurement.notify(bleBuffer, 20);
}

void BLEHelper::updateRevolutionData(short power, unsigned short revolutions, unsigned short timestamp)
//updates power and revolutions --> power always has to be present in ble cycling power template!
{
  // clear the buffer
  memset(bleBuffer, 0, sizeof(bleBuffer));
  flags = 0;
  // indicate presence of crank revolution data
  // set flags accordingly...

  flags |= (1 << 5);

  // write flags to buffer
  bleBuffer[0] = flags & 0xFF;
  bleBuffer[1] = (flags >> 8) & 0xFF;

  // write power, revolutions and timestamp to buffer
  bleBuffer[2] = power & 0xFF;              
  bleBuffer[3] = (power >> 8) & 0xFF;
  bleBuffer[4] = revolutions & 0xff;
  bleBuffer[5] = (revolutions >> 8) & 0xff;
  bleBuffer[6] = timestamp & 0xff;
  bleBuffer[7] = (timestamp >> 8) & 0xff;
  
  // send the notification / publish the data
  cyclePowerMeasurement.notify(bleBuffer, 20);
}


void BLEHelper::startAdvertising() {

  // Clear previous advertising data
  Bluefruit.Advertising.clearData();
  Bluefruit.ScanResponse.clearData();

  // Add BLE flags for General Discoverable Mode
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

  // Add the Cycling Power Service and Battery Service to the advertising packet
  Bluefruit.Advertising.addService(cyclePowerService);  // see docs
  Bluefruit.Advertising.addService(batteryService);     // optional

  // Include the device name in the scan response
  Bluefruit.ScanResponse.addName();

  // Set the advertising interval
  Bluefruit.Advertising.setInterval(160, 320);  // 100ms - 200ms

  // Start advertising in general discoverable mode with a timeout for fast mode
  Bluefruit.Advertising.setFastTimeout(30);           // Fast advertising mode timeout = 30 secs
  Bluefruit.Advertising.restartOnDisconnect(true);    // Restart advertising on disconnect
  Bluefruit.Advertising.start(0);                     // Advertising indefinitely
}