/**
 * @file id.h
 * @author Klaas Schoute - Basalt
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

extern bool debug;

/**
 * @brief Convert a binary string to an integer
 * 
 * @param binaryString
 * @return int
 */
int binaryToInteger(const String& binaryString) {
  int integerValue = 0;
  int length = binaryString.length();
  
  for (int i = 0; i < length; i++) {
    int bitValue = binaryString.charAt(i) - '0';
    integerValue = (integerValue << 1) | bitValue;
  }
  
  return integerValue;
}

/**
 * @brief Set the Sensor ID object
 * 
 * @return int
 */
int setSensorID() {
  String dip = "";

  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  if (digitalRead(7) == LOW) {dip += "1";} else {dip += "0";}
  if (digitalRead(6) == LOW) {dip += "1";} else {dip += "0";}
  if (digitalRead(5) == LOW) {dip += "1";} else {dip += "0";}
  if (digitalRead(4) == LOW) {dip += "1";} else {dip += "0";}

  if (debug) {
    Serial.println(dip);
  }
  return binaryToInteger(dip);
}