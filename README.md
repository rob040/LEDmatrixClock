# LED Matrix Clock a.k.a. Marquee Scroller (Clock, Weather, News, and More)

## NOTICE
This fork enhances the Qrome Marquee Scroller version 3.03 from february 2023, that was developed during 2018-2022 period.

For more information and development history visit [Qrome Marquee Scroller](https://github.com/Qrome/marquee-scroller) and read his [README](https://github.com/Qrome/marquee-scroller/blob/master/README.md).

Read the [feature enhancements below](#feature-enhancements)


## Features include:
* Accurate Clock refresh from Internet Time Servers
* Display Local Weather and conditions (refreshed every 10 - 30 minutes, configurable)
* Display News Headlines from all the major sources
* Display 3D print progress from your OctoPrint Server
* Display Pi-hole status and graph (each pixel accross is 10 minutes)
* Configured through Web Interface
* Basic Authorization to access Configuration web interface
* Update firmware through web interface over WiFi (OTA)
* Configurable scroll speed and LED brightness
* Configurable data display and data update interval frequency
* Configurable sleep / wake times
* Configurable number of (compiletime)
* Options of different LED-matrix panels (4 to >8) and
  types of Clock Displays on larger panels, e.g. also display seconds or temperature
* Video: https://youtu.be/DsThufRpoiQ
* Build Video by Chris Riley: https://youtu.be/KqBiqJT9_lE

## Feature Enhancements
Enhancements included in [THIS repository](https://github.com/rob040/LEDmatrixClock) :
* Removed the TimeZoneDB.com registration requirement.
* Added Time NTP and more efficient time strings instead. Actual time zone information is used from OpenWeather API.
* Update to new OpenWeatherMap.org API. Newly requested Free Service API-keys can no longer use the older call and structure.
* Reduce RAM usage. The ESP8266 is an older wifi processor with limited RAM (80kB)
 especilly when compared to its newer ESP32 members.
* Added basic MQTT support with basic Authentication.
* Use VScode IDE with PlatformIO / PIOarduino, for better development and maintenace experience and much better build environment and library version control.
* Improved start-up on time synchronisation and weather data update,
* Improved weather data display on webpage (minor changes)
* Automatic timezone (from Local wather), hence no need for TimeZoneDB.
* Added a favicon for easy recogninition in your browser.
* Support for different display sizes without re-compilation, via configuration page (device will reboot upon change).
  

### known issues
* web-page updates halts scrolling display for a moment
* optional NEWS, Pi-Hole, OctoPi remain untested and may not work properly.


## Required Parts:
* Wemos D1 Mini ESP8266: https://amzn.to/3tMl81U
* LED Dot Matrix Module: https://amzn.to/2HtnQlD

Note: Using the links provided here help to support [Qrome](https://github.com/Qrome) for his efforts. Thank you for the support.

## Wiring for the Wemos D1 Mini to the LED Dot Matrix Display

| LED | D1 mini |
| --- | --- |
| VCC | 5V+       |
| GND | GND-      |
| CLK | D5 (SCK)  |
| CS  | D6        |
| DIN | D7 (MOSI) |

The Connections in a picture:
<p align="left" title="Wires from D1-mini to LED display">
  <img src="/images/marquee_scroller_pins.png" width="400"/>
</p>

## 3D Printed Case by David Payne:
Original Single Panel version: https://www.thingiverse.com/thing:2867294 <br>
Double Wide LED version: https://www.thingiverse.com/thing:2989552

<p align="left" title="STL models from Thingiverse">
  <img src="/images/Thingiverse-stl-base.png" width="400"/>
  <img src="/images/Thingiverse-stl-back.png" width="400"/>
</p>

<p align="left" title="Marquee Scroller parts">
  <img src="/images/marquee-allparts.jpg" width="270"/>
  <img src="/images/marquee-backplate.jpg" width="270"/>
  <img src="/images/marquee-lit.jpg" width="260"/>
</p>
<p align="left" title="Makes from Thingiverse">
  <img src="/images/20171105_124635.jpg" width="200"/>
  <img src="/images/20171017_075241.jpg" width="200"/>
  <img src="/images/20171105_125913.jpg" width="200"/>
  <img src="/images/20171022_214044.jpg" width="200"/>
</p>
<p align="left" title="Makes from Thingiverse">
  <img src="/images/20180128_091534.jpg" width="200"/>
  <img src="/images/20180128_091524.jpg" width="200"/>
  <img src="/images/20180128_091552.jpg" width="200"/>
  <img src="/images/20180127_135828.jpg" width="200"/>
</p>



## Compiling and Loading to Wemos D1 Mini (ESP8266)
### Using Arduino 2.x IDE
It is NOT recommended to use Arduino IDE. It might be difficult to get the right library versions together, especially when using it also for other Arduino projects.<br>
Still, the source directory structure is kept such that this project can be build with little to no effort. This might change in the future.
* Support for ESP8266 Boards is included in Arduino v2.x
* Select Board:  "ESP8266" --> "LOLIN(WEMOS) D1 R2 & mini"
* Set Flash Size: 4MB (FS:2MB OTA:~1019KB)
* Select the **Port** from the tools menu.

### Loading Supporting Library Files in Arduino
Use the Arduino guide for details on how to installing and manage libraries https://www.arduino.cc/en/Guide/Libraries

**Packages** -- the following packages and libraries are used (download and install):
* <WiFiManager.h> --> https://github.com/tzapu/WiFiManager (latest)
* <TimeLib.h> --> https://github.com/PaulStoffregen/Time
* <Adafruit_GFX.h> --> https://github.com/adafruit/Adafruit-GFX-Library
* <Max72xxPanel.h> --> https://github.com/markruys/arduino-Max72xxPanel
* <JsonStreamingParser.h> --> https://github.com/squix78/json-streaming-parser

Note ArduinoJson (version 5.13.1) is now included as a library file.

## Building with PlatformIO.
Use [**VScode**](https://code.visualstudio.com/docs) with [**PlatformIO**](https://platformio.org/) or better, its desendant fork [**PIOarduino**](https://marketplace.visualstudio.com/items?itemName=pioarduino.pioarduino-ide) extension.

Please refer to the links on how to install VScode on your OS.

Then, open this project with `Visual Studio Code`, via File --> Open Folder: select folder containing `platformio.ini` file.

The `platformio.ini` file contains the references to the required external libraries and version numbers.

To build, open the `pioarduino` or `platformio` extension (left hand side icon), then under *PROJECT TASKS* -> *Default* -> *General* : select **Build** and then **Upload**

## Initial Configuration
Editing the **Settings.h** file is totally optional and not required.
All API Keys are managed in the Web Interface.
It is not required to edit the Settings.h file and re-compiling the code.
* Open Weather Map free service API key: http://openweathermap.org/  -- this is used to get weather data and the current time zone from a selected City. This API key is required for correct time.
* <del>TimeZoneDB free registration for API key: https://timezonedb.com/register -- this is used for setting the time and getting the correct time zone as well as managing time changes due to Day Light Savings time by regions.  This key is set and managed only through the web interface. TimeZoneDB key is required for correct time display.</del><br> This is no longer needed nor requested!
* News API key (free): https://newsapi.org/ -- Optional if you want to get current news headlines.
* Your OctoPrint API Key -- optional if you use the OctoPrint status.
* Your Pi-hole API Key -- optional if you use the Pi-hole status.
* Supports Chained 4x1 LED matrix displays -- configure up to 16x1 in the Settings.h file.

NOTE: The settings in the Settings.h are the default settings for the first loading. After loading you will manage changes to the settings via the Web Interface. If you want to change settings again in the settings.h, you will need to erase the file system on the Wemos or use the “Reset Settings” option in the Web Interface.

## Web Interface
The Marquee Scroller uses the **WiFiManager** so when it can't find the last network it was connected to
it will become a **Access Point Hotspot** -- connect to it with your phone at http://192.168.4.1/ and you can then enter your WiFi connection information.

After connected to your WiFi network it will display the IP address assigned to it and that can be
used to open a browser to the Web Interface.  You will be able to manage your API Keys through the web interface.

The default user / password for the configuration page is: admin / password

The Clock will display the time of the City selected for the weather.

<p align="left" title="the (old) webinterface on a phone">
  <img src="/images/20180419_065805.png" width="200"/>
  <img src="/images/20180419_065815.png" width="200"/>
  <img src="/images/20180419_065832.png" width="200"/>
  <img src="/images/20180419_065858.png" width="200"/>
</p>


## Contributors
David Payne <br>
Nathan Glaus <br>
Daniel Eichhorn -- Author of the TimeClient class (in older versions) <br>
yanvigdev <br>
nashiko-s <br>
magnum129 <br>
rob040  --  author of this repo and these [feature enhancements listed above](#feature-enhancements)<br>


Contributing to this software is warmly welcomed. You can do this basically by forking from master, committing modifications and then making a pulling requests against the latest DEV branch to be reviewed (follow the links above for operating guide). Detailed comments are encouraged. Adding change log and your contact into file header is encouraged. Thanks for your contribution.

When considering making a code contribution, please keep in mind the following goals for the project:
* User should not be required to edit the Settings.h file to compile and run.  This means the feature should be simple enough to manage through the web interface.
* Changes should always support the recommended hardware (links above).

<p title="Marquee Scroller">
  <img src="/images/ScrollingClock.jpg" width="400"/>
</p>
