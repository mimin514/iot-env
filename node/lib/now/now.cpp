#include "now.hpp"
#include "global.hpp"
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    Serial.print("Raw data received: ");
  for (int i = 0; i < len; i++) {
    Serial.printf("%02X ", incomingData[i]);
  }
  Serial.println();
  if (len == sizeof(ControlData)) {
    ControlData ctrl;
    memcpy(&ctrl, incomingData, sizeof(ctrl));

    // Bảo vệ khi truy cập biến dùng chung
    if (xSemaphoreTake(ledMutex, portMAX_DELAY)) {
      if (ledMode != ctrl.ledMode) { // Chỉ cập nhật khi có thay đổi
        ledMode = ctrl.ledMode;
        digitalWrite(FAN_PIN, ledMode ? HIGH : LOW); // Cập nhật LED ngay
        Serial.printf("LED mode received and applied: %d\n", ledMode);
      }
      xSemaphoreGive(ledMutex);
    }
  }
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void espnowInit(void){
     if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW init failed");
    ESP.restart();
  }
esp_now_register_recv_cb(onDataRecv);

  // Register send callback
  esp_now_register_send_cb(onDataSent);

  // Add master as peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 1; // Use same channel as WiFi
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    ESP.restart();
  }
    ledMutex = xSemaphoreCreateMutex();
}
