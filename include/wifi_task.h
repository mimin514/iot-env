#ifndef WIFI_1_H
#define WIFI_1_H

#include "global.h"
#include <WiFi.h>
#include <Arduino.h>

void InitWiFi() ;
void WiFi_Task(void *pvParameters) ;

#endif // WIFI_1_H