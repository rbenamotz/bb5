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


void buildStepsAsJson(recepie r, char* buffer) {
  char temp[100];
  strcpy(buffer,"[");
  for (int i=0; i<r.totalSteps; i++) {
    if (i>0) {
      strcat(buffer,",");
    }
    preparationStep st = r.steps[i];
    sprintf(temp,"{\"ingredient\":\"%s\",\"quantity\":\"%d\"}",getIngredientName(st.ingredient) ,st.milliliters);
    strcat(buffer,temp);
  }
  strcat(buffer,"]");
}

void handleRecepies() {
  char temp[300];
  char output[500];
  strcpy(output,"[");
  for (int i=0; i<TOTAL_RECEPIES; i++) {
    if (i>0) {
      strcat(output,",");
    }
    recepie r = getRecepieById(i);
    sprintf(temp,"{\"id\":\"%d\",\"name\":\"%s\",\"steps\" : ",r.id,r.name);
    strcat(output,temp);
    buildStepsAsJson (r,temp);
    strcat(output,temp);
    strcat(output,"}");
  }
  strcat(output,"]");
  server.send(200,"application/json", output);
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
  sprintf(temp,"Pumping %dml from pump %d", ml, pumpId + 1);
  server.send(200,"text/plain",temp);
  pump(pumpId,ml);
}

void handleCleanAll() {
  cleanAllPumps();
}


void setupWebServer() {
  char buff[100];
  server.on("/log", handleLog);
  server.on("/ota", handleOta);
  server.on("/rst", handleRestart);
  server.on("/recepies", handleRecepies);
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
