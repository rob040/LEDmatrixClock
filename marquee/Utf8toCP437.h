/**
 * @file Utf8toCP437.h
 * @author rob040@users.github.com
 * @brief Convert UTF-8 strings to CP437 for LED matrix display
 * @date 2025-09-20
 * @copyright Copyright (c) 2025
 * Licensed under the MIT license. See LICENSE file in the project root for details.
 */
#pragma once

uint16_t decodeUTF8toUnicode(char **s);
uint8_t unicodeToCP437(uint16_t codepoint);
String utf8ToCP437(const String &input);

// EOF
