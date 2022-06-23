#include "common.h"
#include <ArduinoOTA.h>
#include "user_config.h"

#define OTA_INITIAL_STATE true



void setupOta() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
  });
  ArduinoOTA.begin();
}

void loopOta() {
  ArduinoOTA.handle();
}
