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
  /*  switch (weekday) {
    case 1:
      return F("Sunday");
      break;
    case 2:
      return F("Monday");
      break;
    case 3:
      return F("Tuesday");
      break;
    case 4:
      return F("Wednesday");
      break;
    case 5:
      return F("Thursday");
      break;
    case 6:
      return F("Friday");
      break;
    case 7:
      return F("Saturday");
      break;
    default:
      return "";
  }*/

}

String getMonthName(int month) {
  String rtnValue = "";
  switch (month) {
    case 1:
      rtnValue = F("Jan");
      break;
    case 2:
      rtnValue = F("Feb");
      break;
    case 3:
      rtnValue = F("Mar");
      break;
    case 4:
      rtnValue = F("Apr");
      break;
    case 5:
      rtnValue = F("May");
      break;
    case 6:
      rtnValue = F("June");
      break;
    case 7:
      rtnValue = F("July");
      break;
    case 8:
      rtnValue = F("Aug");
      break;
    case 9:
      rtnValue = F("Sep");
      break;
    case 10:
      rtnValue = F("Oct");
      break;
    case 11:
      rtnValue = F("Nov");
      break;
    case 12:
      rtnValue = F("Dec");
      break;
    default:
      rtnValue = "";
  }
  return rtnValue;
}

String get24HrColonMin(uint32_t epoch)
{
  int hr = numberOfHours(epoch);
  int min = numberOfMinutes(epoch);
  return zeroPad(hr)+":"+zeroPad(min);
}


String getAmPm(bool isPM) {
  String ampmValue = "AM";
  if (isPM) {
    ampmValue = "PM";
  }
  return ampmValue;
}

String zeroPad(int number) {
  if (number < 10) {
    return "0" + String(number);
  } else {
    return String(number);
  }
}

String zeropad(const uint32_t number, const uint8_t length) {
    String str;
    str.reserve(length);
    str = String(number);
    while (str.length() < length) str = "0" + str;
    return str;
}

