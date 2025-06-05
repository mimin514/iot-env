#include "now.hpp"
#include "global.hpp"
// ESP-NOW receive callback
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&data, incomingData, sizeof(data));
  xQueueSend(dataQueue, &data, portMAX_DELAY);
}

void espnowInit(void)
{
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW init failed");
    ESP.restart();
  }
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
}
