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

Change History:
20250628 rob040   Completely changed this OWM client API and OWM access API to use the Free Service
20250712 rob040   Revised data invalidation and data get retry count until data invalid error. New API dataGetRetryCount(), removed API getCached().
*/

#include "OpenWeatherMapClient.h"
#include "timeStr.h"

//OpenWeatherMapClient::OpenWeatherMapClient(const String &ApiKey, int CityID, boolean isMetric) {
//  myGeoLocation_CityID = CityID;
//  myGeoLocation = "";
//  myGeoLocationType = LOC_CITYID;
//  myApiKey = ApiKey;
//  this->isMetric = isMetric;
//  isValid = false;
//}
OpenWeatherMapClient::OpenWeatherMapClient(const String &ApiKey, boolean isMetric) {
  myGeoLocation = "";
  myGeoLocation_CityID = 0;
  myGeoLocationType = LOC_UNSET;
  myApiKey = ApiKey;
  this->isMetric = isMetric;
  isValid = false;
}

// setGeoLocation
// args  String location
// return int 0 on success, 1 on failure such as invalid location; no check with weather server has been performed.
//
// location string can be: A) CityID number (old backwards compatibility), B) Longitude,lattitude,
// C) Cityname[,state],Countrycode
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
      //Serial.print("Inval 0x");
      //Serial.println(ch,HEX);
    }
  }

  myGeoLocationType = LOC_UNKNOWN;
  myGeoLocation = "";
  myGeoLocation_CityID = 0;
  myGeoLocation_lat = myGeoLocation_lon = 0;
  if ((len > 3) && (comma == -1) && (decimal == -1) && (ch_cnt_digits == len) && (ch_cnt_letters == 0)) {
    myGeoLocation_CityID = location.toInt();
    myGeoLocationType = LOC_CITYID;
    // USE http://api.openweathermap.org/data/2.5/weather?id={city-id}&appid={API-key}
  }
  if ((len > 5) && (comma > 0) && (comma2 == 0) && (ch_cnt_digits >= len-3)) {
    myGeoLocationType = LOC_LATLON;
    myGeoLocation_lat = location.toFloat();
    myGeoLocation_lon = location.substring(comma+1).toFloat();
    // USE http://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={API-key}
  }
  if ((ch_cnt_digits == 0) && (ch_cnt_letters >= len-2)) {
    myGeoLocationType = LOC_NAME;    // city,state,countrycode OR city,countrycode OR city
    myGeoLocation = location;
    // USE http://api.openweathermap.org/geo/1.0/direct?q={city-name},{state-code},{country-code}&limit={limit}&appid={API-key}
    // to convert location to lat,lon
    // OR
    // USE http://api.openweathermap.org/data/2.5/weather?q={city-name}&appid={API-key}
    // USE http://api.openweathermap.org/data/2.5/weather?q={city-name},{country-code}&appid={API-key}
    // USE http://api.openweathermap.org/data/2.5/weather?q={city-name},{state-code},{country-code}&appid={API-key}
    // those will also get the lat,lon
  }
  return (myGeoLocationType <= LOC_UNKNOWN);
}

void OpenWeatherMapClient::updateWeather() {
  WiFiClient weatherClient;
  String apiGetData;
  apiGetData.reserve(260);
  apiGetData += F("GET /data/2.5/weather?");
  if (myApiKey == "") {
    errorMsg = F("Please provide an API key for weather.");
    Serial.println(errorMsg);
    isValid = false;
    return;
  }
  switch (myGeoLocationType) {
  default:
  case LOC_UNSET:
  case LOC_UNKNOWN:
    errorMsg = F("Please set location for weather.");
    Serial.println(errorMsg);
    isValid = false;
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

  apiGetData += F("&units=") + String((isMetric) ? F("metric") : F("imperial")) + F("&APPID=") + myApiKey + F(" HTTP/1.1");
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
    errorMsg = F("Connection for weather data failed");
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) isValid = false;
    return;
  }

  Serial.println(F("Waiting for data"));

  // Wait for data, with timeout
  uint32_t start = millis();
  const int timeout_ms = 2000;
  while (weatherClient.connected() &&
        !weatherClient.available() &&
        ((millis()-start) < timeout_ms))
  {
          delay(1); //waits for data
  }
  if ((millis()-start) >= timeout_ms) {
    errorMsg = F("TIMEOUT on weatherClient data receive");
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) isValid = false;
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
    errorMsg = F("Unexpected response: ") + String(status);
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) isValid = false;
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!weatherClient.find(endOfHeaders)) {
    errorMsg = F("Invalid response endOfHeaders");
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) isValid = false;
    return;
  }

  // Parse JSON object
  JsonDocument jdoc;
  DeserializationError error = deserializeJson(jdoc, weatherClient);
  if (error) {
    errorMsg = F("Weather Data Parsing failed!");
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) isValid = false;
    return;
  }

  weatherClient.stop(); //stop client

  // prudancy check: incomplete message ?
  if (int len = measureJson(jdoc) <= 150) {
    Serial.println(F("Error incomplete message, size ") + String(len));
    errorMsg = F("Error: ") + jdoc[F("message")].as<String>();
    Serial.println(errorMsg);
    if (++dataGetRetryCount > dataGetRetryCountError) isValid = false;
    return;
  }


  lat = jdoc["coord"]["lat"];
  lon = jdoc["coord"]["lon"];
  reportTimestamp = jdoc["dt"];
  city = jdoc["name"].as<String>();
  country = jdoc["sys"]["country"].as<String>();
  temperature = jdoc["main"]["temp"];
  humidity = jdoc["main"]["humidity"];
  weatherId = jdoc["weather"][0]["id"];
  weatherCondition = jdoc["weather"][0]["main"].as<String>();
  weatherDescription = jdoc["weather"][0]["description"].as<String>();
  icon = jdoc["weather"][0]["icon"].as<String>();
  pressure = jdoc["main"]["grnd_level"];
  if (pressure == 0) // no local ground level pressure? then get main pressure (at sea level)
    pressure = jdoc["main"]["pressure"];
  windSpeed = jdoc["wind"]["speed"];
  windDirection = jdoc["wind"]["deg"];
  cloudCoverage = jdoc["clouds"]["all"];
  tempHigh = jdoc["main"]["temp_max"];
  tempLow = jdoc["main"]["temp_min"];
  timeZone = jdoc["timezone"];
  sunRise = jdoc["sys"]["sunrise"];
  sunSet = jdoc["sys"]["sunset"];
  isValid = true;

  if (isMetric) {
    // convert m/s to kmh
    windSpeed *= 3.6;
  } else {
    // Imperial mode
    // windspeed is already in mph
    //convert millibars (hPa) to Inches mercury (inHg)
    pressure = (int)((float)pressure * 0.0295300586 + 0.5);
    //convert millibars (hPa) to PSI
    //pressure = (int)((float)pressure * 0.0145037738 + 0.5);
  }

