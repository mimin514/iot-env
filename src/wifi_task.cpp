#include "wifi_task.h"

void InitWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}


// Task kiểm tra WiFi và kết nối lại nếu mất
void WiFi_Task(void *pvParameters) {
    while (true) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi lost. Reconnecting...");
            InitWiFi();
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}