/**
Basalt - @Klaas Schoute
Build for board: ESP32

Sensor code for the Stair Challenge - @Basalt
*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L1X.h"

// Project includes
#include "sensor/id.h"
#include "sensor/status_led.h"
#include "communications/wifi.h"
#include "communications/webserver.h"
#include "communications/mqtt.h"
#include "secrets/config.h"

// I2C Pins
#define PIN_SDA   10
#define PIN_SCL	  8

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

bool IsCalibrated = false;
bool showError = false;
bool showMeasure = false;
bool debug = false;

int16_t sensorID;
int16_t distanceMax, threshold;
const int16_t maxDistanceBufferInmm = 50;

/**
 * @brief Calibrate the sensor
 */
void startCalibration() {
  // calculate the max distance, subtract a buffer and set threshold.
  distanceMax = vl53.distance() - maxDistanceBufferInmm;

  // Set the threshold to half the max distance
  if (distanceMax >= 1300) {
    distanceMax = 1300;
  }
  threshold = distanceMax / 2;

  Serial.print(F("Max distance (mm): "));
  Serial.print(distanceMax);
  Serial.println();

  Serial.print(F("Threshold (mm): "));
  Serial.print(threshold);
  Serial.println();

  for (int i = 0; i < 2; i++) {
    // Fade to blue
    fadeToColor(0x0000FF, 0x000000, 1000);
    fadeToColor(0x000000, 0x0000FF, 1000);
  }

  IsCalibrated = true;
}

/**
 * @brief Setup function
 */
void setup() {
  Serial.begin(115200);
  while(!Serial) delay(10);

  Wire.begin(PIN_SDA, PIN_SCL);
  Serial.println("Starting setup");

  // Set sensor ID
  sensorID = setSensorID();
  Serial.print(F("Sensor ID: "));
  Serial.println(sensorID);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  rainbow(20); // Rainbow animation

  // Setup the wifi, OTA and MQTT
  setupWiFi();
  setupOTA();
  setupMQTT();

  if (! vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    // Stop
    while(1) delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  Serial.print(F("TOF Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    // Stop
    while(1) delay(10);
  }
  Serial.println(F("Ranging started"));

  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(20);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  // Calibrate the sensor
  startCalibration();

  // Setup the webserver
  setupWebserver();

  /*
  vl.VL53L1X_SetDistanceThreshold(100, 300, 3, 1);
  vl.VL53L1X_SetInterruptPolarity(0);
  */

  // Succesful boot animation 
  for (int i = 0; i < 2; i++) {
    // Fade to green
    fadeToColor(0x00FF00, 0x000000, 500);
    fadeToColor(0x000000, 0x00FF00, 500);
  }
}

/**
 * @brief if a new reading is available from the sensor
 * @return distance in mm, 0 for no reading available, -1 for error 
 */
int16_t readDistance() {
  int16_t distance = 0;
  if (vl53.dataReady()) {
    // new measurement for the taking!
    distance = vl53.distance();
    if (distance == -1) {
      // something went wrong!
      Serial.print(F("Couldn't get distance: "));
      Serial.println(vl53.vl_status);
    }
    else {
      // data is read out, time for another reading!
      vl53.clearInterrupt();
    }
  }
  return distance;
}

/**
 * @brief Loop function
 */
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Set RGB LED to black / off
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();

  int16_t distance;
  distance = readDistance();

  // Send status info to MQTT
  if (!showMeasure) {
    sendStatus(sensorID, IP_Address, distanceMax, threshold, "measure");
    showMeasure = true;
  }

  if (distance == -1) {
    // something went wrong!
    Serial.print(F("Couldn't get distance: "));
    Serial.println(vl53.vl_status);

    if (!showError) {
      sendStatus(sensorID, IP_Address, distanceMax, threshold, "error");
      showError = true;
    }

    // Fade to red
    fadeToColor(0xFF0000, 0x000000, 1000);
    fadeToColor(0x000000, 0xFF0000, 1000);
    return;
  } else if(distance != 0 && distance != -1) {
    // Reset error
    showError = false;
    if (distance <= threshold && IsCalibrated) {
      Serial.println("Persoon op de traptreden!");
      pixels.setPixelColor(0, pixels.Color(0, 25, 0));
      pixels.show();

      // Publish MQTT message
      sendTrigger(sensorID, distance);
      sendStatus(sensorID, IP_Address, distanceMax, threshold, "trigger");
      delay(3000);
      showMeasure = false;
    }

    // Print the distance
    if (debug) {
      Serial.print(F("Distance: "));
      Serial.print(distance);
      Serial.println(" mm");
    }
  }
  delay(50);
}