#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "user_config.h"
#include "common.h"
#include <WiFiManager.h>

void setupWifi()
{
  Serial.println("Entering WiFi setup");
  Serial.println("Starting WiFi Manager");
  WiFiManager wifiManager;
  WiFi.hostname(HOST_NAME);
  // wifiManager.resetSettings();
  wifiManager.setDebugOutput(true);
  if (!wifiManager.autoConnect(HOST_NAME))
  {
    ESP.reset();
  }
  MDNS.begin(HOST_NAME);
  globalIsWifiConnected = true;
}
void loopWifi()
{
}