#ifndef MPU6050_H
#define MPU6050_H

#include <Wire.h>

class MPU6050 {
public:
  MPU6050();
  bool initialize();
  bool testConnection();
  void getAcceleration(int16_t *ax, int16_t *ay, int16_t *az);
  void getRotation(int16_t *gx, int16_t *gy, int16_t *gz);

private:
  uint8_t address;
  void writeByte(uint8_t reg, uint8_t value);
  void readBytes(uint8_t reg, uint8_t length, uint8_t *data);
};

#endif
