#ifndef MQTT_H
#define MQTT_H

#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <ArduinoOTA.h>
#include "global.h"
#include "sensors.h"
#include <WiFi.h>

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

void sendMQTT(void *pvParameters);
void sendMQTT_mpu(void *pvParameters);

#endif // MQTT_H