/******************************************************************
 A library for controlling a set of 8x8 LEDs with a MAX7219 or
 MAX7221 displays.
 This is a plugin for Adafruit's core graphics library, providing
 basic graphics primitives (points, lines, circles, etc.).
 You need to download and install Adafruit_GFX to use this library.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!

 Written by Mark Ruys.
 Contains fixes by rob040@user.github.com to support 32 and more display tiles
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/

#include <Adafruit_GFX.h>
#include "Max72xxPanel.h"
#include <SPI.h>

// The opcodes for the MAX7221 and MAX7219
#define OP_NOOP         0
#define OP_DIGIT0       1
#define OP_DIGIT1       2
#define OP_DIGIT2       3
#define OP_DIGIT3       4
#define OP_DIGIT4       5
#define OP_DIGIT5       6
#define OP_DIGIT6       7
#define OP_DIGIT7       8
#define OP_DECODEMODE   9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

Max72xxPanel::Max72xxPanel(uint8_t csPin, uint8_t hDisplays, uint8_t vDisplays) : Adafruit_GFX(hDisplays << 3, vDisplays << 3) {

  Max72xxPanel::SPI_CS = csPin;

  uint16_t displays = hDisplays * vDisplays;
  Max72xxPanel::hDisplays = hDisplays;
  Max72xxPanel::bitmapSize = displays << 3;

  Max72xxPanel::bitmap = (uint8_t*)malloc(bitmapSize);
  Max72xxPanel::matrixRotation = (uint8_t*)malloc(displays);
  Max72xxPanel::matrixPosition = (uint8_t*)malloc(displays);

  for ( uint8_t display = 0; display < displays; display++ ) {
    matrixPosition[display] = display;
    matrixRotation[display] = 0;
  }

  SPI.begin();
//SPI.setBitOrder(MSBFIRST);
//SPI.setDataMode(SPI_MODE0);
  pinMode(SPI_CS, OUTPUT);

  reset();
}

void Max72xxPanel::reset() {

  // Clear the screen
  fillScreen(0);

  // Make sure we are not in test mode
  spiTransfer(OP_DISPLAYTEST, 0);

  // We need the multiplexer to scan all segments
  spiTransfer(OP_SCANLIMIT, 7);

  // We don't want the multiplexer to decode segments for us
  spiTransfer(OP_DECODEMODE, 0);

  // Enable display
  shutdown(false);

  // Set the brightness to a medium value
  setIntensity(7);
}

void Max72xxPanel::setPosition(uint8_t display, uint8_t x, uint8_t y) {
  matrixPosition[x + hDisplays * y] = display;
}

void Max72xxPanel::setRotation(uint8_t display, uint8_t rotation) {
  matrixRotation[display] = rotation;
}

void Max72xxPanel::setRotation(uint8_t display, Rotation rotation) {
  matrixRotation[display] = static_cast<uint8_t>(rotation);
}

void Max72xxPanel::setRotation(uint8_t rotation) {
  Adafruit_GFX::setRotation(rotation);
}

void Max72xxPanel::setRotation(Rotation rotation) {
  Adafruit_GFX::setRotation(static_cast<uint8_t>(rotation));
}

void Max72xxPanel::shutdown(bool b) {
  spiTransfer(OP_SHUTDOWN, b ? 0 : 1);
}

void Max72xxPanel::setIntensity(uint8_t intensity) {
  spiTransfer(OP_INTENSITY, intensity);
}

void Max72xxPanel::fillScreen(uint16_t color) {
  memset(bitmap, color ? 0xff : 0, bitmapSize);
}

void Max72xxPanel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  // The x and y args and calculations need to be 16-bit ints to not limit the display
  // size to 31 tiles of 8x8 or 248 pixels.
   // [rob040] fix 32 display limit

  if ( rotation ) {
    // Implement Adafruit's rotation.
    if ( rotation >= 2 ) {                    // rotation == 2 || rotation == 3
      x = _width - 1 - x;
    }

    if ( rotation == 1 || rotation == 2 ) {   // rotation == 1 || rotation == 2
      y = _height - 1 - y;
    }

    if ( rotation & 1 ) {                     // rotation == 1 || rotation == 3
      int16_t tmp = x; x = y; y = tmp;
    }
  }

  if ( x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT ) {
    // Ignore pixels outside the canvas.
    return;
  }

  // Translate the x, y coordinate according to the layout of the
  // displays. They can be ordered and rotated (0, 90, 180, 270).

  uint8_t display = matrixPosition[(x >> 3) + hDisplays * (y >> 3)];
  x &= 0b111;
  y &= 0b111;

  uint8_t r = matrixRotation[display];
  if ( r >= 2 ) {                      // 180 or 270 degrees
    x = 7 - x;
  }
  if ( r == 1 || r == 2 ) {            // 90 or 180 degrees
    y = 7 - y;
  }
  if ( r & 1 ) {                       // 90 or 270 degrees
    int16_t tmp = x; x = y; y = tmp;
  }

  uint8_t d = display / hDisplays;
  x += (display - d * hDisplays) << 3; // x += (display % hDisplays) * 8
  y += d << 3;                         // y += (display / hDisplays) * 8

  // Update the color bit in our bitmap buffer.

  uint8_t *ptr = bitmap + x + WIDTH * (y >> 3);
  uint8_t val = 1 << (y & 0b111);

  if ( color ) {
    *ptr |= val;
  }
  else {
    *ptr &= ~val;
  }
}

void Max72xxPanel::write() {
  // Send the bitmap buffer to the displays.

  for ( uint8_t row = OP_DIGIT7; row >= OP_DIGIT0; row-- ) {
    spiTransfer(row);
  }
}

void Max72xxPanel::spiTransfer(uint8_t opcode, uint8_t data) {
  // If opcode > OP_DIGIT7, send the opcode and data to all displays.
  // If opcode <= OP_DIGIT7, display the column with data in our buffer for all displays.
  // We do not support (nor need) to use the OP_NOOP opcode.

  // Enable the line
  digitalWrite(SPI_CS, LOW);

  // Now shift out the data, two bytes per display. The first byte is the opcode,
  // the second byte the data.
  uint16_t end = opcode - OP_DIGIT0;
  uint16_t start = bitmapSize + end;
  do {
    start -= 8;
    SPI.transfer(opcode);
    SPI.transfer(opcode <= OP_DIGIT7 ? bitmap[start] : data);
  }
  while ( start > end );

  // Latch the data onto the display(s)
  digitalWrite(SPI_CS, HIGH);
}
