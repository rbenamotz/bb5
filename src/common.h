#ifndef COMMON_H
#define COMMON_H

extern bool globalIsWifiConnected;


void setupPanel();
void loopPanel();

void setupOta();
void loopOta();

void loopWifi();
void setupWifi();

void setupLedStrip();
void loopLedStrip();



#endif