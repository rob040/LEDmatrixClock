/**
 * Time NTP - Synchronize time with Network Time Protocol
 *
 * Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *
 */
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time

#define TIME_VALID_MIN  1735686000  // 2025-01-01

void timeNTPsetup();
bool set_timeZoneSec(int timeZoneSeconds);
time_t getNtpTime();
