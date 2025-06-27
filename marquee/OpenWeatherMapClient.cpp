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

#include "OpenWeatherMapClient.h"
#include "math.h"


OpenWeatherMapClient::OpenWeatherMapClient(String ApiKey, int CityID, boolean isMetric) {
  myCityID = CityID;
  myApiKey = ApiKey;
  this->isMetric = isMetric;
  cached = false;
}

void OpenWeatherMapClient::updateWeather() {
  WiFiClient weatherClient;
  if (myApiKey == "") {
    errorMsg = "Please provide an API key for weather.";
    Serial.println(errorMsg);
    cached = false;
    return;
  }
  String apiGetData = "GET /data/2.5/weather?id=" + String(myCityID) + "&units=" + ((isMetric)?"metric":"imperial") + "&APPID=" + myApiKey + " HTTP/1.1";

  Serial.println("Getting Weather Data");
  Serial.println(apiGetData);
  cached = true;
  errorMsg = "";
  if (weatherClient.connect(servername, 80)) {  //starts client connection, checks for connection
    weatherClient.println(apiGetData);
    weatherClient.println("Host: " + String(servername));
    weatherClient.println("User-Agent: ArduinoWiFi/1.1");
    weatherClient.println("Connection: close");
    weatherClient.println();
  }
  else {
    Serial.println("connection for weather data failed"); //error message if no client connect
    Serial.println();
    errorMsg = "Connection for weather data failed";
    return;
  }

  Serial.println(F("Waiting for data"));

//TODO: add timeout, we don't want to wait forever
  while(weatherClient.connected() && !weatherClient.available()) delay(1); //waits for data

  // Check HTTP status
  char status[32] = {0};
  weatherClient.readBytesUntil('\r', status, sizeof(status));
  Serial.println("Response Header: " + String(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    cached = false;
    errorMsg = "Weather Data Error: " + String(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!weatherClient.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    cached = false;
    return;
  }

  //todo: move to ArduinoJson v7, should be simple!
  const size_t bufferSize = 2710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(weatherClient);
  if (!root.success()) {
    Serial.println(F("Weather Data Parsing failed!"));
    errorMsg = "Weather Data Parsing failed!";
    cached = false;
    return;
  }

  weatherClient.stop(); //stop client

  if (root.measureLength() <= 150) {
    Serial.println("Error Does not look like we got the data.  Size: " + String(root.measureLength()));
    cached = false;
    errorMsg = (const char*)root["message"];
    Serial.println("Error: " + errorMsg);
    return;
  }


  lat = root["coord"]["lat"];
  lon = root["coord"]["lon"];
  reportTimestamp = root["dt"];
  city = (const char*)root["name"];
  country = (const char*)root["sys"]["country"];
  temperature = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  weatherId = root["weather"][0]["id"];
  weatherCondition = (const char*)root["weather"][0]["main"];
  weatherDescription = (const char*)root["weather"][0]["description"];
  icon = (const char*)root["weather"][0]["icon"];
  pressure = root["main"]["grnd_level"];
  if (pressure == 0) // no local ground level pressure? then get main pressure (at sea level)
    pressure = root["main"]["pressure"];
  windSpeed = root["wind"]["speed"];
  windDirection = root["wind"]["deg"];
  cloudCoverage = root["clouds"]["all"];
  tempHigh = root["main"]["temp_max"];
  tempLow = root["main"]["temp_min"];
  timeZone = root["timezone"];
  sunRise = root["sys"]["sunrise"];
  sunSet = root["sys"]["sunset"];

  if (isMetric) {
    // convert m/s to kmh
    windSpeed *= 3.6;
  } else {
    // Imperial mode
    // windspeed is already in mph
    //convert millibars (hPa) to Inches mercury
    pressure = (int)((float)pressure * 0.0295300586 + 0.5);
    //convert millibars (hPa) to PSI
    //pressure = (int)((float)pressure * 0.0145037738 + 0.5);

  }

#if 1 //DEBUG
  //Serial.print("lat: "); Serial.println(lat);
  //Serial.print("lon: "); Serial.println(lon);
  Serial.print("reportTimestamp: "); Serial.println(reportTimestamp);
  //Serial.print("city: "); Serial.println(city);
  //Serial.print("country: "); Serial.println(country);
  Serial.print("temperature: "); Serial.println(temperature);
  Serial.print("humidity: "); Serial.println(humidity);
  Serial.print("weatherCondition: "); Serial.println(weatherCondition);
  //Serial.print("wind: "); Serial.println(windSpeed);
  Serial.print("windDirection: "); Serial.println(windDirection);
  Serial.print("weatherId: "); Serial.println(weatherId);
  Serial.print("weatherDescription: "); Serial.println(weatherDescription);
  Serial.print("icon: "); Serial.println(icon);
  Serial.print("timezone: "); Serial.println(getTimeZone());
  Serial.println();
#endif
}


String OpenWeatherMapClient::getDirectionText() {
  int val = floor((windDirection / 22.5) + 0.5);
  String arr[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
  return arr[(val % 16)];
}


String OpenWeatherMapClient::getWeekDay() {
  String rtnValue = "";
  long timestamp = reportTimestamp;
  long day = 0;
  static const char* dayarr[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
  };
  if (timestamp != 0) {
    //day = (((timestamp + (3600 * (int)offset)) / 86400) + 4) % 7;
    // Add timezone from OWM
    timestamp += timeZone;
    day = ((timestamp / 86400) + 4) % 7;
    rtnValue = dayarr[day];
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
