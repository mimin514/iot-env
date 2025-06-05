#include "global.hpp"
#include "mqtt.hpp"
#include "rpc.hpp"

// ESP-NOW receive callback
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  xQueueSend(dataQueue, &data, portMAX_DELAY);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nMaster WiFi Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("Master MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    ESP.restart();
  }

  // Register receive callback
  esp_now_register_recv_cb(onDataRecv);

  // Add node as peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, nodeAddress, 6);
  peerInfo.channel = WiFi.channel();
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  }

  // Create queue and MQTT task
  dataQueue = xQueueCreate(10, sizeof(SensorData));
  xTaskCreatePinnedToCore(mqttTask, "MQTT_Task", 4096, NULL, 1, &TaskMQTT, 1);
  xTaskCreatePinnedToCore(sendMQTT, "Send_MQTT", 12288, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(sendMQTT_mpu, "Send_MQTT_MPU", 12288, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskFanScheduler, "Fan_Scheduler", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // Empty: FreeRTOS handles tasks
} 







