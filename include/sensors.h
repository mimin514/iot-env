#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_BMP085.h>
#include "MPU6050.h"
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <cmath>
#include <math.h>
#include "global.h"

MPU6050 mpu;
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void DHTTask(void* pvParameters);
void BMP180Task(void* pvParameters);
void MPUTask(void* pvParameters);
float MQ2_getPPM(float voltage);
void MQ135Task(void* pvParameters);
float readRS(); 
float calibrateR0(int numSamples, int delayMS);
void LCDTask(void* pvParameters);
void LEDTask(void* pvParameters);

#endif // SENSORS_H