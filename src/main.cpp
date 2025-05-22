

#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
// #include "DHT.h"
#include "Wire.h"
#include <ArduinoOTA.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Adafruit_BMP085.h>
#include "MPU6050.h"
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <cstdlib> 
constexpr char WIFI_SSID[] = "ACLAB";
constexpr char WIFI_PASSWORD[] = "ACLAB2023";

constexpr char TOKEN[] = "o66T8ZfxE49C9et9EBDF";
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

volatile int ledMode = 0;
volatile uint16_t blinkingInterval = 5000U;
WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);
#define LED_BUILTIN 16
#define BUZZER_PIN 17
#define MQ_PIN 2
#define DHTPIN 14
#define DHTTYPE DHT11

// ================== THƯ VIỆN VÀ BIẾN ==================
MPU6050 mpu;
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float tempDHT, humDHT;
float tempBMP, pressureBMP;
int air_quality_value = 0;
int ax, ay, az, gx, gy, gz;


void InitWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}

void WiFi_Task(void *pvParameters) {
    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi lost. Reconnecting...");
            InitWiFi();
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


void DHTTask(void* pvParameters) {
for (;;) {
// tempDHT = dht.readTemperature();
// humDHT = dht.readHumidity();
// tb.sendTelemetryData("humidity", humDHT);
// tb.sendTelemetryData("temperature", tempDHT);

 tempDHT = rand() % 30 + 20;
 humDHT = rand() % 30 + 30;

vTaskDelay(5000 / portTICK_PERIOD_MS);
}
}

void BMP180Task(void* pvParameters) {
for (;;) {
// tempBMP = bmp.readTemperature();
// pressureBMP = bmp.readPressure() / 100.0;
tempBMP = rand() % 20 + 50;
pressureBMP = rand() % 20 + 90;

vTaskDelay(5000 / portTICK_PERIOD_MS);
}
}

void MQ135Task(void* pvParameters) {
for (;;) {
// air_quality_value = analogRead(MQ_PIN);
air_quality_value = rand() % 100 + 50;

vTaskDelay(5000 / portTICK_PERIOD_MS);
}
}

void MPUTask(void* pvParameters) {
for (;;) {
// mpu.getAcceleration(&ax, &ay, &az);
// mpu.getRotation(&gx, &gy, &gz);
ax = rand() % 100 + 50;
ay = rand() % 100 + 50;
az = rand() % 100 + 50;
gx = rand() % 100 + 50;
gy = rand() % 100 + 50;
gz = rand() % 100 + 50;


vTaskDelay(5000 / portTICK_PERIOD_MS);
}
}

void LEDTask(void* pvParameters) {
for (;;) {
digitalWrite(LED_BUILTIN, HIGH);
digitalWrite(BUZZER_PIN, HIGH);
vTaskDelay(1000 / portTICK_PERIOD_MS);
digitalWrite(LED_BUILTIN, LOW);
digitalWrite(BUZZER_PIN, LOW);
vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}

void LCDTask(void* pvParameters) {
for (;;) {
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("T:");
lcd.print((int)tempDHT);
lcd.print((char)223); // ký hiệu độ
lcd.print("C H:");
lcd.print((int)humDHT);
lcd.print("%");
lcd.setCursor(0, 1);
lcd.print("P:");
lcd.print((int)pressureBMP);
lcd.print(" hPa");
vTaskDelay(5000 / portTICK_PERIOD_MS);
}
}


void sendMQTT(void *pvParameters) {
    while (1) {


        tb.sendTelemetryData("humidity", humDHT);
        tb.sendTelemetryData("temperature", tempDHT);
        tb.sendTelemetryData("pressure", pressureBMP);
        tb.sendTelemetryData("air_quality", air_quality_value);
        tb.sendTelemetryData("ax", ax);
        tb.sendTelemetryData("ay", ay);
        tb.sendTelemetryData("az", az);
        tb.sendTelemetryData("gx", gx);
        tb.sendTelemetryData("gy", gy);
        tb.sendTelemetryData("gz", gz);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


void MQTT_Task(void *pvParameters) {
    while (1) {
        if (!tb.connected()) {
            Serial.println("Connecting to ThingsBoard...");
            if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
                Serial.println("Failed to connect to ThingsBoard");
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
            Serial.println("Connected to ThingsBoard!");

        }

        tb.loop();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void setup() {
    Serial.begin(SERIAL_DEBUG_BAUD);
    InitWiFi();
    xTaskCreate(WiFi_Task, "WiFi Task", 4096, NULL, 1, NULL);
    xTaskCreate(MQTT_Task, "MQTT Task", 8192, NULL, 1, NULL);
    xTaskCreate(DHTTask, "DHT Task", 4096, NULL, 1, NULL);
    xTaskCreate(BMP180Task, "BMP180 Task", 4096, NULL, 1, NULL);
    xTaskCreate(MQ135Task, "MQ135 Task", 4096, NULL, 1, NULL);
    xTaskCreate(MPUTask, "MPU6050 Task", 4096, NULL, 1, NULL);
    xTaskCreate(LEDTask, "LED Task", 4096, NULL, 1, NULL);
    xTaskCreate(sendMQTT, "sendMQTT Task", 4096, NULL, 1, NULL);
    // xTaskCreate(LCDTask, "LCD Task", 2000, NULL, 1, NULL);
}

void loop() {
    vTaskDelete(NULL);
}