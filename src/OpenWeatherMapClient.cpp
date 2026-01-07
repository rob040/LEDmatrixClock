/**
 * @file OpenWeatherMapClient.cpp
 * @brief OpenWeatherMap Client to get current weather data from OWM server
 * @copyright parts Copyright (c) 2018 David Payne
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * @license This code is licensed under MIT license (see LICENSE.txt for details)
 *
 *
 * Change History:
 * 20250628 rob040   Completely changed this OWM client API and OWM access API to use the Free Service
 * 20250712 rob040   Revised data invalidation and data get retry count until data invalid error. New API dataGetRetryCount(), removed API getCached().
 * 20250714 rob040   add more options for geo location; it is no longer centered around CityID, Class Ctor does not need CityID, new method setGeoLocation()
 * Further: See git log
 */

#include "OpenWeatherMapClient.h"
#include "TimeStr.h"
#include "Translations.h"

OpenWeatherMapClient::OpenWeatherMapClient() {
  myGeoLocation = "";
  myGeoLocation_CityID = 0;
  myGeoLocationType = LOC_UNSET;
  myApiKey = "";
  dataGetRetryCount = 0;
  weather.isValid = false;
}

// setGeoLocation
// args  String location
// return int 0 on success, 1 on failure such as invalid location; no check with weather server has been performed.
//
// location string can be:
// A) CityID number (old backwards compatibility),
// B) Longitude,lattitude,
// C) Cityname[[,state],Countrycode]
// The location name must be spelled with ASCII-64, no special chars are allowed. This is
// because the HTML page text encoding is in UTF-8 and internally we use only ASCII with
// LED matrix font using ANSI CP437 extended ASCII, however there is no conversion from
// UTF-8 to ANSI and viceversa)
//
int OpenWeatherMapClient::setGeoLocation(const String &location) {
  int comma = location.indexOf(',');
  int comma2 = location.lastIndexOf(',');
  int decimal = location.indexOf('.');
  int len = location.length();
  int ch_cnt_digits = 0;
  int ch_cnt_letters = 0;
  int ch_cnt_notallowed = 0;
  for (int i=0; i < len; i++){
    char ch = location[i];
    if ((ch >= '0' && ch <= '9') || ch == '.') ch_cnt_digits++;
    else if (ch >= 'A' && ch <= 'Z') ch_cnt_letters++;
    else if (ch >= 'a' && ch <= 'z') ch_cnt_letters++;
    else if (ch == ' ' || ch == '-'|| ch == '('|| ch == ')') ch_cnt_letters++;
    else if (ch != ',') {
      ch_cnt_notallowed++;
      Serial.print(F("Inval 0x"));
      Serial.println(ch,HEX);
    }
  }
  //DEBUG
  #if DEBUG
  Serial.print(F("setGeoLocation: "));
  Serial.println(location);
  #endif

  // Find out what kind of GeoLocation has been passed
  myGeoLocationType = LOC_UNKNOWN;
  myGeoLocation = "";
  myGeoLocation_CityID = 0;
  myGeoLocation_lat = myGeoLocation_lon = 0;
  if ((len > 3) && (comma == -1) && (decimal == -1) && (ch_cnt_digits == len) && (ch_cnt_letters == 0)) {
    myGeoLocation_CityID = location.toInt();
    myGeoLocationType = LOC_CITYID;
    // USE http://api.openweathermap.org/data/2.5/weather?id={city-id}&appid={API-key}
  }
  if ((len > 5) && (comma > 0) && (comma2 == comma) && (ch_cnt_digits >= len-3)) {
    myGeoLocationType = LOC_LATLON;
    myGeoLocation_lat = location.toFloat();
    myGeoLocation_lon = location.substring(comma+1).toFloat();
    // USE http://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={API-key}
  }
  if ((ch_cnt_digits == 0) && (len > 2) && (ch_cnt_letters >= len-2)) {
    myGeoLocationType = LOC_NAME;    // city,state,countrycode OR city,countrycode OR city
    myGeoLocation = location;
    // USE http://api.openweathermap.org/data/2.5/weather?q={city-name}&appid={API-key}
    // USE http://api.openweathermap.org/data/2.5/weather?q={city-name},{country-code}&appid={API-key}
    // USE http://api.openweathermap.org/data/2.5/weather?q={city-name},{state-code},{country-code}&appid={API-key}
    // those will also get the lat,lon
  }
  //if (myGeoLocationType == LOC_UNKNOWN) {
    #if DEBUG
    Serial.printf_P(PSTR("loc=%s,typ=%d,c=%d,c2=%d,len=%d,dec=%d,cd=%d,cl=%d,cn=%d\n"), location.c_str(),myGeoLocationType,comma,comma2,len,decimal,ch_cnt_digits,ch_cnt_letters,ch_cnt_notallowed);
    #endif
  //}
  return (myGeoLocationType <= LOC_UNKNOWN);
}

