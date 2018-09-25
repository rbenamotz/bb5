#include <Arduino.h>
#include "pumps.h"
#include "recepies.h"
#include "user_config.h"
#include "common.h"
#include "panel.h"
#include "ota.h"
#include "wifi.h"
#include "webserver.h"
#include "neo.h"
#include "mqtt.h"


void setup() {
    setupCommon();
    setupRecepies();
    setupWifi();
    setupOta();
    setupPumps();
    setupNeo();
    setupMqtt();
    setupPanel();
    setupWebServer();
}

void loop() {
    loopWifi();
    loopOta();
    loopPumps();
    loopPanel();
    loopMqtt();
    loopWebServer();
    loopNeo();
}