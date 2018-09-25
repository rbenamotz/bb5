#include <Arduino.h>
#include "pumps.h"
#include "user_config.h"
#include "common.h"

int pumps[TOTAL_PUMPS];
int pumpsPins[TOTAL_PUMPS] = PUMPS_PINS;

void setupPumps() {
    pumps[0] = PUMP_ID_0;
    pumps[1] = PUMP_ID_1;
    pumps[2] = PUMP_ID_2;
    pumps[3] = PUMP_ID_3;
    for (int i=0; i<TOTAL_PUMPS; i++) {
        int pin = pumpsPins[i];
        pinMode(pin,OUTPUT);
        digitalWrite(pin,HIGH);
        delay(10);
        digitalWrite(pin,LOW);
        write_to_log("Pump %d set for pin %d",i, pumpsPins[i]);
    }
 }

void loopPumps() {

}

void pump(int pumpId, int milliliters) {
    int pin = pumpsPins[pumpId];
    int duration =  milliliters / (PUMP_FLOW_ML_PER_SEC) * 1000;
    write_to_log ("Pouring %dml from pump %d for %d ms", milliliters, pumpId, duration);
    digitalWrite (pin,HIGH);
    delay(duration);
    // delay(3000);
    digitalWrite (pin,LOW);
}


void cleanPump(int pumpId) {
    pump(pumpId,PUMP_CLEAN_VOLUME);
    write_to_log("Done clenaing");
}

void cleanAllPumps() {
    int duration =  PUMP_CLEAN_VOLUME / (PUMP_FLOW_ML_PER_SEC) * 1000 / TOTAL_PUMPS;
    write_to_log("Cleaning all pumps for %dms", duration);
    for (int i=0 ; i<TOTAL_PUMPS; i++) {
        int pin = pumpsPins[i];
        digitalWrite (pin,HIGH);
    }
    delay(duration);
    for (int i=0 ; i<TOTAL_PUMPS; i++) {
        int pin = pumpsPins[i];
        digitalWrite (pin,LOW);
    }
    write_to_log("Done clenaing");

}

int pumpIdByIngredientId(int ingrdientId) {
    for (int i=0; i<TOTAL_PUMPS; i++) {
        if (pumps[i] == ingrdientId) {
            return i;
        }
    }
    return -1;
}


void pourStuff(int ingrdientId, int milliliters) {
    int pumpId = pumpIdByIngredientId(ingrdientId);
    if (pumpId == -1) {
        return;
    }
    pump(pumpId, milliliters);
}

void prepareDrink(recepie r) {
    write_to_log ("Preparing recepie %d" , r.id);
    for (int i=0; i<r.totalSteps; i++) {
        preparationStep step = r.steps[i];
        pourStuff(step.ingredient,step.milliliters);
    }
}


