/**
 * @file mqtt_topics.h
 * @author Klaas Schoute - Basalt
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

extern int16_t sensorID;

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