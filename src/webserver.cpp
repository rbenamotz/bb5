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


void handleNotFound() {
  write_to_log("not found \"%s\"",server.uri().c_str());
  server.send(404,"text/plain","not found");
}

void handleData() {
  char prep[100];
  char temp[100];
  for (int i=0; i<100; i++) {
    prep[i] = '\0';
  }
  if (currentlyPreping>=0) {
    recepie r = getRecepieById(currentlyPreping);
    strcpy(prep,r.name);
  }
  sprintf(temp,"{\"preparing\" : \"%s\", \"remainingTime\":%lu}", prep,remainingTime);
  server.send(200,"application/json",temp);
}

void handleGetPumps() {
  char temp[300];
  char output[500];
  strcpy(output,"[");
  for (int i=0; i<TOTAL_PUMPS; i++) {
    if (i>0) {
      strcat(output,",");
    }
    pumpState ps = pumpStates[i];
    unsigned long working = 0;
    unsigned long remaining =0;
    if (ps.isWorking) {
      unsigned long l = millis();
      working = l - ps.startTime;
      remaining = ps.scheduledStop - l;
    }
    sprintf(temp,"{\"id\":\"%d\",\"isWorking\":\"%d\",\"started\" : \"%lu\", \"remaining\" : \"%lu\"}",i+1,ps.isWorking,working,remaining);
    strcat(output,temp);
  }
  strcat(output,"]");
  server.send(200,"application/json", output);
}
void handlePrepDrink() {
  int recepieId = server.arg("plain").toInt();
  recepie r = getRecepieById(recepieId);
  if (r.id==-1) {
    server.send(404,"text/plain","Recepie not found");
    return;
  }
  char temp[50];
  sprintf(temp,"Preparing %s", r.name);
  server.send(200,"text/plain",temp);
  prepareDrink(r);
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

void handleStopAllPumps() {
  if (stopAllPumps()) {
    server.send(200,"text/plain","ok");
    return;
  }
  server.send(200,"text/plain","no pumps working");
}


//static pages
void handleIndexPage() {
  server.send(200,"text/html",STATIC_PAGE_INDEX);
}


void setupWebServer() {
  char buff[100];
  server.on("/log", handleLog);
  server.on("/api/ota", handleOta);
  server.on("/api/rst", handleRestart);
  server.on("/api/recepies", handleRecepies);
  server.on("/api/pumps", handleGetPumps);
  server.on("/api/data", handleData);
  server.on("/api/recepies/prepare", HTTP_POST, handlePrepDrink);
  for (int i=0; i<TOTAL_PUMPS; i++) {
    snprintf(buff, sizeof(buff), "/api/pumps/%d/clean",i+1);
    server.on(buff, handlePumpClean);
    snprintf(buff, sizeof(buff), "/api/pumps/%d/pump",i+1);
    server.on(buff,HTTP_POST, handlePumpPour);
  }
  server.on("/api/pumps/clean", handleCleanAll);
  server.on("/api/pumps/stop", handleStopAllPumps);
  //static
  server.on("/",handleIndexPage);
  server.onNotFound(handleNotFound);
  server.begin();
  write_to_log("Web server listening");
}

void loopWebServer() {
  server.handleClient();
}
