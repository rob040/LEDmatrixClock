/**
 * parts Copyright (c) 2018 David Payne
 * Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *
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

  struct
  {
    // Weather report content
    String city;        // City name
    String country;     // Country code (GB, JP etc.)
    String condition;   // Group of weather parameters (Rain, Snow, Extreme etc.)
    String description; // Weather condition within the group. (language translated)
    String icon;        // Weather icon id
    float lat;          // City geo Location coordinate Latitude
    float lon;          // City geo Location coordinate Longitude
    float temperature;  // Temperature. Unit Default: Kelvin, Metric: Celsius, Imperial: Fahrenheit
    float tempHigh;     // Temperature Highs
    float tempLow;      // Temperature Lows
    float windSpeed;    // Unit Default: meter/sec, Metric: meter/sec, Imperial: miles/hour
    int weatherId;      // Weather condition id
    int pressure;       // Atmospheric pressure at ground level, hPa
    int humidity;       // Humidity in %, 0..100
    int windDirection;  // Wind direction in degrees 0..359
    int cloudCoverage;  // in %, 0..100
    int timeZone;       // TZ Shift in seconds from UTC
    uint32_t reportTimestamp; // Time of Weather data calculation, unix, UTC
    uint32_t sunRise;   // Sunrise time, unix, UTC
    uint32_t sunSet;    // Sunset time, unix, UTC
    bool isValid;       // Weather report is valid
  } weather;

  bool isMetric;   // Weather request format Metric or Imperial
  const int dataGetRetryCountError = 10; // Amount of data get timeouts until we invalidate all data
  int dataGetRetryCount;
  String errorMsg; // Weather request error message


public:
  OpenWeatherMapClient(const String &ApiKey, bool isMetric);
  int setGeoLocation(const String &location);
  void updateWeather();
  inline void setWeatherApiKey(const String &ApiKey) {myApiKey = ApiKey;};
  inline void setMetric(bool isMetric) {this->isMetric = isMetric;};

  inline float getLat() {return weather.lat;};
  inline float getLon() {return weather.lon;};
  inline uint32_t getReportTimestamp() {return weather.reportTimestamp;};
  inline String getCity() {return weather.city;};
  inline String getCountry() {return weather.country;};
  inline float getTemperature() {return weather.temperature;};
  inline int getHumidity() {return weather.humidity;};
  inline String getWeatherCondition() {return weather.condition;};
  inline float getWindSpeed() {return weather.windSpeed;};
  inline int getWindDirection() {return weather.windDirection;};
  inline int getCloudCoverage() {return weather.cloudCoverage;};
  inline int getPressure() {return weather.pressure;};
  inline float getTemperatureHigh() {return weather.tempHigh;};
  inline float getTemperatureLow() {return weather.tempLow;};
  inline int getWeatherId() {return weather.weatherId;};
  inline String getWeatherDescription() {return weather.description;};
  inline String getIcon() {return weather.icon;};
  inline bool getWeatherDataValid() {return weather.isValid;};
  inline String getErrorMessage() {return errorMsg;};
  inline int getTimeZone() {return weather.timeZone/3600;}; // Local TimeZone delta with UTC in Hours
  inline int getTimeZoneSeconds() {return weather.timeZone;};
  inline uint32_t getSunRise() {return weather.sunRise;};
  inline uint32_t getSunSet() {return weather.sunSet;};
  String getWeekDay();
  String getWindDirectionText();
  String getWeatherIcon();
};

extern String EncodeUrlSpecialChars(const char *msg);
