#include <Arduino.h>
#include "pumps.h"
#include "user_config.h"
#include "common.h"
#include "neo.h"
#include "mqtt.h"

#define PUMP_PRIME_VOLUME 5
#define PUMP_SHOT_VOLUME 60

int pumps[TOTAL_PUMPS];
int pumpsPins[TOTAL_PUMPS] = PUMPS_PINS;
pumpState pumpStates[TOTAL_PUMPS];
bool isPrepingDrink = false;
unsigned long remainingTime;
int currentlyPreping = -1;


void setupPumps() {
    pumps[0] = PUMP_ID_0;
    pumps[1] = PUMP_ID_1;
    pumps[2] = PUMP_ID_2;
    pumps[3] = PUMP_ID_3;
    for (int i=0; i<TOTAL_PUMPS; i++) {
        int pin = pumpsPins[i];
        pinMode(pin,OUTPUT);
        digitalWrite(pin,LOW);
        write_to_log("Pump %d set for pin %d",i, pumpsPins[i]);
    }
 }

void pump(int pumpId, int milliliters) {
    int pin = pumpsPins[pumpId];
    int duration =  milliliters / (PUMP_FLOW_ML_PER_SEC) * 1000;
    write_to_log ("Pouring %dml from pump %d for %d ms", milliliters, pumpId, duration);
    digitalWrite (pin,HIGH);
    unsigned long l = millis();
    pumpStates[pumpId].isWorking = true;
    pumpStates[pumpId].startTime = l;
    pumpStates[pumpId].scheduledStop = l + duration;
    startCupAnimation();
}


void stopPump(int pumpId) {
    int pin = pumpsPins[pumpId];
    digitalWrite (pin,LOW);
    pumpStates[pumpId].isWorking = false;
    write_to_log("Stopped pump %d",pumpId);
}

void cleanPump(int pumpId) {
    pump(pumpId,PUMP_CLEAN_VOLUME);
}

void cleanAllPumps() {
    int duration =  PUMP_CLEAN_VOLUME / (PUMP_FLOW_ML_PER_SEC) * 1000 / TOTAL_PUMPS;
    write_to_log("Cleaning all pumps for %dms", duration);
    for (int i=0 ; i<TOTAL_PUMPS; i++) {
        cleanPump(i);
    }
}

void primePump(int pumpId) {
    write_to_log("priming pump %d", pumpId);
    pump(pumpId,PUMP_PRIME_VOLUME);
}

void primeAllPumps() {
    write_to_log("Priming all pumps");
    for (int i=0 ; i<TOTAL_PUMPS; i++) {
        primePump(i);
    }
}

void shotPump(int pumpId) {
    write_to_log("shot from pump %d", pumpId);
    pump(pumpId,PUMP_SHOT_VOLUME);

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
    write_to_log ("Preparing %s" , r.name);
    mqttPublishCurrentDrink(r.name);
    currentlyPreping = r.id;
    isPrepingDrink = true;
    for (int i=0; i<r.totalSteps; i++) {
        preparationStep step = r.steps[i];
        pourStuff(step.ingredient,step.milliliters);
    }
}

bool stopAllPumps() {
    bool output = false;
    for (int i=0; i<TOTAL_PUMPS; i++) {
        pumpState ps = pumpStates[i];
        if (ps.isWorking) {
            write_to_log("Need to stop pump %d",i);
            stopPump(i);
            output = true;
        }
    }
    stopCupAnimation();
    return output;
}


void loopPumps() {
    int totalRunningPumps = 0;
    remainingTime = 0;
    for (int i=0; i<TOTAL_PUMPS; i++) {
        pumpState p = pumpStates[i];
        if (!p.isWorking) {
            continue;
        }
        if (millis() >= p.scheduledStop) {
            stopPump(i);
        } else {
            unsigned long l = p.scheduledStop - millis();
            if (l>remainingTime) {
                remainingTime = l;
            }
            totalRunningPumps++;
        }
    }
    if (totalRunningPumps == 0) {
        stopCupAnimation();
    }
    if (!isPrepingDrink) {
        return;
    }
    if (totalRunningPumps == 0) {
        stopCupAnimation();
        mqttPublishCurrentDrink("");
        currentlyPreping = -1;
        isPrepingDrink = false;
    }
}