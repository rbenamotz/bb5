#include "recepies.h"

#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

//hardware



// pumps
#define TOTAL_PUMPS 4
#define PUMPS_PINS {14,12,13,15}

#define PUMP_ID_0 SOURCE_ID_VOKDA
#define PUMP_ID_1 SOURCE_ID_GINGER_BEER
#define PUMP_ID_2 SOURCE_ID_LIME_JUICE
#define PUMP_ID_3 SOURCE_ID_BLOODY_MARRY_MIX

#define PUMP_FLOW_ML_PER_SEC 100.0/60.0
#define PUMP_CLEAN_VOLUME 250


//Panel

#define PANEL_PIN_ENABLE 5
#define PANEL_PIN_LOAD 4
#define PANEL_PIN_CLOCK 0
#define PANEL_PIN_DATA 2
//CLK INH purple, SH_LD yellow, CLK orange, Qh green

#define BUTTON_RECEPIES {RECEPIE_ID_MOSCOW_MULE, RECEPIE_ID_BLOODY_MARRY, -1, -1, -1, -1}



//Logging
#define LOG_TO_SERIAL false
#define LOG_SIZE 20

//WiFi
#define WIFI_SSID "ssid
#define WIFI_PASS "password"
#define HOST_NAME "bb5"
// #define IP_ADDR 192, 168, 1, 3
// #define IP_GW 192,168,1,1
// #define IP_SUBNET 255,255,255,0

//MQTT
#define MQTT_HOST "ip address"
#define MQTT_PORT 1883
#define MQTT_USER "user"
#define MQTT_PASS "password!"
#define MQTT_TOPIC_OUT_DRINK "/bb5/drink"
#define MQTT_TOPIC_CMD "/bb5/cmd"



#endif
