/** The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

class OpenWeatherMapClient {

private:
  String myApiKey;

  const char* servername = "api.openweathermap.org";  // remote server we will connect to

  String myGeoLocation;
  enum locationtype_e {
    LOC_UNSET,
    LOC_UNKNOWN,
    LOC_CITYID,
    LOC_LATLON,
    LOC_NAME
  } myGeoLocationType;
  int myGeoLocation_CityID;
  float myGeoLocation_lat;
  float myGeoLocation_lon;

  String city;
  String country;
  String weatherCondition;
  String weatherDescription;
  String icon;
  String errorMsg;
  float lat;   // Location coordinate Latitude
  float lon;   // Location coordinate Longitude
  float temperature; // Temperature. Unit Default: Kelvin, Metric: Celsius, Imperial: Fahrenheit
  float tempHigh;
  float tempLow;
  float windSpeed;  // Unit Default: meter/sec, Metric: meter/sec, Imperial: miles/hour
  int weatherId;
  int pressure; // Atmospheric pressure on the ground level, hPa
  int humidity;  // in %, 0..100
  int windDirection; // degrees 0..359
  int cloudCoverage;  // in %, 0..100
  int timeZone; // +/- delta seconds to UTC
  uint32_t reportTimestamp;
  uint32_t sunRise;
  uint32_t sunSet;
  boolean isValid;
  boolean isMetric;
  const int dataGetRetryCountError = 10; // Amount of data get timeouts until we invalidate all data
  int dataGetRetryCount;


public:
  //OpenWeatherMapClient(const String &ApiKey, int CityID, boolean isMetric); // depricated
  OpenWeatherMapClient(const String &ApiKey, boolean isMetric);
  int setGeoLocation(const String &location);
  void updateWeather();
  inline void setWeatherApiKey(const String &ApiKey) {myApiKey = ApiKey;};
  //inline void setCityId(int CityID) {myCityID = CityID;}; // depricated; use setGeoLocation() with CityID instead
  inline void setMetric(boolean isMetric) {this->isMetric = isMetric;};

  //Rounding no longer needed; when converting float to String, use second argument to set decimal places
  //static inline int roundFloatToInt(float f) { return (int)(f+0.5);}

  inline float getLat() {return lat;};
  inline float getLon() {return lon;};
  inline uint32_t getReportTimestamp() {return reportTimestamp;};
  inline String getCity() {return city;};
  inline String getCountry() {return country;};
  inline float getTemperature() {return temperature;};
  //inline int getTemperatureRounded() {return roundFloatToInt(temperature);};
  inline int getHumidity() {return humidity;};
  inline String getWeatherCondition() {return weatherCondition;};
  inline float getWindSpeed() {return windSpeed;};
  //inline int getWindSpeedRounded() {return roundFloatToInt(windSpeed);};
  inline int getWindDirection() {return windDirection;};
  inline int getCloudCoverage() {return cloudCoverage;};
  inline int getPressure() {return pressure;};
  inline float getTemperatureHigh() {return tempHigh;};
  inline float getTemperatureLow() {return tempLow;};
  inline int getWeatherId() {return weatherId;};
  inline String getWeatherDescription() {return weatherDescription;};
  inline String getIcon() {return icon;};
  inline boolean getWeatherDataValid() {return isValid;};
  //inline int getMyCityID() {return myCityID;};
  inline String getErrorMessage() {return errorMsg;};
  inline int getTimeZone() {return timeZone/3600;}; // Local TimeZone delta with UTC in Hours
  inline int getTimeZoneSeconds() {return timeZone;};
  inline uint32_t getSunRise() {return sunRise;};
  inline uint32_t getSunSet() {return sunSet;};
  String getWeekDay();
  String getWindDirectionText();
  String getWeatherIcon();
};

extern String EncodeUrlSpecialChars(const char *msg);
