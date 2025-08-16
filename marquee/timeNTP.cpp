/**
 * Time NTP - Synchronize time with Network Time Protocol
 *
 * Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *
 * Based on example code from TimeLib
 *
 * Note that on ESP8266 the time_t type is 64 bits! passing it back and forth,
 * and doing operations on it wil cost extra performance.
 */

#include <Arduino.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


// NTP Servers:
static const char ntpServerName[] = "1.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

int timeZoneSec = 0;     // UTC
//int timeZoneSec = 1*SECS_PER_HOUR;     // Central European Time
//const int timeZoneSec = -5*SECS_PER_HOUR;  // Eastern Standard Time (USA)
//const int timeZoneSec = -4*SECS_PER_HOUR;  // Eastern Daylight Time (USA)
//const int timeZoneSec = -8*SECS_PER_HOUR;  // Pacific Standard Time (USA)
//const int timeZoneSec = -7*SECS_PER_HOUR;  // Pacific Daylight Time (USA)

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

void timeNTPsetup()
{
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(20);
}

bool set_timeZoneSec(int timeZoneSeconds)
{
  bool timechange = false;
  if (timeZoneSec != timeZoneSeconds) {
    Serial.print("New timezone set: ");Serial.println(timeZoneSeconds);
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
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println(F("Receive NTP Response"));
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      secsSince1900 -= 2208988800UL;
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



