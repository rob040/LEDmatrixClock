/**
 * @file TimeStr.h
 * @author rob040@users.github.com
 * @brief Time Str - Support functions For Arduino String
 * @date 2025-09-20
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 */
#pragma once
#include "TimeLib.h"
String getDayName(int weekday);
String getMonthName(int month);
String getAmPm(bool isPM);
String get24HrColonMin(uint32_t epoch);
String spacePad(unsigned int number);
String zeroPad(unsigned int number);
String zeroPad(uint32_t number, uint8_t length);
String findWordInCommaList(const String &list, int index, int maxwords);

// EOF
