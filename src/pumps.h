#ifndef _PUMPS_H_
#define _PUMPS_H_

#include "recepies.h"
#include "user_config.h"


void setupPumps();
void loopPumps();
void prepareDrink(recepie r);
void cleanPump(int pumpId);
void cleanAllPumps();
void pump(int pumpId, int milliliters);
void stopPump(int pumpId);

typedef struct { 
    int id;
    bool isWorking;
    unsigned long startTime;
    unsigned long scheduledStop;
} pumpState;

extern pumpState pumpStates[TOTAL_PUMPS];


#endif