/**
Basalt - @Klaas Schoute
Build for board: ESP32

Sensor code for the Stair Challenge - @Basalt
*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L1X.h"
#include "sensor/id.h"
#include "sensor/status_led.h"

// I2C Pins
#define PIN_SDA   10
#define PIN_SCL	  8

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

bool IsCalibrated = false;
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

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  Wire.begin(PIN_SDA, PIN_SCL);
  Serial.println("Starting setup");

  if (! vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    // Stop
    while(1) delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    // Stop
    while(1) delay(10);
  }
  Serial.println(F("Ranging started"));

  // Set sensor ID
  sensorID = setSensorID();
  Serial.print(F("Sensor ID: "));
  Serial.println(sensorID);

  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  // Calibrate the sensor
  startCalibration();

  /*
  vl.VL53L1X_SetDistanceThreshold(100, 300, 3, 1);
  vl.VL53L1X_SetInterruptPolarity(0);
  */
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
  // Set RGB LED to black / off
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();

  int16_t distance;
  distance = readDistance();

  if (distance == -1) {
    // something went wrong!
    Serial.print(F("Couldn't get distance: "));
    Serial.println(vl53.vl_status);

    // Fade to red
    fadeToColor(0xFF0000, 0x000000, 1000);
    fadeToColor(0x000000, 0xFF0000, 1000);
    return;
  } else if(distance != 0 && distance != -1) {
    if (distance <= threshold && IsCalibrated) {
      Serial.println("Persoon op de traptreden!");
      pixels.setPixelColor(0, pixels.Color(0, 25, 0));
      pixels.show();
      delay(5000);
    }

    // Print the distance
    if (debug) {
      Serial.print(F("Distance: "));
      Serial.print(distance);
      Serial.println(" mm");
    }
  }
  delay(100);
}