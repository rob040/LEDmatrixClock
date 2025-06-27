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


//DEPRICATED: with current OpenWeatherMap, it is NOT possible to provide more than 1 CityID!
OpenWeatherMapClient::OpenWeatherMapClient(String ApiKey, int CityIDs[], int cityCount, boolean isMetric) {
  updateCityIdList(CityIDs, cityCount);
  myApiKey = ApiKey;
  setMetric(isMetric);
}

OpenWeatherMapClient::OpenWeatherMapClient(String ApiKey, int CityID, boolean isMetric) {
  setCityId(CityID);
  myApiKey = ApiKey;
  setMetric(isMetric);
}

void OpenWeatherMapClient::setWeatherApiKey(String ApiKey) {
  myApiKey = ApiKey;
}

void OpenWeatherMapClient::updateWeather() {
  WiFiClient weatherClient;
  if (myApiKey == "") {
    weathers[0].error = "Please provide an API key for weather.";
    Serial.println(weathers[0].error);
    return;
  }
  String apiGetData = "GET /data/2.5/weather?id=" + myCityIDs + "&units=" + units + "&APPID=" + myApiKey + " HTTP/1.1";

  Serial.println("Getting Weather Data");
  Serial.println(apiGetData);
  weathers[0].cached = false;
  weathers[0].error = "";
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
    weathers[0].error = "Connection for weather data failed";
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
    weathers[0].error = "Weather Data Error: " + String(status);
    return;
  }

    // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!weatherClient.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  //todo: move to ArduinoJson v7, should be simple!
  const size_t bufferSize = 2710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(weatherClient);
  if (!root.success()) {
    Serial.println(F("Weather Data Parsing failed!"));
    weathers[0].error = "Weather Data Parsing failed!";
    return;
  }

  weatherClient.stop(); //stop client

  if (root.measureLength() <= 150) {
    Serial.println("Error Does not look like we got the data.  Size: " + String(root.measureLength()));
    weathers[0].cached = true;
    weathers[0].error = (const char*)root["message"];
    Serial.println("Error: " + weathers[0].error);
    return;
  }


    weathers[0].lat = (const char*)root["coord"]["lat"];
    weathers[0].lon = (const char*)root["coord"]["lon"];
    weathers[0].dt = (const char*)root["dt"];
    weathers[0].city = (const char*)root["name"];
    weathers[0].country = (const char*)root["sys"]["country"];
    weathers[0].temp = (const char*)root["main"]["temp"];
    weathers[0].humidity = (const char*)root["main"]["humidity"];
    weathers[0].condition = (const char*)root["weather"][0]["main"];
    weathers[0].wind = (const char*)root["wind"]["speed"];
    weathers[0].weatherId = (const char*)root["weather"][0]["id"];
    weathers[0].description = (const char*)root["weather"][0]["description"];
    weathers[0].icon = (const char*)root["weather"][0]["icon"];
    weathers[0].pressure = (const char*)root["main"]["pressure"];
    weathers[0].direction = (const char*)root["wind"]["deg"];
    weathers[0].high = (const char*)root["main"]["temp_max"];
    weathers[0].low = (const char*)root["main"]["temp_min"];
    weathers[0].timeZone = (const char*)root["timezone"];
    weathers[0].sunRise = root["sys"]["sunrise"];
    weathers[0].sunSet = root["sys"]["sunset"];

    if (units == "metric") {
      // convert to kmh from m/s
      float f = (weathers[0].wind.toFloat() * 3.6);
      weathers[0].wind = String(f);
    }
    else
    {
      float p = (weathers[0].pressure.toFloat() * 0.0295301); //convert millibars to inches (PSI)
      weathers[0].pressure = String(p);
    }

    Serial.println("lat: " + weathers[0].lat);
    Serial.println("lon: " + weathers[0].lon);
    Serial.println("dt: " + weathers[0].dt);
    Serial.println("city: " + weathers[0].city);
    Serial.println("country: " + weathers[0].country);
    Serial.println("temp: " + weathers[0].temp);
    Serial.println("humidity: " + weathers[0].humidity);
    Serial.println("condition: " + weathers[0].condition);
    Serial.println("wind: " + weathers[0].wind);
    Serial.println("direction: " + weathers[0].direction);
    Serial.println("weatherId: " + weathers[0].weatherId);
    Serial.println("description: " + weathers[0].description);
    Serial.println("icon: " + weathers[0].icon);
    Serial.println("timezone: " + String(getTimeZone(0)));
    Serial.println();

}

String OpenWeatherMapClient::roundValue(String value) {
  float f = value.toFloat();
  int rounded = (int)(f+0.5f);
  return String(rounded);
}

