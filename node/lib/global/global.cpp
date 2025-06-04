#include "global.hpp"

 const char* ssid = "ACLAB";
 const char* password = "ACLAB2023";

// Đối tượng cảm biến và LCD
 MPU6050 mpu;
 Adafruit_BMP085 bmp;
 DHT dht(DHTPIN, DHTTYPE);
 LiquidCrystal_I2C lcd(0x27, 16, 2);
// Biến lưu dữ liệu
SemaphoreHandle_t ledMutex;
 float air_quality_value1 = 0;
 float air_quality_value2 = 0;

 float RL = 5.0;  // RS_air: điện trở của không khí, RL: điện trở tải, RS: điện trở cảm biến
 float R0 = 10.0; // RS trong không khí sạch (phải hiệu chỉnh trước)
// float A = 1000.0;
// float B = 1.5;
 float A = 605.18;
 float B = -2.074;

 uint8_t masterAddress[] = {0x80, 0x7D, 0x3A, 0xBA, 0xC0, 0xC8}; // Replace with your master's MAC address

 volatile uint8_t ledMode = 0;

int16_t ax1 = 0, ay1 = 0, az1 = 0, gx1 = 0, gy1 = 0, gz1 = 0;
int ax2 = 0, ay2 = 0, az2 = 0, gx2 = 0, gy2 = 0, gz2 = 0;
int sensorValue = 0;
float voltage = 0.0;


SensorData data;