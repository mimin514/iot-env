#include <Arduino.h>
#define LED_BUILTIN 16
#define BUZZER_PIN 17  // Bạn có thể đổi sang chân khác nếu cần


#define MQ_PIN 2  // MQ2 hoặc MQ135 kết nối chân A0

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include "MPU6050.h"
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Khai báo đối tượng MPU6050
MPU6050 mpu;
// Tạo đối tượng BMP180
Adafruit_BMP085 bmp;
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


LiquidCrystal_I2C lcd(0x27, 16, 2);  // Địa chỉ I2C có thể là 0x27 hoặc 0x3F

void setup() {
    Wire.begin(21, 22);
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  // ESP32 default I2C: SDA = GPIO21, SCL = GPIO22
  pinMode(LED_BUILTIN, OUTPUT);
pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
    byte error, address;
  int count = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Thiết bị I2C tìm thấy tại địa chỉ 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      count++;
    }
  }

  if (count == 0) Serial.println("Không tìm thấy thiết bị I2C nào.");
  else Serial.println("Quét xong.");

  if (!bmp.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BMP180 Fail!");
    Serial.println("Không tìm thấy BMP180.");
    while (1);
  }

  mpu.initialize();
  if (!mpu.testConnection()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MPU6050 Fail!");
    Serial.println("Không thể kết nối với MPU6050!");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("Khoi dong OK");
  delay(2000);
  lcd.clear();
}


void loop() {

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Không đọc được từ DHT11!");
  } else {
    Serial.print("Nhiet do: ");
    Serial.print(temp);
    Serial.println(" *C");

    Serial.print("Do am: ");
    Serial.print(hum);
    Serial.println(" %");
  }

  float temperature = bmp.readTemperature();       // °C
  float pressure = bmp.readPressure() / 100.0;     // hPa
  float altitude = bmp.readAltitude();             // meters
Serial.println("BMP180");
  Serial.print("Nhiet do: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Ap suat: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Do cao (tuong doi): ");
  Serial.print(altitude);
  Serial.println(" m");

  delay(2000);
    int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);
Serial.println("MPU6050");
  // In ra các giá trị gia tốc và con quay hồi chuyển
  Serial.print("Gia tốc (X, Y, Z): ");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.println("\t");

  Serial.print("Con quay hồi chuyển (X, Y, Z): ");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);
   int air_quality_value = analogRead(MQ_PIN);  // Đọc giá trị analog từ MQ135 (0–4095)
  
  Serial.print("Chỉ số chất lượng không khí (AQ): ");
  Serial.println(air_quality_value);
    Serial.println("-----------------------------");
  // Hiển thị lên LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print((char)223); // Ký tự độ
  lcd.print("C H:");
  lcd.print(hum);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(pressure);
  lcd.print(" hPa");
    digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);  // Bật buzzer

  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);  // Tắt buzzer
  delay(1000);
}
