/**
 * @file mqtt.h
 * @author Klaas Schoute - Basalt
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <PubSubClient.h>
#include <WiFiClient.h>

extern const char *subscribeTopic;
extern int16_t sensorID;

extern WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    Serial.print(message += (char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "Sensor-";
    clientId += sensorID;

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(subscribeTopic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setupMQTT() {
  client.setServer(mqttBroker, mqttPort);
  client.setCallback(callback);
}