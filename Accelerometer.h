// Rescources / for more info refer to:
// https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi160-ds000.pdf

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Wire.h>

#define BMI160_I2C_ADDRESS 0x69 // Default I2C address for BMI160
// Register addresses acc. to register map in docs
#define BMI160_REG_ACC_X_LSB 0x12
#define BMI160_REG_CMD 0x7E
#define BMI160_REG_CHIP_ID 0x00
#define BMI160_CHIP_ID 0xD1


class Accelerometer {
public:
  /**
   * @brief used to initialize the BLE communication
   * 
   */
  void begin();

  /**
   * @brief function to read an i2c register
   * 
   */
  uint8_t readRegister(uint8_t reg);

  /**
   * @brief function to write to i2c register
   * 
   */
  void writeRegister(uint8_t reg, uint8_t value);

  /**
   * @brief function used to retrieve acceleration via i2c, uses pointers to directly manipulate values of passed variables
   * 
   */
  void readAcceleration(int16_t &x, int16_t &y, int16_t &z);
  
  /**
   * @brief function used to read the first 3 bits of the sensor time, was only used to see if i2c communication is working as expected
   * 
   */
  int readSensorTime();
  

private:

};

#endif
