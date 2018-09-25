#include "webserver.h"
#include <ESP8266WebServer.h>
#include "common.h"
#include "pumps.h"
#include "user_config.h"
#include "ota.h"


ESP8266WebServer server(80);

void buildJsonInt(char* output, String name, int value) {
  char temp[100];
  sprintf(temp,", \"%s\" : %d",name.c_str(),value);
  strcat(output,temp);
}


void handleData() {
  char output[200];
  // char temp[20];
  strcpy(output,"{");
  // for (int i=0; i<TOTAL_DOORS; i++) {
  //   if (is_door_open[i]) {
  //     sprintf(temp,"\"door%d\" : \"%s\"",i+1,"open");
  //   } else {
  //     sprintf(temp,"\"door%d\" : \"%s\"",i+1,"closed");
  //   }
  //   if (i>0) {
  //     strcat(output,",");
  //   }
  //   strcat(output, temp);
  // }
  // buildJsonInt(output, "ota",is_ota_enabled);
  strcat(output,"}");
  server.send(200, "application/json", output);
}


void handleLog() {
  server.send(200,"text/plain",read_log_buffer());
}

void handleOta() {
  is_ota_enabled = !is_ota_enabled;
  char temp[10];
  sprintf(temp,"{\"ota\" : %d}", is_ota_enabled);
  server.send(200,"application/json",temp);
}

void handleRestart() {
  write_to_log("Restarting due to API request");
  ESP.restart();
}
//Pumps

int extractPumpId() {
  char c = server.uri().charAt(7);
  int pumpId = (int) (c - '0') - 1;
  if (pumpId<0 || pumpId >= TOTAL_PUMPS) {
    char temp[10];
    sprintf(temp,"Invalid pump number %c",c);
    server.send(400,"text/plain",temp);
    return -1;    
  }
  return pumpId;

}
void handlePumpClean() {
  int pumpId = extractPumpId();
  if (pumpId==-1) {
    return;
  }
  char temp[10];
  sprintf(temp,"Cleaning pump %d",pumpId + 1);
  server.send(200,"text/plain",temp);
  cleanPump(pumpId);
}

void handlePumpPour() {
  int pumpId = extractPumpId();
  if (pumpId==-1) {
    return;
  }
  int ml = server.arg("plain").toInt();
  char temp[10];
  sprintf(temp,"Pumping %d from pump %d", ml, pumpId + 1);
  server.send(200,"text/plain",temp);
  pump(pumpId,ml);
}

void handleCleanAll() {
  cleanAllPumps();
}


void setupWebServer() {
  char buff[100];
  server.on("/log", handleLog);
  server.on("/data", handleData);
  server.on("/ota", handleOta);
  server.on("/rst", handleRestart);
  for (int i=0; i<TOTAL_PUMPS; i++) {
    snprintf(buff, sizeof(buff), "/pumps/%d/clean",i+1);
    server.on(buff, handlePumpClean);
    snprintf(buff, sizeof(buff), "/pumps/%d/pump",i+1);
    server.on(buff,HTTP_POST, handlePumpPour);
  }
  server.on("/pumps/clean", handleCleanAll);
  server.begin();
  write_to_log("Web server listening");
}

void loopWebServer() {
  server.handleClient();
}