//DEPRECATED:
void OpenWeatherMapClient::updateCityIdList(int CityIDs[], int cityCount) {
  myCityIDs = "";
  for (int inx = 0; inx < cityCount; inx++) {
    if (CityIDs[0] > 0) {
      if (myCityIDs != "") {
        myCityIDs = myCityIDs + ",";
      }
      myCityIDs = myCityIDs + String(CityIDs[0]);
    }
  }
}

void OpenWeatherMapClient::setCityId(int CityID) {
  myCityIDs = String(CityID);
}


void OpenWeatherMapClient::setMetric(boolean isMetric) {
  if (isMetric) {
    units = "metric";
  } else {
    units = "imperial";
  }
}

String OpenWeatherMapClient::getLat(int index) {
  return weathers[index].lat;
}

String OpenWeatherMapClient::getLon(int index) {
  return weathers[index].lon;
}

String OpenWeatherMapClient::getDt(int index) {
  return weathers[index].dt;
}

String OpenWeatherMapClient::getCity(int index) {
  return weathers[index].city;
}

String OpenWeatherMapClient::getCountry(int index) {
  return weathers[index].country;
}

String OpenWeatherMapClient::getTemp(int index) {
  return weathers[index].temp;
}

String OpenWeatherMapClient::getTempRounded(int index) {
  return roundValue(getTemp(index));
}

String OpenWeatherMapClient::getHumidity(int index) {
  return weathers[index].humidity;
}

String OpenWeatherMapClient::getHumidityRounded(int index) {
  return roundValue(getHumidity(index));
}

String OpenWeatherMapClient::getCondition(int index) {
  return weathers[index].condition;
}

String OpenWeatherMapClient::getWind(int index) {
  return weathers[index].wind;
}

String OpenWeatherMapClient::getDirection(int index)
{
  return weathers[index].direction;
}

String OpenWeatherMapClient::getDirectionRounded(int index)
{
  return roundValue(getDirection(index));
}

String OpenWeatherMapClient::getDirectionText(int index) {
  int num = getDirectionRounded(index).toInt();
  int val = floor((num / 22.5) + 0.5);
  String arr[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
  return arr[(val % 16)];
}

String OpenWeatherMapClient::getWindRounded(int index) {
  return roundValue(getWind(index));
}

String OpenWeatherMapClient::getWeatherId(int index) {
  return weathers[index].weatherId;
}

String OpenWeatherMapClient::getDescription(int index) {
  return weathers[index].description;
}

String OpenWeatherMapClient::getPressure(int index)
{
  return weathers[index].pressure;
}

String OpenWeatherMapClient::getHigh(int index)
{
  return roundValue(weathers[index].high);
}

String OpenWeatherMapClient::getLow(int index)
{
  return roundValue(weathers[index].low);
}

String OpenWeatherMapClient::getIcon(int index) {
  return weathers[index].icon;
}

boolean OpenWeatherMapClient::getCached() {
  return weathers[0].cached;
}

String OpenWeatherMapClient::getMyCityIDs() {
  return myCityIDs;
}

String OpenWeatherMapClient::getError() {
  return weathers[0].error;
}

// DEPRICATED
String OpenWeatherMapClient::getWeekDay(int index, float offset) {
  String rtnValue = "";
  long epoc = weathers[index].dt.toInt();
  long day = 0;
  if (epoc != 0) {
    day = (((epoc + (3600 * (int)offset)) / 86400) + 4) % 7;
    switch (day) {
      case 0:
        rtnValue = "Sunday";
        break;
      case 1:
        rtnValue = "Monday";
        break;
      case 2:
        rtnValue = "Tuesday";
        break;
      case 3:
        rtnValue = "Wednesday";
        break;
      case 4:
        rtnValue = "Thursday";
        break;
      case 5:
        rtnValue = "Friday";
        break;
      case 6:
        rtnValue = "Saturday";
        break;
      default:
        break;
    }
  }
  return rtnValue;
}

String OpenWeatherMapClient::getWeekDay() {
  String rtnValue = "";
  long epoc = weathers[0].dt.toInt();
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
  if (epoc != 0) {
    // do not use offset arg
    //(void)offset;
    //day = (((epoc + (3600 * (int)offset)) / 86400) + 4) % 7;
    // Add timezone from OWM
    epoc += weathers[0].timeZone.toInt();
    day = ((epoc / 86400) + 4) % 7;
    rtnValue = dayarr[day];
  }
  return rtnValue;
}

int OpenWeatherMapClient::getTimeZone(int index) {
  int rtnValue = weathers[index].timeZone.toInt();
  if (rtnValue != 0) {
    rtnValue = rtnValue / 3600;
  }
  return rtnValue;
}

String OpenWeatherMapClient::getWeatherIcon(int index) {
  int id = getWeatherId(index).toInt();
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
