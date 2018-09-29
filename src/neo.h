#ifndef _NEO_H_
#define _NEO_H_
#include <NeoPixelBus.h>

typedef struct { 
    bool state;
    RgbColor idleColor;
    int brightness;
} lightState;


void setupNeo();
void loopNeo();
void refreshNeo();

void startCupAnimation();
void stopCupAnimation();

extern lightState contextLightState;
String idleColorAsJson();

#endif