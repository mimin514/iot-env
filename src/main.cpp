#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Adafruit_BMP085.h>
#include "MPU6050.h"
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include "Attribute_Request_Callback.h"
#include "Shared_Attribute_Callback.h"
#include "RPC_Callback.h"
#include "RPC_Response.h"

// #define THINGSBOARD_ENABLE_DEBUG 1

// constexpr char WIFI_SSID[] = "ACLAB";
// constexpr char WIFI_PASSWORD[] = "ACLAB2023";
constexpr char WIFI_SSID[] = "NaNa";
constexpr char WIFI_PASSWORD[] = "12356789";
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

// Tokens cho từng thiết bị (2 bộ)
constexpr char TOKEN_DHT_1[] = "uaXyXm8OZaMmCwpanY06";
constexpr char TOKEN_MQ_1[] = "o66T8ZfxE49C9et9EBDF";
constexpr char TOKEN_BMP_1[] = "KiIBDDBcTujSGBqSydoW";
constexpr char TOKEN_MPU_1[] = "qipHAWnThEO2eQDzXg31";

constexpr char TOKEN_DHT_2[] = "dSckL0PQstsVH7KZsJD0";
constexpr char TOKEN_MQ_2[] = "9qv3PCo1C0XSQYSKbb3M";
constexpr char TOKEN_BMP_2[] = "BiAdWS2zmO06FyVwRHDJ";
constexpr char TOKEN_MPU_2[] = "3vNeTBqH4cXNyNS0jv5y";

#define LED_BUILTIN 13
#define BUZZER_PIN 25
#define MQ_PIN 39
#define DHTPIN 26
#define DHTTYPE DHT11

// Đối tượng cảm biến và LCD
MPU6050 mpu;
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb_led(mqttClient, MAX_MESSAGE_SIZE);

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
  float RL = 5.0; // RS_air: điện trở của không khí, RL: điện trở tải, RS: điện trở cảm biến
  float R0 = 10.0; // RS trong không khí sạch (phải hiệu chỉnh trước)
// float A = 1000.0;
// float B = 1.5;
float A = 605.18;
float B = -2.074;

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

volatile int ledMode = 0;
volatile uint16_t blinkingInterval = 5000U;

void onAttributesReceived(const JsonObjectConst &data) {
    if (data.containsKey("ledMode")) {
        ledMode = data["ledMode"];
        Serial.printf("Updated LED mode: %d\n", ledMode);
    }
}

void requestSharedAttributes() {
    tb_led.Shared_Attributes_Request(Attribute_Request_Callback(onAttributesReceived));
}

void InitWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}
// Declare the function prototype before its usage
RPC_Response setLedModeCallback(const RPC_Data &data) {
  Serial.println("Received RPC call: setLedMode");
  Serial.print("Raw data received: ");
  Serial.println(data.as<String>());
  if (data.containsKey("params") && data["params"].containsKey("ledMode")) {
     ledMode = data["params"]["ledMode"];
    Serial.printf("LED mode updated: %d\n", ledMode);

    digitalWrite(LED_BUILTIN, ledMode ? HIGH : LOW);
    return RPC_Response("LED mode updated", true);
  }

  Serial.println(" Error: No ledMode parameter found in RPC call.");
  return RPC_Response("Error: No ledMode parameter", false);
}
void MQTT_Task(void *pvParameters) {
    while (1) {
        if (!tb_led.connected()) {
            Serial.println("Connecting to ThingsBoard...");
            if (!tb_led.connect(THINGSBOARD_SERVER, TOKEN_DHT_1, THINGSBOARD_PORT)) {
                Serial.println("Failed to connect to ThingsBoard");
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
            Serial.println("Connected to ThingsBoard!");
            tb_led.RPC_Subscribe(RPC_Callback("setLedMode", setLedModeCallback));

            // tb.RPC_Subscribe(RPC_Callback("processData", processDataCallback));
            tb_led.Shared_Attributes_Subscribe(Shared_Attribute_Callback(onAttributesReceived));
            requestSharedAttributes();
        }
        tb_led.loop();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
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

// Task mô phỏng đọc dữ liệu DHT
void DHTTask(void* pvParameters) {
    for (;;) {
        // tempDHT1 = rand() % 30 + 20;
        // humDHT1 = rand() % 30 + 30;

        tempDHT2 = rand() % 30 + 20;
        humDHT2 = rand() % 30 + 30;

        tempDHT1 = dht.readTemperature();
        humDHT1 = dht.readHumidity();
        Serial.printf("DHT1: Temp=%.2f C, Humidity=%.2f%%\n", tempDHT1, humDHT1);
        Serial.printf("DHT2: Temp=%.2f C, Humidity=%.2f%%\n", tempDHT2, humDHT2);
        //  tempDHT2 = dht.readTemperature();
        // humDHT2 = dht.readHumidity();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Task mô phỏng đọc dữ liệu BMP180
void BMP180Task(void* pvParameters) {
    for (;;) {
        // tempBMP1 = rand() % 20 + 50;
        // pressureBMP1 = rand() % 20 + 90;

         tempBMP1 = bmp.readTemperature();
        pressureBMP1 = bmp.readPressure() / 100.0; // hPa
        tempBMP2 = rand() % 20 + 50;
        pressureBMP2 = rand() % 20 + 90;
        Serial.printf("BMP1: Temp=%.2f C, Pressure=%.2f hPa\n", tempBMP1, pressureBMP1);
        Serial.printf("BMP2: Temp=%.2f C, Pressure=%.2f hPa\n", tempBMP2, pressureBMP2);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
float readRS() {
    int sensorValue = analogRead(MQ_PIN);
    float voltage = sensorValue * (3.3 / 4095.0);
    float RS = ((3.3 / voltage) - 1.0) * RL;
    return RS;
}


// Hàm đo R0 (điện trở trong không khí sạch)
// đo nhiều mẫu để lấy trung bình cho chính xác
float calibrateR0(int numSamples = 50, int delayMS = 200) {
    float rsSum = 0;
    for (int i = 0; i < numSamples; i++) {
        rsSum += readRS();
        delay(delayMS);
    }
    float r0 = rsSum / numSamples;
    Serial.printf("Calibration done, R0 = %.3f kΩ\n", r0);
    return r0;
}

// Hàm tính PPM dựa vào điện áp và R0
float MQ2_getPPM(float voltage) {
    if (voltage < 0.01 || voltage >= 3.3) {
        Serial.println("Voltage out of range.");
        return -1;
    }

    float RS = ((3.3 / voltage) - 1.0) * RL;
    float ratio = RS / R0;

    float ppm = A * pow(ratio, B);
    return ppm;
}

// Task mô phỏng đọc dữ liệu MQ135
void MQ135Task(void* pvParameters) {
    for (;;) {
        sensorValue = analogRead(MQ_PIN);
        voltage = sensorValue * (3.3 / 4095.0);
        float RS = ((3.3 / voltage) - 1.0) * RL;
        float ratio = RS / R0;

        air_quality_value1 = MQ2_getPPM(voltage);

        Serial.printf("MQ1 | Raw: %d, Voltage: %.3f V, RS: %.2f, R0: %.2f, Ratio: %.2f, PPM: %.2f\n",
                      sensorValue, voltage, RS, R0, ratio, air_quality_value1);

        air_quality_value2 = rand() % 100 + 50;  // Mô phỏng device 2

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


// Task mô phỏng đọc dữ liệu MPU6050 và tính toán Pitch, Roll
void MPUTask(void* pvParameters) {
    for (;;) {
        // ax1 = rand() % 100 + 50;
        // ay1 = rand() % 100 + 50;
        // az1 = rand() % 100 + 50;
        // gx1 = rand() % 100 + 50;
        // gy1 = rand() % 100 + 50;
        // gz1 = rand() % 100 + 50;
        // Pitch1 = atan2(-ax1, sqrt(ay1*ay1 + az1*az1)) * 180 / PI;
        // Roll1 = atan2(ay1, az1) * 180 / PI;
        mpu.getAcceleration(&ax1, &ay1, &az1);
        mpu.getRotation(&gx1, &gy1, &gz1);
        Pitch1 = atan2(-ax1, sqrt(ay1*ay1 + az1*az1)) * 180 / PI;
        Roll1 = atan2(ay1, az1) * 180 / PI;

        ax2 = rand() % 100 + 50;
        ay2 = rand() % 100 + 50;
        az2 = rand() % 100 + 50;
        gx2 = rand() % 100 + 50;
        gy2 = rand() % 100 + 50;
        gz2 = rand() % 100 + 50;
        Pitch2 = atan2(-ax2, sqrt(ay2*ay2 + az2*az2)) * 180 / PI;
        Roll2 = atan2(ay2, az2) * 180 / PI;
Serial.printf("MPU1: Pitch=%.2f, Roll=%.2f\n", Pitch1, Roll1);
        Serial.printf("MPU2: Pitch=%.2f, Roll=%.2f\n", Pitch2, Roll2);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Task LED + Buzzer nhấp nháy 1s on/off
void LEDTask(void* pvParameters) {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    for (;;) {
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(9000));
    }
}

// Task hiển thị LCD cập nhật mỗi 5 giây
void LCDTask(void* pvParameters) {
    lcd.init();
    lcd.backlight();

    for (;;) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T:");
        lcd.print((int)tempDHT1);
        lcd.print((char)223); // ký hiệu độ
        lcd.print("C H:");
        lcd.print((int)humDHT1);
        lcd.print("%");

        lcd.setCursor(0, 1);
        lcd.print("P:");
        lcd.print((int)pressureBMP1);
        lcd.print(" hPa");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

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
void setup() {
    Serial.begin(SERIAL_DEBUG_BAUD);
    InitWiFi();

    // Khởi tạo cảm biến
    dht.begin();
    if (!bmp.begin()) Serial.println("BMP180 error");
if (!bmp.begin()) Serial.println("BMP180 not found!");
if (!mpu.testConnection()) Serial.println("MPU6050 not found!");
    // Khởi tạo MPU (bạn có thể khởi tạo thật nếu có phần cứng)
    mpu.initialize();
    R0 = calibrateR0();  // Hiệu chuẩn R0 tự động khi khởi động

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

void loop() {
    // Loop để giữ CPU không bị treo
    // vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelete(NULL);

}