int OpenWeatherMapClient::setLanguage(const String &languageCode) {
  // languageCode must be a two-letter code such as "en", "fr", "de", ...
  // see https://openweathermap.org/current#multi
  // return 0 on success, 1 on failure such as invalid code
  if (languageCode.length() != 2) return 1;
  myLanguage = languageCode;
  return 0;
}

void OpenWeatherMapClient::updateWeather() {
  WiFiClient weatherClient;
  String apiGetData;
  apiGetData.reserve(260);
  apiGetData += F("GET /data/2.5/weather?");
  if (myApiKey == "") {
    //errorMsg = F("Please provide an API key for weather.");
    errorMsg = getTranslationStr(TR_PLEASESETOWMKEY);
    Serial.println(errorMsg);
    weather.isValid = false;
    return;
  }
  switch (myGeoLocationType) {
  default:
  case LOC_UNSET:
  case LOC_UNKNOWN:
    //errorMsg = F("Please set location for weather.");
    errorMsg = getTranslationStr(TR_PLEASESETLOCATION);
    Serial.println(errorMsg);
    weather.isValid = false;
    return;
  case LOC_CITYID:
    apiGetData += F("id=");
    apiGetData += String(myGeoLocation_CityID);
    break;
  case LOC_LATLON:
    apiGetData += F("lat=");
    apiGetData += String(myGeoLocation_lat);
    apiGetData += F("&lon=");
    apiGetData += String(myGeoLocation_lon);
    break;
  case LOC_NAME:
    apiGetData += F("q=");
    apiGetData += EncodeUrlSpecialChars(myGeoLocation.c_str());
    break;
  }
  apiGetData += F("&units=metric&lang=");
  apiGetData += myLanguage;
  apiGetData += F("&APPID=");
  apiGetData += myApiKey + F(" HTTP/1.1");
  Serial.println(F("Getting Weather Data"));
  Serial.println(apiGetData);
  errorMsg = "";
  if (weatherClient.connect(servername, 80)) {  //starts client connection, checks for connection
    weatherClient.println(apiGetData);
    weatherClient.println(F("Host: ") + String(servername));
    weatherClient.println(F("User-Agent: ArduinoWiFi/1.1"));
    weatherClient.println(F("Connection: close"));
    weatherClient.println();
  }
  else {
    //errorMsg = F("Connection for weather data failed");
    errorMsg = getTranslationStr(TR_CONNECTIONFORWEATHERFAILED);
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) weather.isValid = false;
    return;
  }

  Serial.println(F("Waiting for data"));

  // Wait for data, with timeout
  uint32_t start = millis();
  const int timeout_ms = 5000;
  while (weatherClient.connected() &&
        !weatherClient.available() &&
        ((millis()-start) < timeout_ms))
  {
          delay(1); //waits for data
  }
  if ((millis()-start) >= timeout_ms) {
    //errorMsg = F("TIMEOUT on weatherClient data receive");
    errorMsg = getTranslationStr(TR_TIMEOUTONWEATHERCLIENTDATARECEIVE);
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) weather.isValid = false;
    return;
  }
  //else { //FIXME DEBUG
  //  Serial.print("Wait for data took ");
  //  Serial.println((millis()-start));
  //}

  // Check HTTP status
  char status[32] = {0};
  weatherClient.readBytesUntil('\r', status, sizeof(status));
  Serial.println(F("Response Header: ") + String(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    errorMsg = F("Unexpected response: ") + String(status); // exceptional error, not translated
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) weather.isValid = false;
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!weatherClient.find(endOfHeaders)) {
    errorMsg = F("Invalid response endOfHeaders"); // exceptional error, not translated
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) weather.isValid = false;
    return;
  }

  // Parse JSON object
  JsonDocument jdoc;
  DeserializationError error = deserializeJson(jdoc, weatherClient);
  if (error) {
    errorMsg = F("Weather Data Parsing failed!"); // exceptional error, not translated
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) weather.isValid = false;
    return;
  }

  weatherClient.stop(); //stop client

  // prudancy check: incomplete message ?
  if (int len = measureJson(jdoc) <= 150) {
    Serial.println(F("Error incomplete message, size ") + String(len));
    errorMsg = F("Error: ") + jdoc[F("message")].as<String>(); // exceptional error, not translated
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) weather.isValid = false;
    return;
  }


  weather.lat = jdoc["coord"]["lat"];
  weather.lon = jdoc["coord"]["lon"];
  weather.reportTimestamp = jdoc["dt"];
  weather.city = jdoc["name"].as<String>();
  weather.country = jdoc["sys"]["country"].as<String>();
  weather.temperature = jdoc["main"]["temp"];
  weather.humidity = jdoc["main"]["humidity"];
  weather.weatherId = jdoc["weather"][0]["id"];
  weather.condition = jdoc["weather"][0]["main"].as<String>();
  weather.description = jdoc["weather"][0]["description"].as<String>();
  weather.icon = jdoc["weather"][0]["icon"].as<String>();
  weather.pressure = jdoc["main"]["grnd_level"];
  if (weather.pressure == 0) // no local ground level pressure? then get main pressure (at sea level)
    weather.pressure = jdoc["main"]["pressure"];
  weather.windSpeed = jdoc["wind"]["speed"];
  weather.windDirection = jdoc["wind"]["deg"];
  weather.cloudCoverage = jdoc["clouds"]["all"];
  weather.tempHigh = jdoc["main"]["temp_max"];
  weather.tempLow = jdoc["main"]["temp_min"];
  weather.timeZone = jdoc["timezone"];
  weather.sunRise = jdoc["sys"]["sunrise"];
  weather.sunSet = jdoc["sys"]["sunset"];
  weather.isValid = true;


