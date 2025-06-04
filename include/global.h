#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <cstdlib>

#define LED_BUILTIN 13
#define BUZZER_PIN 25
#define MQ_PIN 39
#define DHTPIN 26
#define DHTTYPE DHT11

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

// Biến lưu dữ liệu
extern float tempDHT1, humDHT1;
extern float tempDHT2, humDHT2;
extern float tempBMP1, pressureBMP1;
extern float tempBMP2, pressureBMP2;
extern float Pitch1, Roll1;
extern float Pitch2, Roll2;
extern float air_quality_value1;
extern float air_quality_value2;
extern int16_t ax1, ay1, az1, gx1, gy1, gz1;
extern int ax2, ay2, az2, gx2, gy2, gz2;
extern int sensorValue;
extern float voltage;
extern float RL; // RS_air: điện trở của không khí, RL: điện trở tải, RS: điện trở cảm biến
extern float R0; // RS trong không khí sạch (phải hiệu chỉnh trước)
extern float A;
extern float B;

#endif /* INC_GLOBAL_H_ */