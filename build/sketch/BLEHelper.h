#line 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\BLEHelper.h"
// BLEHelper.h
#ifndef BLEHELPER_H
#define BLEHELPER_H

#include <bluefruit.h>

class BLEHelper {
public:
  void begin();
  /**
   * @brief Update the cycling power measurement data and notify connected devices.
   * 
   * This function updates the power measurement values such as power, revolutions, timestamp, and other optional cycling power characteristics.
   * If an optional characteristic is not provided, it will not be advertised via BLE.
   * 
   */
  void updatePowerData(short power, unsigned short revolutions, unsigned short timestamp, uint8_t batteryLevel = 100, unsigned short crankRevolutionData = 0, unsigned short cumulativeCrankRevolutions = 0, unsigned short lastCrankEventTime = 0, unsigned short maxForceMagnitude = 0, unsigned short minForceMagnitude = 0, unsigned short maxAngle = 0, unsigned short minAngle = 0);
  void updatePowerOnly(short power);
  void updateRevolutionData(short power, unsigned short revolutions, unsigned short timestamp);


private:
  BLEService cyclePowerService = BLEService(0x1818);  // Cycling Power Service UUID
  BLECharacteristic cyclePowerFeature = BLECharacteristic(0x2A65);  // Cycling Power Feature characteristic
  BLECharacteristic cyclePowerMeasurement = BLECharacteristic(0x2A63);  // Cycling Power Measurement characteristic
  BLECharacteristic cyclePowerSensorLocation = BLECharacteristic(0x2A5D);  // Sensor Location characteristic

  BLEService batteryService = BLEService(0x180F);  // Battery Service UUID
  BLECharacteristic batteryLevel = BLECharacteristic(0x2A19, BLERead | BLENotify, 1);  // Battery Level characteristic

  unsigned char bleBuffer[20];
  uint8_t batteryLevelValue = 100;  // Initial battery level (100%)
  unsigned short flags = 0x20;

  void startAdvertising();
};

#endif
