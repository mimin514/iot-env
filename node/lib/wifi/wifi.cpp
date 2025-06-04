
#include "global.hpp"
#include "wifi.hpp"

void wifiInit(void){ 
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nNode WiFi Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("Node MAC Address: ");
  Serial.println(WiFi.macAddress());
}