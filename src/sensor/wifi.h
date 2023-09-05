/**
 * @file wifi.h
 * @author Klaas Schoute - Basalt
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <WiFi.h>
#include "../secrets/env.h"

/**
 * @brief Setup the WiFi connection
 * 
 */
void setupWiFi() {
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}