#include "global.hpp"
#include "mqtt.hpp"
#include "rpc.hpp"
#include "ota.hpp"
#include "now.hpp"
#include "wifi.hpp"

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  wifiInit();
  
espnowInit();

  // Create queue and MQTT task
  dataQueue = xQueueCreate(10, sizeof(SensorData));
  xTaskCreatePinnedToCore(mqttTask, "MQTT_Task", 4096, NULL, 1, &TaskMQTT, 1);
  xTaskCreatePinnedToCore(sendMQTT, "Send_MQTT", 12288, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(sendMQTT_mpu, "Send_MQTT_MPU", 12288, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(taskFanScheduler, "Fan_Scheduler", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // Empty: FreeRTOS handles tasks
} 






