#include <Arduino.h>
#include "user_config.h"
#include "common.h"
#include "pumps.h"
#include "recepies.h"

unsigned long lastPanelCheck = 0;
unsigned long lastPanelStatusLogged = 0;
int buttonAssignments[] = BUTTON_RECEPIES;
int currentButtonDown = -1;


void setupPanel() {
    pinMode(PANEL_PIN_ENABLE,OUTPUT);
    pinMode(PANEL_PIN_LOAD,OUTPUT);
    pinMode(PANEL_PIN_CLOCK,OUTPUT);
    pinMode(PANEL_PIN_DATA,INPUT_PULLUP);
    digitalWrite(PANEL_PIN_LOAD,HIGH);
    digitalWrite(PANEL_PIN_ENABLE,HIGH);
}



void handleButtonDown(int buttonId) {
    currentButtonDown = buttonId;
    if (stopAllPumps()) {
        return;
    }
    int receipeId = buttonAssignments[buttonId];
    if (receipeId==-1) {
        write_to_log("Button %d pushed but no receipe assigned", buttonId);
        return;
    }
    write_to_log("Button %d pushed. Need to prepare recepie %d", buttonId, receipeId);
    recepie r = getRecepieById(receipeId);
    prepareDrink(r);
}
bool isButtonStillDown(byte incoming) {
    if (currentButtonDown ==- 1) {
        return false;
    }
    return (bitRead(incoming,currentButtonDown) ==0);
}

void loopPanel() {
    if (millis() < 1000) {
        return;
    }
    unsigned long l = millis() - lastPanelCheck;
    if (l<50) {
        return;
    }
    digitalWrite(PANEL_PIN_LOAD,LOW);
    delayMicroseconds(5);
    digitalWrite(PANEL_PIN_LOAD,HIGH);
    delayMicroseconds(5);

    digitalWrite(PANEL_PIN_CLOCK,HIGH);
    digitalWrite(PANEL_PIN_ENABLE,LOW);
    byte incoming=shiftIn(PANEL_PIN_DATA,PANEL_PIN_CLOCK,MSBFIRST);
    digitalWrite(PANEL_PIN_ENABLE,HIGH);
    lastPanelCheck = millis();
    if (isButtonStillDown(incoming)) {
        return;
    }
    for(int i=5;i>=0;i--)
    {
        if (bitRead(incoming,i) ==0) {
            handleButtonDown(i);
            return;
        }
    }
    currentButtonDown = -1;
}