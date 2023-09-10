/**
 * @file wifi.h
 * @author Klaas Schoute - Basalt
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <WiFi.h>

// Project includes
#include "secrets/config.h"

extern int16_t sensorID;

WiFiClient espClient;

/**
 * @brief Setup the WiFi connection
 */
void setupWiFi() {
    WiFi.mode(WIFI_STA); //Optional
    WiFi.setHostname(("Stair Sensor-" + String(sensorID)).c_str());
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    // Show connection details
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Default Hostname: ");
    Serial.println(WiFi.getHostname());
}