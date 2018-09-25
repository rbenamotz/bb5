#include <Arduino.h>
#include "pumps.h"
#include "recepies.h"
#include "user_config.h"
#include "common.h"
#include "panel.h"
#include "ota.h"
#include "wifi.h"
#include "webserver.h"



void setup() {
    setupCommon();
    setupRecepies();
    setupWifi();
    setupOta();
    setupPumps();
    //setupMqtt();
    setupPanel();
    setupWebServer();
}

void loop() {
    loopWifi();
    loopOta();
    loopPumps();
    // recepie r = getRecepieById(RECEPIE_ID_MOSCOW_MULE);
    // prepareDrink(r);
    loopPanel();
    loopWebServer();
    delay(5);
}