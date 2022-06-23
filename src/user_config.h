#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

// hardware

// pumps
#define TOTAL_PUMPS 4
#define PUMPS_PINS {14, 12, 13, 15}


#define PUMP_FLOW_ML_PER_SEC 100.0 / 60.0
#define PUMP_CLEAN_VOLUME 250

// Panel

#define PANEL_PIN_ENABLE 5
#define PANEL_PIN_LOAD 4
#define PANEL_PIN_CLOCK 0
#define PANEL_PIN_DATA 2

// neo
#define PIN_NEO_PIXEL 2
#define PIXEL_COUNT 13



// Logging
#define LOG_TO_SERIAL false
#define LOG_SIZE 20

// WiFi
#define HOST_NAME "bb5"

#endif
