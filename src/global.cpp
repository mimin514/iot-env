#include "global.h"

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
