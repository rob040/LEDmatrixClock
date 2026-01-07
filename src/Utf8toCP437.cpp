/**
 * @file Utf8toCP437.cpp
 * @brief Convert UTF-8 strings to ASCII with CP437 extension for LED matrix display
 * @author rob040@users.github.com,  https://github.com/rob040
 * @date 2025-09-20
 * @copyright Copyright (c) 2025
 * @license Licensed under the MIT license. See LICENSE file in the project root for details.
 *
 * Here we convert UTF8 encoded character string to a UNICODE wide char codepoint and is then converted to an ASCII-extended code page 437.
 * CP437 is the original IBM PC character set with some tweaks (e.g. Pasetas symbol is replaced with Euro symbol).
 * CP437 is the default font used in Adafruit GFX library, used in our LED Matrix display.
 *
 * Info:
 * https://en.wikipedia.org/wiki/UTF-8
 * https://en.wikipedia.org/wiki/Code_page_437
 * https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/PC/CP437.TXT
 *
 */


#include <Arduino.h>
#include "Utf8toCP437.h"

// Lookup table for Unicode codepoints to CP437 character code translation, SORTED on codepoints.
// Note that ASCII codes 0x20-0x7F and control codes 0x00-0x1F are not included, except when higher unicode is
// mapped to ordinary ASCII character or to non-printable NUL character.
#include "Utf8ConvTable.h"

// Unicode codepoints in UTF-8 character streams conversions
// U+0000 - U+001F  non-printable characters are passed without conversion, but the display routine will reject these.
// U+0020 - U+007F  Ascii-96, no conversion
// U+0080 - U+07FF  Convert 2-byte UTF-8 to 16-bit Unicode
// U+0800 - U+FFFF  Convert 3-byte UTF-8 to 16-bit Unicode
// U+FFFF - U+10FFFF 4-byte UTF-8 are all rejected
// Invalid
// rejected character codes are either not printed, or print an upside-down question mark '¿' '/xA8' (CP437_FALLBACK_CHAR)

// UTF-8 decoder; return Unicode codepoint
// Advances the input string pointer *s to the next character
// If invalid UTF-8 byte sequence is found, return 0 and advance *s by one byte
// Does also check for overlong sequences and > 16-bit codepoints (4-byte UTF-8)
uint16_t decodeUTF8toUnicode(char **s) {
  uint8_t *cp = (uint8_t*)*s;
  uint8_t c = *cp;
  uint16_t rv = 0; // default to invalid UTF-8 byte (-sequence)
  *s += 1;
  if (c < 0x80) {
    rv = c;
  } else if (((c & 0xE0) == 0xC0) && ((cp[1] & 0xC0) == 0x80)) {
    rv = ((c & 0x1F) << 6) | (cp[1] & 0x3F);
    if (rv > 0 && rv < 0x80) {
      rv = 0; // invalid code
    } else {
      *s += 1;
    }
  } else if (((c & 0xF0) == 0xE0) && ((cp[1] & 0xC0) == 0x80) && ((cp[2] & 0xC0) == 0x80)) {
    rv = ((c & 0x0F) << 12) | ((cp[1] & 0x3F) << 6) | (cp[2] & 0x3F);
    if (rv < 0x800) {
      rv = 0; // invalid code
    } else {
      *s += 2;
    }
  } else if (((c & 0xF8) == 0xF0) && ((cp[1] & 0xC0) == 0x80) && ((cp[2] & 0xC0) == 0x80) && ((cp[3] & 0xC0) == 0x80)) {
    // 4-byte Unicode, codepoint above 16-bits are all rejected and skipped
    *s += 3;
  }
  return rv;
}

// Find CP437 character for given Unicode codepoint using binary search
uint8_t unicodeToCP437(uint16_t codepoint) {
  int low = 0, high = CP437_TABLE_SIZE;
  if (codepoint < 0x20) {
    // control characters: do not display
    return 0;
  }
  if (codepoint < 0x80) {
    // Standard ASCII; no lookup needed
    return (uint8_t) codepoint;
  }
  while (low <= high) {
    int mid = (low + high) / 2;
    uint16_t mid_val = pgm_read_word(&cp437_sorted_table[mid].unicode);
    if (mid_val == codepoint)
      return pgm_read_byte(&cp437_sorted_table[mid].cp437);
    else if (mid_val < codepoint)
      low = mid + 1;
    else
      high = mid - 1;
  }
  return CP437_FALLBACK_CHAR; // fallback
}

// Convert a UTF-8 encoded String to CP437 encoded String
String utf8ToCP437(const String &input) {
  String output;
  output.reserve(input.length()); // reserve enough space
  const char *p = input.c_str();
  Serial.printf_P(PSTR("utf8ToCP437 in:  %s\n"), p);
  Serial.flush();
  while (*p) {
    //Serial.printf_P(PSTR("Processing byte: 0x%02X '%c'\n"), (uint8_t)*p, (*p >= 32 && *p <= 126) ? *p : '.');
    //Serial.flush();
    uint16_t codepoint = decodeUTF8toUnicode((char**)&p);
    if (codepoint) {
      uint8_t cp437char = unicodeToCP437(codepoint);
      if (cp437char) {
        output += (char)cp437char;
      }
    }
  }
  return output;
}

// End of utf8tocp437.cpp
