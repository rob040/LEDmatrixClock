/*
 * Time Str - Support functions For Arduino String
 *
 * Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *
 */
#include <Arduino.h>
#include "timeLib.h"
#include "timeStr.h"

String getDayName(int weekday) {
  static const char daynames[] PROGMEM = "Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,";
  String retstr = "";
  String dayn = FPSTR(daynames);
  int begin = 0, end = 0;
  if ((weekday <= 7) && (weekday > 0)) {
    // iterate through comma separated list
    while (weekday > 0) {
      end = dayn.indexOf(',', begin);
      if (--weekday == 0) break;
      begin = end + 1;
    }
    retstr = dayn.substring(begin, end);
  }
  return retstr;
}

String getMonthName(int month) {
  static const char monthnames[] PROGMEM = "Jan,Feb,Mar,Apr,May,June,July,Aug,Sep,Oct,Nov,Dec,";
  String retstr = "";
  String monn = FPSTR(monthnames);
  int begin = 0, end = 0;
  if ((month <= 12) && (month > 0)) {
    // iterate through comma separated list
    while (month > 0) {
      end = monn.indexOf(',', begin);
      if (--month == 0) break;
      begin = end + 1;
    }
    retstr = monn.substring(begin, end);
  }
  return retstr;
}

String get24HrColonMin(uint32_t epoch)
{
  int hr = numberOfHours(epoch);
  int min = numberOfMinutes(epoch);
  return zeroPad(hr)+":"+zeroPad(min);
}

String getAmPm(bool isPM) {
  return (isPM) ? "PM" : "AM";
}

String spacePad(unsigned int number) {
  if (number < 10) {
    return " " + String(number);
  } else {
    return String(number);
  }
}

String zeroPad(unsigned int number) {
  if (number < 10) {
    return "0" + String(number);
  } else {
    return String(number);
  }
}

String zeroPad(uint32_t number, uint8_t length) {
    String str;
    str.reserve(length);
    str = String(number);
    while (str.length() < length) str = "0" + str;
    return str;
}

