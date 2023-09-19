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
extern int16_t distanceMax, threshold;

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
 * @brief Publish a status message to the MQTT broker
 * 
 * @param client_id  Client ID
 * @param ip_address  IP address
 * @param max_distance  Maximum distance in mm
 * @param threshold  Threshold in mm
 * @param status  Sensor status 
 */
void sendStatus(int16_t client_id, String ip_address, int max_distance, int threshold, String status) {
  // Create a JSON object and populate it
  StaticJsonDocument<200> jsonDoc;

  jsonDoc["client_id"] = client_id;
  jsonDoc["ip_address"] = ip_address;
  jsonDoc["max_distance"] = max_distance;
  jsonDoc["threshold"] = threshold;
  jsonDoc["status"] = status;

  // Serialize JSON object to a string buffer
  char jsonBuffer[200];
  serializeJson(jsonDoc, jsonBuffer);
  client.publish(generateTopic("status").c_str(), jsonBuffer);
}

/**
 * @brief Create a JSON string with the status of the sensor
 * 
 * @param client_id  Client ID
 * @param ip_address  IP address
 * @param max_distance  Maximum distance in mm
 * @param threshold  Threshold in mm
 * @param status  Sensor status
 * 
 * @return String
 */
String statusJson(int16_t client_id, String ip_address, int max_distance, int threshold, String status) {
  // JSON-buffer maken met voldoende capaciteit
  StaticJsonDocument<200> jsonBuffer;

  // JSON-object maken
  JsonObject jsonObject = jsonBuffer.to<JsonObject>();

  // Parameters toevoegen aan het JSON-object
  jsonObject["client_id"] = client_id;
  jsonObject["ip_address"] = ip_address;
  jsonObject["max_distance"] = max_distance;
  jsonObject["threshold"] = threshold;
  jsonObject["status"] = status;

  // JSON-gegevens omzetten naar een String
  String jsonString;
  serializeJson(jsonObject, jsonString);

  // JSON-string retourneren
  return jsonString;
}

/**
 * @brief Reconnect to the MQTT broker
 */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "Sensor-";
    clientId += sensorID;

    if (client.connect(clientId.c_str(), generateTopic("status").c_str(), 0, true, statusJson(sensorID, IP_Address, distanceMax, threshold, "offline").c_str())) {
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
 * @param client_id  Client ID
 * @param distance  Distance in mm
 */
void sendTrigger(int16_t client_id, int distance) {
  // Create a JSON object and populate it
  StaticJsonDocument<200> jsonDoc;

  jsonDoc["client_id"] = sensorID;
  jsonDoc["distance"] = distance;

  // Serialize JSON object to a string buffer
  char jsonBuffer[200];
  serializeJson(jsonDoc, jsonBuffer);
  client.publish(generateTopic("trigger").c_str(), jsonBuffer);
}

void setupMQTT() {
  client.setServer(mqttBroker, mqttPort);
  client.setCallback(callback);
}