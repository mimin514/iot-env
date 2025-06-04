#include "MQTT.h"

// Task gửi dữ liệu lên ThingsBoard qua MQTT, 10s gửi 1 lần
void sendMQTT(void *pvParameters) {
    for (;;) {
        if (!tb_dht1.connected()) tb_dht1.connect(THINGSBOARD_SERVER, TOKEN_DHT_1, THINGSBOARD_PORT);
        if (!tb_mq1.connected()) tb_mq1.connect(THINGSBOARD_SERVER, TOKEN_MQ_1, THINGSBOARD_PORT);
        if (!tb_bmp1.connected()) tb_bmp1.connect(THINGSBOARD_SERVER, TOKEN_BMP_1, THINGSBOARD_PORT);

        if (!tb_dht2.connected()) tb_dht2.connect(THINGSBOARD_SERVER, TOKEN_DHT_2, THINGSBOARD_PORT);
        if (!tb_mq2.connected()) tb_mq2.connect(THINGSBOARD_SERVER, TOKEN_MQ_2, THINGSBOARD_PORT);
        if (!tb_bmp2.connected()) tb_bmp2.connect(THINGSBOARD_SERVER, TOKEN_BMP_2, THINGSBOARD_PORT);

        // Device 1
        tb_dht1.sendTelemetryData("temperature", tempDHT1);
        tb_dht1.sendTelemetryData("humidity", humDHT1);

        tb_mq1.sendTelemetryData("air_quality", air_quality_value1);

        tb_bmp1.sendTelemetryData("temperature_bmp", tempBMP1);
        tb_bmp1.sendTelemetryData("pressure_bmp", pressureBMP1);


        // Device 2
        tb_dht2.sendTelemetryData("temperature", tempDHT2);
        tb_dht2.sendTelemetryData("humidity", humDHT2);

        tb_mq2.sendTelemetryData("air_quality", air_quality_value2);

        tb_bmp2.sendTelemetryData("temperature_bmp", tempBMP2);
        tb_bmp2.sendTelemetryData("pressure_bmp", pressureBMP2);


        Serial.println("Data sent to ThingsBoard");

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void sendMQTT_mpu(void *pvParameters) {
    for (;;) {
        if (!tb_mpu1.connected()) tb_mpu1.connect(THINGSBOARD_SERVER, TOKEN_MPU_1, THINGSBOARD_PORT);

      if (!tb_mpu2.connected()) tb_mpu2.connect(THINGSBOARD_SERVER, TOKEN_MPU_2, THINGSBOARD_PORT);

        tb_mpu1.sendTelemetryData("pitch", Pitch1);
        tb_mpu1.sendTelemetryData("roll", Roll1);

        tb_mpu2.sendTelemetryData("pitch", Pitch2);
        tb_mpu2.sendTelemetryData("roll", Roll2);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}