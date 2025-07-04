/*
 * Time Str - Support functions For Arduino String
 *
 * By rob040, Copyleft
 */
#include "TimeLib.h"
#pragma once
String getDayName(int weekday);
String getMonthName(int month);
String getAmPm(bool isPM);
String get24HrColonMin(uint32_t epoch);
String spacePad(unsigned int number);
String zeroPad(unsigned int number);
String zeroPad(uint32_t number, uint8_t length);
