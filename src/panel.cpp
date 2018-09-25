#include <Arduino.h>
#include "user_config.h"
#include "common.h"
#include "pumps.h"
#include "recepies.h"

unsigned long lastPanelCheck = 0;
unsigned long lastPanelStatusLogged = 0;
int buttonAssignments[] = BUTTON_RECEPIES;


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
    // return;
    for(int i=5;i>=0;i--)
    {
        if (bitRead(incoming,i) ==0) {
            int receipeId = buttonAssignments[i];
            if (receipeId==-1) {
                write_to_log("Button %d pushed but no receipe assigned", i);
                return;
            }
            write_to_log("Button %d pushed. Need to prepare recepie %d", i, receipeId);
            recepie r = getRecepieById(receipeId);
            prepareDrink(r);
            delay(500);
            return;
        }
    }

}