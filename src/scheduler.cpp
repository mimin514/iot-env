#include "scheduler.h"

void scheduler() {
    // Tạo các task
    xTaskCreatePinnedToCore(WiFi_Task, "WiFi Task", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(DHTTask, "DHT Task", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(BMP180Task, "BMP180 Task", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(MQ135Task, "MQ135 Task", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(MPUTask, "MPU Task", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(LEDTask, "LED Task", 1024, NULL, 1, NULL, 1);
    // xTaskCreatePinnedToCore(LCDTask, "LCD Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendMQTT, "MQTT Task", 8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendMQTT_mpu, "MQTT MPU Task", 8192, NULL, 1, NULL, 1);
}