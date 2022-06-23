#include <Arduino.h>
#include "common.h"
#include "user_config.h"
#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip1(PIXEL_COUNT, PIN_NEO_PIXEL);

void setupLedStrip()
{
    strip1.Begin();
    strip1.ClearTo(RgbColor(255, 0, 0));
    strip1.Show();
}

void loopLedStrip()
{
    static unsigned long last = 0;
    static uint8_t currentPixel = 0;
    if (!globalIsWifiConnected) {
        return;
    }
    unsigned long l = millis() - last;
    if (l<50) {
        return;
    }
    strip1.ClearTo(RgbColor(0,0,0));
    strip1.SetPixelColor(currentPixel,RgbColor(0,0,255));
    strip1.Show();
    currentPixel++;
    if (currentPixel==PIXEL_COUNT) {
        currentPixel = 0;
    }
    last = millis();
}