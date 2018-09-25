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
    // return;
    pinMode(PANEL_PIN_ENABLE,OUTPUT);
    pinMode(PANEL_PIN_LOAD,OUTPUT);
    pinMode(PANEL_PIN_CLOCK,OUTPUT);
    pinMode(PANEL_PIN_DATA,INPUT_PULLUP);
    digitalWrite(PANEL_PIN_LOAD,HIGH);
    digitalWrite(PANEL_PIN_ENABLE,HIGH);
}
void logPanel (byte incoming) {
    unsigned long l = millis() - lastPanelStatusLogged;
    if (l<1000) {
        return;
    }
    String panel = "";
    for(int i=5;i>=0;i--)
    {
        panel = panel + bitRead(incoming,i);
    }
    write_to_log(panel);
    lastPanelStatusLogged = millis();

}

bool stopOperationWithAnyButton() {
    bool output = false;
    for (int i=0; i<TOTAL_PUMPS; i++) {
        pumpState ps = pumpStates[i];
        if (ps.isWorking) {
            write_to_log("Need to stop pump %d due to button down",i);
            stopPump(i);
            output = true;
        }
    }
    return output;
}

void handleButtonDown(int buttonId) {
    currentButtonDown = buttonId;
    if (stopOperationWithAnyButton()) {
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

void loopPanel() {
    // return;
    if (millis() < 1000) {
        return;
    }
    unsigned long l = millis() - lastPanelCheck;
    if (l<10) {
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
    // logPanel(incoming);
    lastPanelCheck = millis();
    if (currentButtonDown>=0) {
        if (bitRead(incoming,currentButtonDown) ==0) {
            //button still down
            return;
        }
    }
    // return;
    for(int i=5;i>=0;i--)
    {
        if (bitRead(incoming,i) ==0) {
            handleButtonDown(i);
            return;
        }
    }
    currentButtonDown = -1;

}