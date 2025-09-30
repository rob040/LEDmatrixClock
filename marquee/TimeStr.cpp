/**
 * @file TimeStr.cpp
 * @author rob040@users.github.com
 * @brief Time Str - Support functions For Arduino String
 * @date 2025-09-20
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 */

#include <Arduino.h>
#include <timeLib.h>
#include "TimeStr.h"
#include "Translations.h"

String findWordInCommaList(const String &list, int index, int maxwords) {
  String retstr = "";
  int begin = 0, end = 0;
  int orgindex = index;
  if ((index < maxwords) && (index >= 0)) {
    // iterate through comma separated list
    while (index >= 0) {
      end = list.indexOf(',', begin);
      if (end == -1) end = list.length();
      if (index-- == 0) break;
      begin = end + 1;
    }
    retstr = list.substring(begin, end);
  }
  Serial.printf_P(PSTR("findWordInCommaList list='%s' index=%d -> '%s'\n"), list.c_str(), orgindex, retstr.c_str());
  return retstr;
}

String getDayName(int weekday) {
  /* ORG:
  static const char daynames[] PROGMEM = "Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday";
  String dayn = FPSTR(daynames);
  String retstr = "";
  int begin = 0, end = 0;
  if ((weekday <= 7) && (weekday > 0)) {
    // iterate through comma separated list
    while (weekday > 0) {
      end = dayn.indexOf(',', begin);
      if (end == -1) end = dayn.length();
      if (--weekday == 0) break;
      begin = end + 1;
    }
    retstr = dayn.substring(begin, end);
  }
  return retstr;*/
  return findWordInCommaList(getTranslationStr(TR_DAYNAMEFULL), weekday-1, 7);
}

String getMonthName(int month) {
  /* ORG:
  static const char monthnames[] PROGMEM = "Jan,Feb,Mar,Apr,May,June,July,Aug,Sep,Oct,Nov,Dec";
  String monn = FPSTR(monthnames);
  String retstr = "";
  int begin = 0, end = 0;
  if ((month <= 12) && (month > 0)) {
    // iterate through comma separated list
    while (month > 0) {
      end = monn.indexOf(',', begin);
      if (end == -1) end = monn.length();
      if (--month == 0) break;
      begin = end + 1;
    }
    retstr = monn.substring(begin, end);
  }
  return retstr;*/
  //return getTranslationStr(TR_MONTHNAMESHORT).substring((month-1)*4, (month-1)*4+3);
  return findWordInCommaList(getTranslationStr(TR_MONTHNAMESHORT), month-1, 12);
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

// END OF FILE
