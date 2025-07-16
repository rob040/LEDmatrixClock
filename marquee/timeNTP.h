/*
 * Time NTP - Synchronize time with Network Time Protocol
 */
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time

#define TIME_VALID_MIN  1735686000  // 2025-01-01

void timeNTPsetup();
boolean set_timeZoneSec(int timeZoneSeconds);
time_t getNtpTime();