#if 1 //DEBUG
  Serial.println(F("Weather data:"));
  //Serial.print(F("lat: ")); Serial.println(weather.lat);
  //Serial.print(F("lon: ")); Serial.println(weather.lon);
  //Serial.print(F("reportTimestamp: ")); Serial.println(weather.reportTimestamp);
  //Serial.print(F("city: ")); Serial.println(weather.city);
  //Serial.print(F("country: ")); Serial.println(weather.country);
  Serial.print(F("temperature: ")); Serial.println(weather.temperature);
  Serial.print(F("humidity: ")); Serial.println(weather.humidity);
  Serial.print(F("wind: ")); Serial.println(weather.windSpeed);
  Serial.print(F("windDirection: ")); Serial.println(weather.windDirection);
  //Serial.print(F("weatherId: ")); Serial.println(weather.weatherId);
  Serial.print(F("weatherCondition: ")); Serial.println(weather.condition);
  Serial.print(F("weatherDescription: ")); Serial.println(weather.description);
  //Serial.print(F("icon: ")); Serial.println(weather.icon);
  Serial.print(F("timezone: ")); Serial.println(getTimeZone());
  Serial.println();
#endif
}

String OpenWeatherMapClient::getWeatherIcon() {
  // match weather condition codes to OLED icon in the weatherstation font
  // see https://openweathermap.org/weather-conditions
  // TODO: the weatherstation fonts (Meteocons, http://oleddisplay.squix.ch/) contain far more icons than used here
  // TODO: There is no night/day difference made here
  // TODO: check if translation from weather icon code OLED icon makes more sense.
  static const struct { int16_t nr; char w;} lookuptable[] =
  {
    { 800, 'B'},    { 801, 'Y'},    { 802, 'H'},    { 803, 'H'},    { 804, 'Y'},
    { 200, '0'},    { 201, '0'},    { 202, '0'},    { 210, '0'},    { 211, '0'},
    { 212, '0'},    { 221, '0'},    { 230, '0'},    { 231, '0'},    { 232, '0'},
    { 300, 'R'},    { 301, 'R'},    { 302, 'R'},    { 310, 'R'},    { 311, 'R'},
    { 312, 'R'},    { 313, 'R'},    { 314, 'R'},    { 321, 'R'},
    { 500, 'R'},    { 501, 'R'},    { 502, 'R'},    { 503, 'R'},    { 504, 'R'},
    { 511, 'R'},    { 520, 'R'},    { 521, 'R'},    { 522, 'R'},    { 531, 'R'},
    { 600, 'W'},    { 601, 'W'},    { 602, 'W'},    { 611, 'W'},    { 612, 'W'},
    { 615, 'W'},    { 616, 'W'},    { 620, 'W'},    { 621, 'W'},    { 622, 'W'},
    { 701, 'M'},    { 711, 'M'},    { 721, 'M'},    { 731, 'M'},    { 741, 'M'},
    { 751, 'M'},    { 761, 'M'},    { 762, 'M'},    { 771, 'M'},    { 781, 'M'},
    { 0,   ')'}
  };
  int id = weather.weatherId;
  int i;
  for (i=0; lookuptable[i].nr != 0; i++)
  {
    if (lookuptable[i].nr == id) break;
  }
  return String(lookuptable[i].w);
}

