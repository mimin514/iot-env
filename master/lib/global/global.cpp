#include "global.hpp"
TaskHandle_t TaskMQTT;
QueueHandle_t dataQueue;

WiFiClient espClient;
Arduino_MQTT_Client  mqttClient(espClient);

SensorData data;

// Replace with the node's actual MAC address (get from node's Serial output)
// A4:CF:12:81:5E:B4
uint8_t nodeAddress[] = {0xA4, 0xCF, 0x12, 0x81, 0x5E, 0xB4};

// const char* ssid = "ACLAB";
// const char* password = "ACLAB2023";
const char* ssid = "ACLAB";
const char* password = "ACLAB2023";
const char* mqtt_server = "app.coreiot.io"; // Replace with your MQTT broker IP
const char THINGSBOARD_SERVER[] = "app.coreiot.io";

// Tokens cho từng thiết bị (2 bộ)
const char TOKEN_DHT_1[] = "uaXyXm8OZaMmCwpanY06";
const char TOKEN_MQ_1[] = "o66T8ZfxE49C9et9EBDF";
const char TOKEN_BMP_1[] = "KiIBDDBcTujSGBqSydoW";
const char TOKEN_MPU_1[] = "qipHAWnThEO2eQDzXg31";

const char TOKEN_DHT_2[] = "dSckL0PQstsVH7KZsJD0";
const char TOKEN_MQ_2[] = "9qv3PCo1C0XSQYSKbb3M";
const char TOKEN_BMP_2[] = "BiAdWS2zmO06FyVwRHDJ";
const char TOKEN_MPU_2[] = "3vNeTBqH4cXNyNS0jv5y";
const uint16_t THINGSBOARD_PORT = 1883U;
const uint32_t MAX_MESSAGE_SIZE = 1024U;
const uint32_t SERIAL_DEBUG_BAUD = 115200U;
// Kết nối WiFi client cho từng thiết bị
WiFiClient wifiClient_dht1, wifiClient_mq1, wifiClient_bmp1, wifiClient_mpu1;
WiFiClient wifiClient_dht2, wifiClient_mq2, wifiClient_bmp2, wifiClient_mpu2;

// MQTT clients cho từng thiết bị
Arduino_MQTT_Client mqttClient_dht1(wifiClient_dht1);
Arduino_MQTT_Client mqttClient_mq1(wifiClient_mq1);
Arduino_MQTT_Client mqttClient_bmp1(wifiClient_bmp1);
Arduino_MQTT_Client mqttClient_mpu1(wifiClient_mpu1);

Arduino_MQTT_Client mqttClient_dht2(wifiClient_dht2);
Arduino_MQTT_Client mqttClient_mq2(wifiClient_mq2);
Arduino_MQTT_Client mqttClient_bmp2(wifiClient_bmp2);
Arduino_MQTT_Client mqttClient_mpu2(wifiClient_mpu2);

// ThingsBoard objects cho từng thiết bị
ThingsBoard tb_dht1(mqttClient_dht1, MAX_MESSAGE_SIZE);
ThingsBoard tb_mq1(mqttClient_mq1, MAX_MESSAGE_SIZE);
ThingsBoard tb_bmp1(mqttClient_bmp1, MAX_MESSAGE_SIZE);
ThingsBoard tb_mpu1(mqttClient_mpu1, MAX_MESSAGE_SIZE);

ThingsBoard tb_dht2(mqttClient_dht2, MAX_MESSAGE_SIZE);
ThingsBoard tb_mq2(mqttClient_mq2, MAX_MESSAGE_SIZE);
ThingsBoard tb_bmp2(mqttClient_bmp2, MAX_MESSAGE_SIZE);
ThingsBoard tb_mpu2(mqttClient_mpu2, MAX_MESSAGE_SIZE);

ThingsBoard tb_led(mqttClient, MAX_MESSAGE_SIZE); // LED control object

volatile int ledMode = 0;

FanSchedule fanSchedule;