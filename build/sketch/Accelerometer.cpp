#line 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\Accelerometer.cpp"
#include "accelerometer.h"
#include <Arduino.h>

void Accelerometer::begin() {
  // Initialize BMI160 (set to normal mode)
  writeRegister(BMI160_REG_CMD, 0x11); // Accelerometer normal mode
}

void Accelerometer::readAcceleration(int16_t &x, int16_t &y, int16_t &z) {
  Wire.beginTransmission(BMI160_I2C_ADDRESS);
  Wire.write(BMI160_REG_ACC_X_LSB);
  Wire.endTransmission(false);
  Wire.requestFrom(BMI160_I2C_ADDRESS, 6);
  x = Wire.read() | (Wire.read() << 8);
  y = Wire.read() | (Wire.read() << 8);
  z = Wire.read() | (Wire.read() << 8);
}

void Accelerometer::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(BMI160_I2C_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t Accelerometer::readRegister(uint8_t reg) {
  Wire.beginTransmission(BMI160_I2C_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  delay(15);
  Wire.beginTransmission(BMI160_I2C_ADDRESS);
  Wire.requestFrom(BMI160_I2C_ADDRESS, 1);
  return Wire.read();
}

int Accelerometer::readSensorTime() {
  Wire.beginTransmission(BMI160_I2C_ADDRESS);
  Wire.write(0x18);
  Wire.endTransmission(false);

  delay(10);

  Wire.requestFrom(BMI160_I2C_ADDRESS, 3);
  
  int ret = 0;
  int i = 0;
  while (Wire.available()) {
    ret |= Wire.read() << (i * 8);
    i += 1;
  }

  Serial.println("Sensor time");
  Serial.println(i);
  Serial.println(ret);

  return ret;
}



