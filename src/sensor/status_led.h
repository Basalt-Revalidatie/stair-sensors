/**
 * @file status_led.h
 * @author Klaas Schoute - Basalt
 * @brief 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Onboard RGB LED (NeoPixel)
#define PIXELPIN  2 // 2 for C3 Mate
#define NUMPIXELS 1

extern bool debug;

Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

/**
 * @brief Fade to a color
 * 
 * @param fromColor
 * @param toColor
 * @param duration
 */
void fadeToColor(uint32_t fromColor, uint32_t toColor, int duration) {
  for (int i = 0; i <= 255; i++) {
    uint8_t r = map(i, 0, 255, (fromColor >> 16) & 0xFF, (toColor >> 16) & 0xFF);
    uint8_t g = map(i, 0, 255, (fromColor >> 8) & 0xFF, (toColor >> 8) & 0xFF);
    uint8_t b = map(i, 0, 255, fromColor & 0xFF, toColor & 0xFF);

    uint32_t color = (r << 16) | (g << 8) | b;

    pixels.setPixelColor(0, color);
    pixels.show();
    delay(duration / 255);
  }
}

/**
 * @brief Rainbow cycle animation
 * 
 * @param WheelPos
 * @return uint32_t
 */
uint32_t wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/**
 * @brief Rainbow animation
 * 
 * @param delayTime
 */
void rainbow(int delayTime) {
  for (int j = 0; j < 255; j++) {
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, wheel((i + j) & 255));
    }
    pixels.show();
    delay(delayTime);
  }
}