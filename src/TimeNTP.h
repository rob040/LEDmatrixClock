/**
 * @file TimeNTP.h
 * @brief Time NTP - Synchronize time with Network Time Protocol
 *
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * @license This code is licensed under MIT license (see LICENSE.txt for details)
 *
 */

#define TIME_VALID_MIN  1735686000  // 2025-01-01
#define TIME_VALID_MAX  2081869200  // 2036-01-01

void timeNTPsetup();
bool set_timeZoneSec(int timeZoneSeconds);
time_t getNtpTime();
