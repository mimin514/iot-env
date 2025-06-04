#include "lcd.hpp"
#include "global.hpp"
#include "sensor.hpp"
void lcd_init()
{
    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("IOT SMARTFARM");
}
void LCDshowDHT(float a, float b)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT11-v1:");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print((int)a);
    lcd.print((char)223); // ký hiệu độ
    lcd.print("C   H:");
    lcd.print((int)b);
    lcd.print("%");
}
void LCDshowMPU(float a, float b)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MPU6050-V1:");
    lcd.setCursor(0, 1);
    lcd.print("PITCH:");
    lcd.print((int)a);
    lcd.print("ROLL:");
    lcd.print((int)b);
}
void LCDshowbmp_mq2(float a, float b)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BMP180-V1: ");
    lcd.print((int)a);
    lcd.setCursor(0, 1);
    lcd.print("MQ2 -V1:");
    lcd.print((int)b);
}
void LCDTask(void *pvParameters)
{
    for (;;)
    {
        lcd_task();
        
    }
}