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
#include <ArduinoJson.h>

extern const char *subscribeTopic;
extern int16_t sensorID;

extern WiFiClient espClient;
PubSubClient client(espClient);

// MQTT topics
const char *subscribeTopic = "settings";

/**
 * @brief Generate a topic for the sensor
 * 
 * @param type 
 * @return String 
 */
String generateTopic(String type) {
  return "sensor/" + String(sensorID) + "/" + String(type) ;
}

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

/**
 * @brief Reconnect to the MQTT broker
 */
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

      // Fade to purple
      fadeToColor(0x800080, 0x000000, 1000);
      fadeToColor(0x000000, 0x800080, 1000);

      delay(5000);
    }
  }
}

/**
 * @brief Publish a trigger message to the MQTT broker
 * 
 * @param distance  Distance in mm
 * @param max_distance  Maximum distance in mm
 * @param threshold  Threshold in mm
 */
void trigger(int distance, int max_distance, int threshold) {
  // Create a JSON object and populate it
  StaticJsonDocument<200> jsonDoc;

  jsonDoc["distance"] = distance;
  jsonDoc["max_distance"] = max_distance;
  jsonDoc["threshold"] = threshold;

  // Serialize JSON object to a string buffer
  char jsonBuffer[200];
  serializeJson(jsonDoc, jsonBuffer);
  client.publish(generateTopic("trigger").c_str(), jsonBuffer);
}

void setupMQTT() {
  client.setServer(mqttBroker, mqttPort);
  client.setCallback(callback);
}