float OpenWeatherMapClient::convTemperature(float temp_celcius, temperatureUnits_t tu) {
  switch (tu) {
  default:
  case TU_CELSIUS:
    return temp_celcius;
  case TU_FAHRENHEIT:
    return (temp_celcius * 9.0 / 5.0) + 32.0;
  case TU_KELVIN:
    return temp_celcius + 273.15;
  }
}

float OpenWeatherMapClient::convAirPressure(int pressure_hpa, airPressureUnits_t apu) {
  switch (apu) {
  default:
  case APU_MBAR:
  case APU_HPA:
    return (float)pressure_hpa;
  case APU_MMHG:
    return (float)pressure_hpa * 0.750061561;
  case APU_INHG:
    return (float)pressure_hpa * 0.0295300586;
  case APU_PSI:
    return (float)pressure_hpa * 0.0145037738;
  case APU_ATM:
    return (float)pressure_hpa / 1013.25;
  }
}

float OpenWeatherMapClient::convWindSpeed(float speed_mps, windSpeedUnits_t wsu) {
  switch (wsu) {
  default:
  case WSU_MPS: // m/s
    return round(speed_mps * 100) / 100; // two decimals
  case WSU_KMH: // km/h
    return round(speed_mps * 3.6f);
  case WSU_MPH: // miles per hour
    return round((speed_mps * 2.23694f) * 100) / 100; // two decimals
  case WSU_KNOTS: // nautical miles per hour
    return round((speed_mps * 1.94384f) * 100) / 100; // two decimals
  case WSU_BFT: // Beaufort scale
    {
      // Beaufort scale
      int speedKmh = round(speed_mps * 3.6f);
      if (speedKmh < 1) return 0;
      else if (speedKmh < 6) return 1;
      else if (speedKmh < 12) return 2;
      else if (speedKmh < 20) return 3;
      else if (speedKmh < 29) return 4;
      else if (speedKmh < 39) return 5;
      else if (speedKmh < 50) return 6;
      else if (speedKmh < 62) return 7;
      else if (speedKmh < 75) return 8;
      else if (speedKmh < 89) return 9;
      else if (speedKmh < 103) return 10;
      else if (speedKmh < 118) return 11;
      else return 12;
    }
  }
}

// End of File
