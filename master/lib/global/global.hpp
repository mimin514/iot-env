 #include <WiFi.h>
#include <esp_now.h>
#include <PubSubClient.h>
// #include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#define LED_BUILTIN 26
#define BUZZER_PIN 4

extern TaskHandle_t TaskMQTT;
extern QueueHandle_t dataQueue;

extern WiFiClient espClient;
extern Arduino_MQTT_Client  mqttClient;

extern uint8_t nodeAddress[] ;

extern const char* ssid ;
extern const char* password;
extern const char* mqtt_server ;
extern const char THINGSBOARD_SERVER[] ;

extern const char TOKEN_DHT_1[];
extern const char TOKEN_MQ_1[];
extern const char TOKEN_BMP_1[];
extern const char TOKEN_MPU_1[];

extern const char TOKEN_DHT_2[];
extern const char TOKEN_MQ_2[];
extern const char TOKEN_BMP_2[];
extern const char TOKEN_MPU_2[];
extern const uint16_t THINGSBOARD_PORT ;
extern const uint32_t MAX_MESSAGE_SIZE;
extern const uint32_t SERIAL_DEBUG_BAUD ;
// Kết nối WiFi client cho từng thiết bị
extern WiFiClient wifiClient_dht1, wifiClient_mq1, wifiClient_bmp1, wifiClient_mpu1;
extern WiFiClient wifiClient_dht2, wifiClient_mq2, wifiClient_bmp2, wifiClient_mpu2;

// MQTT clients cho từng thiết bị
extern Arduino_MQTT_Client mqttClient_dht1;
extern Arduino_MQTT_Client mqttClient_mq1;
extern Arduino_MQTT_Client mqttClient_bmp1;
extern Arduino_MQTT_Client mqttClient_mpu1;

extern Arduino_MQTT_Client mqttClient_dht2;
extern Arduino_MQTT_Client mqttClient_mq2;
extern Arduino_MQTT_Client mqttClient_bmp2;
extern Arduino_MQTT_Client mqttClient_mpu2;

// ThingsBoard objects cho từng thiết bị
extern ThingsBoard tb_dht1;
extern ThingsBoard tb_mq1;
extern ThingsBoard tb_bmp1;
extern ThingsBoard tb_mpu1;

extern ThingsBoard tb_dht2;
extern ThingsBoard tb_mq2;
extern ThingsBoard tb_bmp2;
extern ThingsBoard tb_mpu2;

extern ThingsBoard tb_led;

extern volatile int ledMode;
typedef struct
{
  float tempDHT1, humDHT1;
  float tempDHT2, humDHT2;
  float tempBMP1, pressureBMP1;
  float tempBMP2, pressureBMP2;
  float Pitch1, Roll1;
  float Pitch2, Roll2;
  float air_quality_value1 = 0;
  float air_quality_value2 = 0;
  int16_t ax1, ay1, az1, gx1, gy1, gz1;
  int ax2, ay2, az2, gx2, gy2, gz2;
  int sensorValue;
  float voltage;
  uint8_t id;

} SensorData;
 extern SensorData data;

 typedef struct {
  uint8_t ledMode;
    uint8_t fanControl;
  uint8_t fanState;    
} ControlData;

typedef struct {
  int startHour, startMin;
  int endHour, endMin;
  uint8_t mode; // 1: bật, 0: tắt
  bool valid;
} FanSchedule;

extern FanSchedule fanSchedule;