#if 1 //DEBUG
  Serial.println(F("Weather data:"));
  //Serial.print(F("lat: ")); Serial.println(lat);
  //Serial.print(F("lon: ")); Serial.println(lon);
  Serial.print(F("reportTimestamp: ")); Serial.println(reportTimestamp);
  //Serial.print(F("city: ")); Serial.println(city);
  //Serial.print(F("country: ")); Serial.println(country);
  Serial.print(F("temperature: ")); Serial.println(temperature);
  Serial.print(F("humidity: ")); Serial.println(humidity);
  Serial.print(F("wind: ")); Serial.println(windSpeed);
  Serial.print(F("windDirection: ")); Serial.println(windDirection);
  //Serial.print(F("weatherId: ")); Serial.println(weatherId);
  Serial.print(F("weatherCondition: ")); Serial.println(weatherCondition);
  Serial.print(F("weatherDescription: ")); Serial.println(weatherDescription);
  //Serial.print(F("icon: ")); Serial.println(icon);
  Serial.print(F("timezone: ")); Serial.println(getTimeZone());
  Serial.println();
#endif
}


String OpenWeatherMapClient::getWindDirectionText() {
  int val = floor((windDirection / 22.5) + 0.5);
  String arr[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
  return arr[(val % 16)];
}


String OpenWeatherMapClient::getWeekDay() {
  String rtnValue = "";
  long timestamp = reportTimestamp;
  long day = 0;
  /*static const char* dayarr[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
  };*/
  if (timestamp != 0) {
    //day = (((timestamp + (3600 * (int)offset)) / 86400) + 4) % 7;
    // Add timezone from OWM
    timestamp += timeZone;
    day = ((timestamp / 86400) + 4) % 7;
    rtnValue = getDayName(day);
  }
  return rtnValue;
}

String OpenWeatherMapClient::getWeatherIcon() {
  int id = weatherId;
  String W = ")";
  switch(id)
  {
    case 800: W = "B"; break;
    case 801: W = "Y"; break;
    case 802: W = "H"; break;
    case 803: W = "H"; break;
    case 804: W = "Y"; break;

    case 200: W = "0"; break;
    case 201: W = "0"; break;
    case 202: W = "0"; break;
    case 210: W = "0"; break;
    case 211: W = "0"; break;
    case 212: W = "0"; break;
    case 221: W = "0"; break;
    case 230: W = "0"; break;
    case 231: W = "0"; break;
    case 232: W = "0"; break;

    case 300: W = "R"; break;
    case 301: W = "R"; break;
    case 302: W = "R"; break;
    case 310: W = "R"; break;
    case 311: W = "R"; break;
    case 312: W = "R"; break;
    case 313: W = "R"; break;
    case 314: W = "R"; break;
    case 321: W = "R"; break;

    case 500: W = "R"; break;
    case 501: W = "R"; break;
    case 502: W = "R"; break;
    case 503: W = "R"; break;
    case 504: W = "R"; break;
    case 511: W = "R"; break;
    case 520: W = "R"; break;
    case 521: W = "R"; break;
    case 522: W = "R"; break;
    case 531: W = "R"; break;

    case 600: W = "W"; break;
    case 601: W = "W"; break;
    case 602: W = "W"; break;
    case 611: W = "W"; break;
    case 612: W = "W"; break;
    case 615: W = "W"; break;
    case 616: W = "W"; break;
    case 620: W = "W"; break;
    case 621: W = "W"; break;
    case 622: W = "W"; break;

    case 701: W = "M"; break;
    case 711: W = "M"; break;
    case 721: W = "M"; break;
    case 731: W = "M"; break;
    case 741: W = "M"; break;
    case 751: W = "M"; break;
    case 761: W = "M"; break;
    case 762: W = "M"; break;
    case 771: W = "M"; break;
    case 781: W = "M"; break;

    default:break;
  }
  return W;
}
