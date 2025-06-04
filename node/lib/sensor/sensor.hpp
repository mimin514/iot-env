#ifndef sensor_HPP
#define sensor_HPP


void DHTTask(void);
void sendTask(void *pvParameters);
void LEDTask(void* pvParameters) ;
void lcd_task(void);

#endif