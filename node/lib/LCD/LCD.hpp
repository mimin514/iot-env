#ifndef LCD_HPP
#define LCD_HPP

void lcd_init();
void LCDTask(void* pvParameters) ;
void LCDshowDHT(float a, float b) ;
void LCDshowMPU(float a, float b) ;
void LCDshowbmp_mq2(float a, float b) ;
#endif 