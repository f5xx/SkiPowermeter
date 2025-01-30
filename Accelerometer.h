#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Wire.h>

#define BMI160_I2C_ADDRESS 0x69 // Default I2C address for BMI160
// Register addresses
#define BMI160_REG_ACC_X_LSB 0x12
#define BMI160_REG_CMD 0x7E
#define BMI160_REG_CHIP_ID 0x00
#define BMI160_CHIP_ID 0xD1


class Accelerometer {
public:
  void begin();
  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t value);
  void readAcceleration(int16_t &x, int16_t &y, int16_t &z);
  int readSensorTime();

private:

};

#endif
