/**
 * @file TimeNTP.cpp
 * @brief Time NTP - Synchronize time with Network Time Protocol
 *
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * @license This code is licensed under MIT license (see LICENSE.txt for details)
 *
 * Based on example code from TimeLib
 *
 * Note that on ESP8266 the time_t type is 64 bits! passing it back and forth,
 * and doing operations on it wil cost extra performance.
 */

#include <Arduino.h>
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "TimeNTP.h"


// NTP Server settings:
static const char ntpServerNameDefault[] PROGMEM = "1.pool.ntp.org";
static char ntpServerName[32]; // buffer to hold NTP server name (max 31 chars + null terminator)
static int timeZoneSec;     // UTC TZ offset in seconds (e.g. 3600 for UTC+1, -18000 for UTC-5)

WiFiUDP Udp;
const unsigned int localPort = 8888;  // local port to listen for UDP packets

static void sendNTPpacket(IPAddress &address);

/* setup the NTP client
 */
void timeNTPsetup()
{
  if (ntpServerName[0] == 0) {
    // if ntpServerName is not set, initialize it with default value
    set_ntpServerName(0);
  }
  Serial.println(F("NTP: Starting UDP"));
  Udp.begin(localPort);
  Serial.print(F("Local port: "));
  Serial.println(Udp.localPort());
  Serial.println(F("waiting for sync"));
  setSyncProvider(getNtpTime);
  setSyncInterval(20);
}

/* set the NTP server name to use for time synchronization
 * serverName: null or empty string to reset to default, otherwise the new server name (max 31 chars)
 * returns pointer to current NTP server name
 */
const char* set_ntpServerName(const char* serverName) {
  memset(ntpServerName, 0, sizeof(ntpServerName)); // clear buffer
  if ((serverName == 0) || serverName[0] == 0) {
    // if serverName is not set, initialize it with default value
    strcpy_P(ntpServerName, ntpServerNameDefault); // copy default server name from PROGMEM
  } else {
    strncpy(ntpServerName, serverName, sizeof(ntpServerName) - 1); // copy serverName to buffer, ensuring length limit and null termination
  }
  Serial.print(F("NTP server set to: "));
  Serial.println(ntpServerName);
  return ntpServerName;
}

/* set the time zone offset in seconds for NTP time synchronization
 * timeZoneSeconds: UTC offset in seconds (e.g. 3600 for UTC+1, -18000 for UTC-5)
 * returns true if the time zone was changed, false if it was the same as before
 */
bool set_timeZoneSec(int timeZoneSeconds)
{
  bool timechange = false;
  if (timeZoneSec != timeZoneSeconds) {
    Serial.print(F("New timezone set: "));
    Serial.println(timeZoneSeconds);
    // we have received new timezone value
    //
    timeZoneSec = timeZoneSeconds;
    setSyncProvider(getNtpTime);
    if (timeStatus() != timeNotSet) {
      setSyncInterval(300);
    }
    timechange = true;
  }
  return timechange;
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

/* Verify an incoming NTP packet for basic validity
 * Returns true if the packet looks like a valid NTP response, false otherwise.
 * Note: this is not a full NTP implementation, just some basic checks to filter out invalid responses.
 */
static bool verifyNtpPacket(const uint8_t *buf, size_t len)
{
  if (len < NTP_PACKET_SIZE) {
    Serial.println(F("NTP: packet too small"));
    return false;
  }
  // LI (2 bits), VN (3 bits), Mode (3 bits)
  uint8_t li   = (buf[0] >> 6) & 0x03;
  uint8_t vn   = (buf[0] >> 3) & 0x07;
  uint8_t mode = buf[0] & 0x07;
  uint8_t stratum = buf[1];

  // Expect a server response (mode 4) and a reasonable version (>=3)
  if (mode != 4) {
    //Serial.print(F("NTP: invalid mode ")); Serial.println(mode);
    return false;
  }
  if (vn < 3) {
    //Serial.print(F("NTP: unsupported version ")); Serial.println(vn);
    return false;
  }
  // LI == 3 indicates unsynchronized / alarm condition
  if (li == 3) {
    //Serial.println(F("NTP: leap indicator = alarm (unsynchronized)"));
    return false;
  }
  // Stratum 0 is a Kiss-o'-Death or unsynchronized server
  if (stratum == 0) {
    //Serial.println(F("NTP: stratum 0 (KOD or unsynchronized)"));
    return false;
  }
  // Ensure transmit timestamp (bytes 40..43) is non-zero
  bool txZero = true;
  for (int i = 40; i <= 43; ++i) {
    if (buf[i] != 0) { txZero = false; break; }
  }
  if (txZero) {
    //Serial.println(F("NTP: transmit timestamp is zero"));
    return false;
  }
  return true;
}

/* Get the current time from the NTP server
 * Returns the current time as a time_t value (seconds since Jan 1, 1970), or 0 if unable to get the time.
 */
time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println(F("Transmit NTP Request"));
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while ((millis() - beginWait) < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println(F("Receive NTP Response"));
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      if (!verifyNtpPacket(packetBuffer, size)) {
        Serial.println(F("NTP: invalid packet"));
        return 0;
      }
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      secsSince1900 -= NTP_UNIXEPOCH; // convert NTP time to Unix time (seconds since Jan 1, 1970)
      secsSince1900 += timeZoneSec;
      Serial.print(F("NTP timezone ")); Serial.println(timeZoneSec);
      Serial.flush();
      // NOTE: implicit conversion from 32-bit to 64-bit return value
      return secsSince1900;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}



