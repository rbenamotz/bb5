#include <Arduino.h>
#include "user_config.h"
#include "common.h"
#include <stdio.h>
#include "pump.cpp"

bool globalIsWifiConnected = false;
uint8_t pumpPins[] = PUMPS_PINS;
Pump *pumps = (Pump *)malloc(sizeof(Pump) * TOTAL_PUMPS);

void setup()
{
    Serial.begin(9600);
    for (uint8_t i = 0; i < TOTAL_PUMPS; i++)
    {
        pumps[i] = Pump(pumpPins[i]);
    }
    Serial.println("Hello from BB5");
    setupLedStrip();
    setupWifi();
    setupOta();
}

void loopPumps()
{
    return; 
    static int i = -1;
    static unsigned long last = 0;
    unsigned long l = millis() - last;
    if (l < 500)
    {
        return;
    }
    if (i >= 0)
    {
        pumps[i].toggle();
    }
    i++;
    if (i == TOTAL_PUMPS)
    {
        i = 0;
    }
    pumps[i].toggle();

    last = millis();
}

void loop()
{
    loopLedStrip();
    loopWifi();
    loopOta();
    loopPumps();
}
