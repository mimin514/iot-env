
#include "global.hpp"
#include "LCD.hpp"
#include "sensor.hpp"
#include "now.hpp"
#include "wifi.hpp"
void setup()
{
  Serial.begin(115200);
  dht.begin();
  bmp.begin();
  mpu.initialize();
  wifiInit();
  espnowInit();
  lcd_init();
  xTaskCreatePinnedToCore(sendTask, "SendTask", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(LEDTask, "LEDTask", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(LCDTask, "LCDTask", 8192, NULL, 1, NULL, 1);
}

void loop()
{
}