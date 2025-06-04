#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>
// #include <WiFi.h>
// #include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Adafruit_BMP085.h>
// #include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include <MPU6050.h>

#define DHTPIN 17
#define DHTTYPE DHT11
#define LED_PIN 26
#define FAN_PIN 14
#define BUZZER_PIN 25
#define MQ_PIN 39

extern const char* ssid;
extern const char* password;
// Đối tượng cảm biến và LCD

extern MPU6050 mpu;
extern Adafruit_BMP085 bmp;
extern DHT dht;
extern LiquidCrystal_I2C lcd;

// Biến lưu dữ liệu
extern float tempDHT1, humDHT1;
extern float tempDHT2, humDHT2;
extern float tempBMP1, pressureBMP1;
extern float tempBMP2, pressureBMP2;
extern float Pitch1, Roll1;
extern float Pitch2, Roll2;
extern float air_quality_value1 ;
extern float air_quality_value2 ;
extern int16_t ax1, ay1, az1, gx1, gy1, gz1;
extern int ax2, ay2, az2, gx2, gy2, gz2;
extern int sensorValue;
extern float voltage;
extern float RL;  // RS_air: điện trở của không khí, RL: điện trở tải, RS: điện trở cảm biến
extern float R0 ; // RS trong không khí sạch (phải hiệu chỉnh trước)
// float A = 1000.0;
// float B = 1.5;
extern float A ;
extern float B ;

extern uint8_t masterAddress[] ;

extern volatile uint8_t ledMode ;
extern SemaphoreHandle_t ledMutex;

typedef struct {
  uint8_t ledMode;
} ControlData;

typedef struct {
  float tempDHT1, humDHT1;
  float tempDHT2, humDHT2;
  float tempBMP1, pressureBMP1;
  float tempBMP2, pressureBMP2;
  float Pitch1, Roll1;
  float Pitch2, Roll2;
  float air_quality_value1;
  float air_quality_value2;
  int16_t ax1, ay1, az1, gx1, gy1, gz1;
  int ax2, ay2, az2, gx2, gy2, gz2;
  int sensorValue;
  float voltage;
  uint8_t id;
  int ledMode;
  MPU6050 mpu; // nếu cần
} SensorData;
extern TaskHandle_t TaskSendData;
extern volatile bool warningActive;

extern SensorData data;

extern const int MAX_DHT;
extern const int MAX_BMP;
extern const int MAX_MQ;
extern const int MAX_MPU;
extern const int MAX_MPU_PITCH;
extern const int MAX_MPU_ROLL;

extern const int MIN_DHT;
extern const int MIN_BMP;
extern const int MIN_MQ;
extern const int MIN_MPU;
extern const int MIN_MPU_PITCH;
extern const int MIN_MPU_ROLL;