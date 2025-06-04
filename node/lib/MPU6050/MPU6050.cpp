#include "MPU6050.h"

// Địa chỉ của MPU6050
#define MPU6050_ADDR 0x68

// Các thanh ghi cần thiết cho MPU6050
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43

MPU6050::MPU6050() {
  address = MPU6050_ADDR;
}

bool MPU6050::initialize() {
  Wire.begin();
  
  // Bật cảm biến MPU6050 bằng cách ghi vào thanh ghi PWR_MGMT_1
  writeByte(PWR_MGMT_1, 0x00); // Đặt PWR_MGMT_1 thành 0 để khởi động cảm biến

  // Kiểm tra kết nối cảm biến
  return testConnection();
}

bool MPU6050::testConnection() {
  uint8_t data;
  readBytes(0x75, 1, &data);  // Đọc ID của cảm biến
  return (data == 0x68);
}

void MPU6050::getAcceleration(int16_t *ax, int16_t *ay, int16_t *az) {
  uint8_t buffer[6];
  
  // Đọc 6 byte dữ liệu gia tốc từ các thanh ghi
  readBytes(ACCEL_XOUT_H, 6, buffer);
  
  // Kết nối các byte để tạo thành giá trị gia tốc X, Y, Z
  *ax = (int16_t)(buffer[0] << 8 | buffer[1]);
  *ay = (int16_t)(buffer[2] << 8 | buffer[3]);
  *az = (int16_t)(buffer[4] << 8 | buffer[5]);
}

void MPU6050::getRotation(int16_t *gx, int16_t *gy, int16_t *gz) {
  uint8_t buffer[6];
  
  // Đọc 6 byte dữ liệu con quay hồi chuyển từ các thanh ghi
  readBytes(GYRO_XOUT_H, 6, buffer);
  
  // Kết nối các byte để tạo thành giá trị con quay hồi chuyển X, Y, Z
  *gx = (int16_t)(buffer[0] << 8 | buffer[1]);
  *gy = (int16_t)(buffer[2] << 8 | buffer[3]);
  *gz = (int16_t)(buffer[4] << 8 | buffer[5]);
}

void MPU6050::writeByte(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void MPU6050::readBytes(uint8_t reg, uint8_t length, uint8_t *data) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(address, length);
  for (uint8_t i = 0; i < length; i++) {
    data[i] = Wire.read();
  }
}

