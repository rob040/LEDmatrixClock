/******************************************************************
 A library for controlling a set of 8x8 LEDs with a MAX7219 or
 MAX7221 displays.

 This is a plugin for Adafruit's core graphics library, providing
 basic graphics primitives (points, lines, circles, etc.).
 You need to download and install Adafruit_GFX to use this library.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!

 Written by Mark Ruys, 2013.
 Contains fixes by rob040@user.github.com
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheet: http://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf

 ******************************************************************/

#ifndef Max72xxPanel_h
#define Max72xxPanel_h

#if (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
  #include "pins_arduino.h"
#endif

class Max72xxPanel : public Adafruit_GFX {

public:

  /*
   * Create a new controller
   * Parameters:
   * csPin    pin for selecting the device
   * hDisplays  number of displays horizontally
   * vDisplays  number of displays vertically
   */
  Max72xxPanel(uint8_t csPin, uint8_t hDisplays=1, uint8_t vDisplays=1);

  /*
   * Define how the displays are ordered. The first display (0)
   * is the one closest to the Arduino.
   */
  void setPosition(uint8_t display, uint8_t x, uint8_t y);

  /*
   * Define if and how the displays are rotated. The first display
   * (0) is the one closest to the Arduino. rotation can be:
   *   0: no rotation
   *   1: 90 degrees clockwise
   *   2: 180 degrees
   *   3: 90 degrees counter clockwise
   */
  void setRotation(uint8_t display, uint8_t rotation);

  /*
   * Implementation of Adafruit's setRotation(). Probably, you don't
   * need this function as you can achieve the same result by using
   * the previous two functions.
   */
  void setRotation(uint8_t rotation);

  /*
   * Draw a pixel on your canvas. Note that for performance reasons,
   * the pixels are not actually send to the displays. Only the internal
   * bitmap buffer is modified.
   */
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  /*
   * As we can do this much faster then setting all the pixels one by
   * one, we have a dedicated function to clear the screen.
   * The color can be 0 (blank) or non-zero (pixel on).
   */
  void fillScreen(uint16_t color);

  /*
   * Set the shutdown (power saving) mode for the device
   * Parameters:
   * status If true the device goes into power-down mode. Set to false
   *    for normal operation.
   */
  void shutdown(bool status);

  /*
   * Set the brightness of the display.
   * Parameters:
   * intensity  the brightness of the display. (0..15)
   */
  void setIntensity(uint8_t intensity);

  /*
   * After you're done filling the bitmap buffer with your picture,
   * send it to the display(s).
   */
  void write();

private:
  uint8_t SPI_CS; /* SPI chip selection */

  /* Send out a single command to the device */
  void spiTransfer(uint8_t opcode, uint8_t data=0);

  /* We keep track of the led-status for 8 devices in this array */
  uint8_t *bitmap;
  uint16_t bitmapSize; // [rob040] fix 32 display limit

  uint8_t hDisplays;
  uint8_t *matrixPosition;
  uint8_t *matrixRotation;
};

#endif  // Max72xxPanel_h



