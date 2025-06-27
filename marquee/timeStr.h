#include "TimeLib.h"
#pragma once
String getDayName(int weekday);
String getMonthName(int month);
String getAmPm(bool isPM);
String get24HrColonMin(uint32_t epoch);
//String get24HrColonMin(uint32_t epoch, int32_t timezoneSecs) { return get24HrColonMin(epoch+timezoneSecs);}

String zeroPad(int number);
String zeroPad(const uint32_t number, const uint8_t length);
