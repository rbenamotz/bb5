#include "common.h"
#include "mqtt.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "user_config.h"

#define MAX_ATTEMPTS_FOR_EACH_RECONNECT 1



WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastClientLoop = 0;

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
  }
}

void setupMqtt() {
  client.setServer(MQTT_HOST, MQTT_PORT);
}

void mqttPublishCurrentDrink(const char* status) {
  client.publish(MQTT_TOPIC_OUT_DRINK, status);
}

void loopMqtt() {
  unsigned long l = millis() - lastClientLoop;
  if (l<500) {
    return;
  }
  reconnect_to_mqtt();
  client.loop();
  lastClientLoop = millis();
}
