#include "common.h"
#include "mqtt.h"
#include "pumps.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "neo.h"
#include "user_config.h"
#include <ArduinoJson.h>

#define MAX_ATTEMPTS_FOR_EACH_RECONNECT 1



WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastClientLoop = 0;
const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);
bool isTimerActive = true;

void publishLightState() {
  if (client.publish(MQTT_TOPIC_LIGHT_STATE,idleColorAsJson().c_str(),true)) {
    return;
  }
  write_to_log("Failed to publish light state");
}

void reconnect_to_mqtt() {
  if (client.connected()) {
    return;
  }
  int attempts = 0;
  while (!client.connected()) {
    if (attempts > MAX_ATTEMPTS_FOR_EACH_RECONNECT) {
      write_to_log("Could not connect to MQTT for %d times. Yielding",attempts);
      return;
    }
    attempts++;
    if (!client.connect(HOST_NAME,MQTT_USER,MQTT_PASS)) {
      write_to_log("Failed to connect to MQTT server \"%s\", rc=%d.",MQTT_HOST,client.state());
      continue;
    }
    write_to_log("Connected to MQTT server \"%s\" as %s",MQTT_HOST,MQTT_USER);
    client.subscribe(MQTT_TOPIC_CMD);
    client.subscribe(MQTT_TOPIC_LIGHT_CMD);
    publishLightState();
  }
}

void processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);
  if (!root.success()) {
    write_to_log("parseObject() failed");
    return;
  }
  bool lightRefreshNeeded = false;
  if (root.containsKey("state")) {
    bool b = (strcmp(root["state"], "ON") == 0);
    if (b!=contextLightState.state) {
      contextLightState.state = b;
      lightRefreshNeeded = true;
    }
  }
  if (root.containsKey("brightness")) {
    int br = root["brightness"];
    if (contextLightState.brightness != br) {
      write_to_log("Setting brightness to %d", br);
      contextLightState.brightness = br;
      lightRefreshNeeded = true;
    }

  }
  if (root.containsKey("color")) {
    int r = root["color"]["r"];
    int g = root["color"]["g"];
    int b = root["color"]["b"];
    write_to_log("Changing light color to %d,%d,%d",r,g,b);
    RgbColor c = RgbColor(r,g,b);
    if (c!=contextLightState.idleColor) {
      contextLightState.idleColor = c;
      lightRefreshNeeded = true;
    }
  }
  if (lightRefreshNeeded) {
    refreshNeo();
  }
  publishLightState();
}


void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  processJson(message);
}

void setupMqtt() {
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

void mqttPublishCurrentDrink(const char* status) {
  client.publish(MQTT_TOPIC_DRINK_STATE, status);
}

void loopMqtt() {
  unsigned long l = millis() - lastClientLoop;
  if (l<200) {
    return;
  }
  reconnect_to_mqtt();
  if (remainingTime ==0 && isTimerActive) {
    client.publish(MQTT_TOPIC_REMAINING_TIME,"");
    client.publish(MQTT_TOPIC_DRINK_STATE,"");
    isTimerActive = false;
  }
  if (remainingTime > 0) {
    char temp[10];
    sprintf(temp,"%d",remainingTime / 1000);
    client.publish(MQTT_TOPIC_REMAINING_TIME,temp);
    isTimerActive = true;
  }
  client.loop();
  lastClientLoop = millis();
}
