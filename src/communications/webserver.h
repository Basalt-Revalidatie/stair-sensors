/**
 * @file ota.h
 * @author Klaas Schoute - Basalt
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

AsyncWebServer server(80);
extern int16_t sensorID;
extern int16_t distanceMax;

/**
 * @brief Setup the OTA server
 */
void setupOTA() {
  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");
}

void handleReset() {
  ESP.restart();
}

void setupWebserver() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String htmlContent = "<html><head><style>"
                        "body { font-family: Arial, sans-serif; background-color: #f2f2f2; }"
                        "h1 { text-align: center; color: #333; }"
                        ".container { display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; }"
                        "form { margin-top: 20px; }"
                        "button { padding: 10px 20px; background-color: #AF4C4C; color: white; border: none; cursor: pointer; font-size: 16px; }"
                        "</style></head><body><div class=\"container\">"
                        "<h1>Stair Sensor</h1>"
                        "<p>Sensor ID: " + String(sensorID) + "</p>"
                        "<p>Max Distance: " + String(distanceMax) + "</p>"
                        "<form action=\"/reset\" method=\"post\">"
                        "<button type=\"submit\">Reset</button>"
                        "</form></div></body></html>";

    request->send(200, "text/html", htmlContent);
  });

  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request){
    handleReset();
  });
}