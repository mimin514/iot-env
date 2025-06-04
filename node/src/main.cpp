#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>
// #include <WiFi.h>
// #include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Adafruit_BMP085.h>
#include "MPU6050.h"
// #include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <cstdlib>
#include <cmath>
#include <math.h>

// #include "esp_wifi.h"
#define DHTPIN 17
#define DHTTYPE DHT11
#define LED_PIN 26
#define BUZZER_PIN 25
#define MQ_PIN 39

const char* ssid = "ACLAB";
const char* password = "ACLAB2023";

// Đối tượng cảm biến và LCD
MPU6050 mpu;
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Biến lưu dữ liệu
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
float RL = 5.0;  // RS_air: điện trở của không khí, RL: điện trở tải, RS: điện trở cảm biến
float R0 = 10.0; // RS trong không khí sạch (phải hiệu chỉnh trước)
// float A = 1000.0;
// float B = 1.5;
float A = 605.18;
float B = -2.074;

// A4:CF:12:81:5E:B4
// Replace with the master's actual MAC address (get from master's Serial output)
// uint8_t masterAddress[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF};
// E8:68:E7:19:26:98
// 80:7D:3A:BA:C0:C8
uint8_t masterAddress[] = {0x80, 0x7D, 0x3A, 0xBA, 0xC0, 0xC8}; // Replace with your master's MAC address
typedef struct {
  uint8_t ledMode;
} ControlData;

volatile uint8_t ledMode = 0;
SemaphoreHandle_t ledMutex;

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
  int ledMode = 0;

} SensorData;

TaskHandle_t TaskSendData;

// Callback to monitor ESP-NOW send status
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void sendTask(void *pvParameters) {
  SensorData data;
  data.id = 1; // Node ID

  while (1) {
    // Giả lập dữ liệu
    data.tempDHT1 = rand() % 30 + 20;
    data.humDHT1 = rand() % 30 + 30;
    data.tempBMP1 = rand() % 20 + 50;
    data.pressureBMP1 = rand() % 20 + 90;
    data.Pitch1 = rand() % 90 - 45;
    data.Roll1 = rand() % 90 - 45;
    data.air_quality_value1 = rand() % 100 + 50;

    if (xSemaphoreTake(ledMutex, pdMS_TO_TICKS(10))) {
      data.ledMode = ledMode;
      xSemaphoreGive(ledMutex);
    }

    esp_err_t result = esp_now_send(masterAddress, (uint8_t *)&data, sizeof(data));
    Serial.printf("Send Result: %s\n", result == ESP_OK ? "OK" : "Fail");

    vTaskDelay(pdMS_TO_TICKS(5000)); // 5 giây/lần
  }
}


volatile uint16_t blinkingInterval = 5000U;
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(ControlData)) {
    ControlData ctrl;
    memcpy(&ctrl, incomingData, sizeof(ctrl));

    // Bảo vệ khi truy cập biến dùng chung
    if (xSemaphoreTake(ledMutex, portMAX_DELAY)) {
      ledMode = ctrl.ledMode;
      digitalWrite(LED_PIN, ledMode ? HIGH : LOW); // Cập nhật LED ngay
      Serial.printf("LED mode received and applied: %d\n", ledMode);
      xSemaphoreGive(ledMutex);
    }
  }
}


void setup()
{
  Serial.begin(115200);

  // Initialize WiFi in Station mode and connect to the same network as master
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nNode WiFi Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("Node MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize DHT sensor
  dht.begin();
  delay(2000); // Wait for DHT to stabilize

  // Initialize ESP-NOW
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
  pinMode(LED_PIN, OUTPUT);
  ledMutex = xSemaphoreCreateMutex();

  // Create FreeRTOS task
  xTaskCreatePinnedToCore(sendTask, "SendTask", 12288, NULL, 1, &TaskSendData, 1);
}

void loop()
{
  // Empty: FreeRTOS handles tasks
}