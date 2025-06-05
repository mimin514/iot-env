#include "global.hpp"
#include "mqtt.hpp"
#include "rpc.hpp"
void mqttTask(void *pvParameters) {
  static bool subscribed = false;
  while (1) {
    if (!tb_led.connected()) {
      Serial.println("[TB] Attempting to connect...");
      if (tb_led.connect(mqtt_server, TOKEN_DHT_1, THINGSBOARD_PORT)) {
        Serial.println("[TB] Connected to ThingsBoard");
        // if (!subscribed) {
          tb_led.RPC_Subscribe(RPC_Callback("setLedMode", setLedModeCallback));
          tb_led.Shared_Attributes_Subscribe(Shared_Attribute_Callback(onAttributesReceived));
          tb_led.Shared_Attributes_Request(Attribute_Request_Callback(onAttributesReceived));
          subscribed = true;
        // }
      } else {
        Serial.println("[TB] Failed to connect to ThingsBoard");
      }
    }
    tb_led.loop();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
void sendMQTT(void *pvParameters) {
    for (;;) {
        if (!tb_dht1.connected()) tb_dht1.connect(THINGSBOARD_SERVER, TOKEN_DHT_1, THINGSBOARD_PORT);
        if (!tb_mq1.connected()) tb_mq1.connect(THINGSBOARD_SERVER, TOKEN_MQ_1, THINGSBOARD_PORT);
        if (!tb_bmp1.connected()) tb_bmp1.connect(THINGSBOARD_SERVER, TOKEN_BMP_1, THINGSBOARD_PORT);

        if (!tb_dht2.connected()) tb_dht2.connect(THINGSBOARD_SERVER, TOKEN_DHT_2, THINGSBOARD_PORT);
        if (!tb_mq2.connected()) tb_mq2.connect(THINGSBOARD_SERVER, TOKEN_MQ_2, THINGSBOARD_PORT);
        if (!tb_bmp2.connected()) tb_bmp2.connect(THINGSBOARD_SERVER, TOKEN_BMP_2, THINGSBOARD_PORT);
    SensorData data;
    if (xQueueReceive(dataQueue, &data, portMAX_DELAY)) {
        // Device 1
        tb_dht1.sendTelemetryData("temperature", data.tempDHT1);
        tb_dht1.sendTelemetryData("humidity", data.humDHT1);

        tb_mq1.sendTelemetryData("air_quality", data.air_quality_value1);

        tb_bmp1.sendTelemetryData("temperature_bmp", data.tempBMP1);
        tb_bmp1.sendTelemetryData("pressure_bmp", data.pressureBMP1);


        // Device 2
        tb_dht2.sendTelemetryData("temperature", data.tempDHT2);
        tb_dht2.sendTelemetryData("humidity", data.humDHT2);

        tb_mq2.sendTelemetryData("air_quality", data.air_quality_value2);

        tb_bmp2.sendTelemetryData("temperature_bmp", data.tempBMP2);
        tb_bmp2.sendTelemetryData("pressure_bmp", data.pressureBMP2);

    }
        Serial.println("Data sent to ThingsBoard");

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
void sendMQTT_mpu(void *pvParameters) {
    for (;;) {
        if (!tb_mpu1.connected()) tb_mpu1.connect(mqtt_server, TOKEN_MPU_1, THINGSBOARD_PORT);

      if (!tb_mpu2.connected()) tb_mpu2.connect(mqtt_server, TOKEN_MPU_2, THINGSBOARD_PORT);
    SensorData data;
    if (xQueueReceive(dataQueue, &data, portMAX_DELAY)) {
        tb_mpu1.sendTelemetryData("pitch", data.Pitch1);
        tb_mpu1.sendTelemetryData("roll", data.Roll1);

        tb_mpu2.sendTelemetryData("pitch", data.Pitch2);
        tb_mpu2.sendTelemetryData("roll", data.Roll2);
    }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
