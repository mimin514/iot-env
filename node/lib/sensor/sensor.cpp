
#include "global.hpp"
#include "sensor.hpp"
#include "now.hpp"
#include "LCD.hpp"
void DHTTask(void)
{

  data.tempDHT2 = rand() % 30 + 20;
  data.humDHT2 = rand() % 30 + 30;

  data.tempDHT1 = dht.readTemperature();
  data.humDHT1 = dht.readHumidity();
  if (isnan(data.tempDHT1) || isnan(data.humDHT1))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.printf("DHT1: Temp: %.2f, Hum: %.2f\n", data.tempDHT1, data.humDHT1);
  }
  Serial.printf("DHT2: Temp=%.2f C, Humidity=%.2f%%\n", data.tempDHT2, data.humDHT2);
}
// Task mô phỏng đọc dữ liệu BMP180
void BMP180Task(void)
{

  data.tempBMP1 = bmp.readTemperature();
  data.pressureBMP1 = bmp.readPressure() / 100.0; // hPa
  data.tempBMP2 = rand() % 20 + 50;
  data.pressureBMP2 = rand() % 20 + 90;
  Serial.printf("BMP1: Temp=%.2f C, Pressure=%.2f hPa\n", data.tempBMP1, data.pressureBMP1);
  Serial.printf("BMP2: Temp=%.2f C, Pressure=%.2f hPa\n", data.tempBMP2, data.pressureBMP2);
}
float readRS()
{
  int sensorValue = analogRead(MQ_PIN);
  float voltage = sensorValue * (3.3 / 4095.0);
  float RS = ((3.3 / voltage) - 1.0) * RL;
  return RS;
}

// Hàm đo R0 (điện trở trong không khí sạch)
// đo nhiều mẫu để lấy trung bình cho chính xác
float calibrateR0(int numSamples = 50, int delayMS = 200)
{
  float rsSum = 0;
  for (int i = 0; i < numSamples; i++)
  {
    rsSum += readRS();
    delay(delayMS);
  }
  float r0 = rsSum / numSamples;
  Serial.printf("Calibration done, R0 = %.3f kΩ\n", r0);
  return r0;
}

// Hàm tính PPM dựa vào điện áp và R0
float MQ2_getPPM(float voltage)
{
  if (voltage < 0.01 || voltage >= 3.3)
  {
    Serial.println("Voltage out of range.");
    return -1;
  }

  float RS = ((3.3 / voltage) - 1.0) * RL;
  float ratio = RS / R0;

  float ppm = A * pow(ratio, B);
  return ppm;
}

void MQ135Task(void)
{
  sensorValue = analogRead(MQ_PIN);
  voltage = sensorValue * (3.3 / 4095.0);
  float RS = ((3.3 / voltage) - 1.0) * RL;
  float ratio = RS / R0;

  data.air_quality_value1 = MQ2_getPPM(voltage);

  Serial.printf("MQ1 | Raw: %d, Voltage: %.3f V, RS: %.2f, R0: %.2f, Ratio: %.2f, PPM: %.2f\n",
                sensorValue, voltage, RS, R0, ratio, data.air_quality_value1);

  data.air_quality_value2 = rand() % 100 + 50; // Mô phỏng device 2
}
void MPUTask(void)
{

  data.mpu.getAcceleration(&ax1, &ay1, &az1);
  data.mpu.getRotation(&gx1, &gy1, &gz1);
  data.Pitch1 = atan2(-ax1, sqrt(ay1 * ay1 + az1 * az1)) * 180 / PI;
  data.Roll1 = atan2(ay1, az1) * 180 / PI;

  ax2 = rand() % 100 + 50;
  ay2 = rand() % 100 + 50;
  az2 = rand() % 100 + 50;
  gx2 = rand() % 100 + 50;
  gy2 = rand() % 100 + 50;
  gz2 = rand() % 100 + 50;
  data.Pitch2 = atan2(-ax2, sqrt(ay2 * ay2 + az2 * az2)) * 180 / PI;
  data.Roll2 = atan2(ay2, az2) * 180 / PI;
  Serial.printf("MPU1: Pitch=%.2f, Roll=%.2f\n", data.Pitch1, data.Roll1);
  Serial.printf("MPU2: Pitch=%.2f, Roll=%.2f\n", data.Pitch2, data.Roll2);
}
void LEDTask(void *pvParameters)
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  for (;;)
  {
    if (xSemaphoreTake(ledMutex, pdMS_TO_TICKS(10))) {
      digitalWrite(FAN_PIN, ledMode ? HIGH : LOW);
      xSemaphoreGive(ledMutex);
    }
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(9000));
  }
}
void sendTask(void *pvParameters)
{
  data.id = 1; // Node ID

  while (1)
  {
Serial.println("Led Mode: " + String(ledMode));
    DHTTask();
    BMP180Task();
    MQ135Task();
    MPUTask();
limit_task();
    if (xSemaphoreTake(ledMutex, pdMS_TO_TICKS(10)))
    {
      data.ledMode = ledMode;
      xSemaphoreGive(ledMutex);
    }

    esp_err_t result = esp_now_send(masterAddress, (uint8_t *)&data, sizeof(data));
    Serial.printf("Send Result: %s\n", result == ESP_OK ? "OK" : "Fail");
    vTaskDelay(pdMS_TO_TICKS(5000)); // 5 giây/lần
  }
}

void lcd_task(void)
{
  LCDshowDHT(data.tempDHT1, data.humDHT1);
  vTaskDelay(pdMS_TO_TICKS(5000));
  LCDshowMPU(data.Pitch1, data.Roll1);
  vTaskDelay(pdMS_TO_TICKS(5000));
  LCDshowbmp_mq2(data.tempBMP1, data.air_quality_value1);
  vTaskDelay(pdMS_TO_TICKS(5000));
}
void onWarning(void)
{
  digitalWrite(BUZZER_PIN, HIGH);
}
void offWarning(void)
{
  digitalWrite(BUZZER_PIN, LOW);
}
void limit_task(void)
{
  if (data.tempDHT1 > MAX_DHT || data.humDHT1 < MIN_DHT ||
      data.tempDHT2 > MAX_DHT || data.humDHT2 < MIN_DHT)
  {
    onWarning();
    Serial.println("Warning: Temperature or Humidity out of range!");
  }
  else if (data.tempBMP1 > MAX_BMP || data.pressureBMP1 < MIN_BMP ||
           data.tempBMP2 > MAX_BMP || data.pressureBMP2 < MIN_BMP)
  {
    onWarning();
    Serial.println("Warning: Temperature or Humidity out of range!");
  }
  else if (data.air_quality_value1 > MAX_MQ || data.air_quality_value1 < MIN_MQ ||
           data.air_quality_value2 > MAX_MQ || data.air_quality_value2 < MIN_MQ)
  {
    onWarning();
    Serial.println("Warning: Air quality out of range!");
  }
  else if (data.Pitch1 > MAX_MPU_PITCH || data.Pitch1 < MIN_MPU_PITCH ||
           data.Roll1 > MAX_MPU_ROLL || data.Roll1 < MIN_MPU_ROLL ||
           data.Pitch2 > MAX_MPU_PITCH || data.Pitch2 < MIN_MPU_PITCH ||
           data.Roll2 > MAX_MPU_ROLL || data.Roll2 < MIN_MPU_ROLL )
  {
    onWarning();
    Serial.println("Warning: MPU values out of range!");
  }

  else
  {
    offWarning();
  }
}