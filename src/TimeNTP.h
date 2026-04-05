/**
 * @file TimeNTP.h
 * @brief Time NTP - Synchronize time with Network Time Protocol
 *
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * @license This code is licensed under MIT license (see LICENSE.txt for details)
 *
 */
#define NTP_UNIXEPOCH   2208988800UL  // NTP time starts on 1 Jan 1900, Unix time starts on 1 Jan 1970
#define TIME_VALID_MIN  1735686000UL  // 2025-01-01 (@ Unix epoch; this is not a real limit, but we want to detect invalid time values)
#define TIME_VALID_MAX  2081869200UL  // 2036-01-01

void timeNTPsetup();
void set_ntpServerName(const char* serverName);
bool set_timeZoneSec(int timeZoneSeconds);
time_t getNtpTime();
