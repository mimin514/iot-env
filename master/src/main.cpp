 #include <WiFi.h>
#include <esp_now.h>
#include <PubSubClient.h>
// #include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#define LED_BUILTIN 26
#define BUZZER_PIN 4

TaskHandle_t TaskMQTT;
QueueHandle_t dataQueue;

WiFiClient espClient;
Arduino_MQTT_Client  mqttClient(espClient);

typedef struct
{
  float tempDHT1, humDHT1;
  float tempDHT2, humDHT2;
  float tempBMP1, pressureBMP1;
  float tempBMP2, pressureBMP2;
  float Pitch1, Roll1;
  float Pitch2, Roll2;
  float air_quality_value1 = 0;
  float air_quality_value2 = 0;
  int16_t ax1, ay1, az1, gx1, gy1, gz1;
  int ax2, ay2, az2, gx2, gy2, gz2;
  int sensorValue;
  float voltage;
  uint8_t id;

} SensorData;
// Replace with the node's actual MAC address (get from node's Serial output)
// A4:CF:12:81:5E:B4
uint8_t nodeAddress[] = {0xA4, 0xCF, 0x12, 0x81, 0x5E, 0xB4};

const char* ssid = "ACLAB";
const char* password = "ACLAB2023";
const char* mqtt_server = "app.coreiot.io"; // Replace with your MQTT broker IP
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";

// Tokens cho từng thiết bị (2 bộ)
constexpr char TOKEN_DHT_1[] = "uaXyXm8OZaMmCwpanY06";
constexpr char TOKEN_MQ_1[] = "o66T8ZfxE49C9et9EBDF";
constexpr char TOKEN_BMP_1[] = "KiIBDDBcTujSGBqSydoW";
constexpr char TOKEN_MPU_1[] = "qipHAWnThEO2eQDzXg31";

constexpr char TOKEN_DHT_2[] = "dSckL0PQstsVH7KZsJD0";
constexpr char TOKEN_MQ_2[] = "9qv3PCo1C0XSQYSKbb3M";
constexpr char TOKEN_BMP_2[] = "BiAdWS2zmO06FyVwRHDJ";
constexpr char TOKEN_MPU_2[] = "3vNeTBqH4cXNyNS0jv5y";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;
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

void onAttributesReceived(const JsonObjectConst &data) {
    if (data.containsKey("ledMode")) {
      
        ledMode = data["ledMode"];
        Serial.printf("Updated LED mode: %d\n", ledMode);
        
    }
}

void requestSharedAttributes() {
    tb_led.Shared_Attributes_Request(Attribute_Request_Callback(onAttributesReceived));
}

RPC_Response setLedModeCallback(const RPC_Data &data) {
  Serial.println("Received RPC call: setLedMode");
  Serial.print("Raw data received: ");
  Serial.println(data.as<String>());

  JsonObjectConst params = data["params"];

  if (params.containsKey("ledMode")) {
    ledMode = params["ledMode"];
    Serial.printf("LED mode updated: %d\n", ledMode);
    digitalWrite(LED_BUILTIN, ledMode ? HIGH : LOW);

    // Gửi về node qua ESP-NOW
    typedef struct {
      uint8_t ledMode;
    } ControlData;

    ControlData ctrl;
    ctrl.ledMode = ledMode;

    esp_err_t result = esp_now_send(nodeAddress, (uint8_t *)&ctrl, sizeof(ctrl));
    if (result == ESP_OK) {
      Serial.println("Sent LED mode to node via ESP-NOW");
    } else {
      Serial.println("Failed to send LED mode to node");
    }

    return RPC_Response("LED mode updated", true);
  }

  Serial.println("Error: No ledMode parameter found in RPC call.");
  return RPC_Response("Error: No ledMode parameter", false);
}


// ESP-NOW receive callback
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  SensorData data;
  memcpy(&data, incomingData, sizeof(data));
  xQueueSend(dataQueue, &data, portMAX_DELAY);
}

void mqttTask(void *pvParameters) {
  while (1) {
    if (!tb_led.connected()) {
      Serial.println("[TB] Attempting to connect...");
      if (tb_led.connect(mqtt_server, TOKEN_DHT_1, THINGSBOARD_PORT)) {
        Serial.println("[TB] Connected to ThingsBoard");

        tb_led.RPC_Subscribe(RPC_Callback("setLedMode", setLedModeCallback));
        tb_led.Shared_Attributes_Subscribe(Shared_Attribute_Callback(onAttributesReceived));
        tb_led.Shared_Attributes_Request(Attribute_Request_Callback(onAttributesReceived));
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
}

void loop() {
  // Empty: FreeRTOS handles tasks
} 







