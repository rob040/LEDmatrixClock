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

#include "OctoPrintClient.h"

OctoPrintClient::OctoPrintClient(const String &ApiKey, const String &server, int port, const String &user, const String &pass) {
  updateOctoPrintClient(ApiKey, server, port, user, pass);
}

void OctoPrintClient::updateOctoPrintClient(const String &ApiKey, const String &server, int port, const String &user, const String &pass) {
  server.toCharArray(myServer, 100);
  myApiKey = ApiKey;
  myPort = port;
  encodedAuth.clear();
  if (user != "") {
    String userpass = user + ":" + pass;
    base64 b64;
    encodedAuth = b64.encode(userpass, true);
  }
}

boolean OctoPrintClient::validate() {
  boolean rtnValue = false;
  printerData.error.clear();
  if (String(myServer).length() == 0) {
    printerData.error += F("Server address is required; ");
  }
  if (myApiKey.length() == 0) {
    printerData.error += F("ApiKey is required; ");
  }
  if (printerData.error.length() == 0) {
    rtnValue = true;
  }
  return rtnValue;
}

void OctoPrintClient::getPrinterJobResults() {
  if (!validate()) {
    return;
  }
  WiFiClient printClient;
  printClient.setTimeout(10000);
  String apiGetData = F("GET /api/job HTTP/1.1");

  Serial.println(F("Getting Octoprint Data"));
  Serial.println(apiGetData);
  result = "";
  if (printClient.connect(myServer, myPort)) {  //starts client connection, checks for connection
    printClient.println(apiGetData);
    printClient.println(F("Host: ") + String(myServer) + ":" + String(myPort));
    printClient.println(F("X-Api-Key: ") + myApiKey);
    if (encodedAuth != "") {
      printClient.print(F("Authorization: "));
      printClient.println(F("Basic ") + encodedAuth);
    }
    printClient.println(F("User-Agent: ArduinoWiFi/1.1"));
    printClient.println(F("Connection: close"));
    if (printClient.println() == 0) {
      Serial.println(F("OctoPrint Connection failed."));
      resetPrintData();
      printerData.error = F("Octoprint Connection Failed");
      return;
    }
  }
  else {
    Serial.println(F("Connection for OctoPrint data failed: ") + String(myServer) + ":" + String(myPort)); //error message if no client connect
    Serial.println();
    resetPrintData();
    printerData.error = F("Connection for OctoPrint data failed: ") + String(myServer) + ":" + String(myPort);
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    resetPrintData();
    printerData.error = String(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!printClient.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    resetPrintData();
    printerData.error = F("Invalid response");
    return;
  }

  // Parse JSON object
  JsonDocument jdoc;
  DeserializationError error = deserializeJson(jdoc, printClient);
  if (error) {
    Serial.println(F("OctoPrint Data Parsing failed!"));
    return;
  }

  // TODO: check if "String" is sensible type for all parameters
  printerData.averagePrintTime = jdoc["job"]["averagePrintTime"].as<String>();
  printerData.estimatedPrintTime = jdoc["job"]["estimatedPrintTime"].as<String>();
  printerData.fileName = jdoc["job"]["file"]["name"].as<String>();
  printerData.fileSize = jdoc["job"]["file"]["size"].as<String>();
  printerData.lastPrintTime = jdoc["job"]["lastPrintTime"].as<String>();
  printerData.progressCompletion = jdoc["progress"]["completion"].as<String>();
  printerData.progressFilepos = jdoc["progress"]["filepos"].as<String>();
  printerData.progressPrintTime = jdoc["progress"]["printTime"].as<String>();
  printerData.progressPrintTimeLeft = jdoc["progress"]["printTimeLeft"].as<String>();
  printerData.state = jdoc["state"].as<String>();

  if (isPrinting()) {
    Serial.println("Status: " + printerData.state + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
  } else if (isOperational()) {
    Serial.println("Status: " + printerData.state);
  } else {
    Serial.println(F("Printer Not Opperational"));
  }

  printClient.stop(); //stop client
}

// Reset all PrinterData
void OctoPrintClient::resetPrintData() {
  printerData.averagePrintTime = "";
  printerData.estimatedPrintTime = "";
  printerData.fileName = "";
  printerData.fileSize = "";
  printerData.lastPrintTime = "";
  printerData.progressCompletion = "";
  printerData.progressFilepos = "";
  printerData.progressPrintTime = "";
  printerData.progressPrintTimeLeft = "";
  printerData.state = "";
  printerData.error = "";
}

String OctoPrintClient::getAveragePrintTime(){
  return printerData.averagePrintTime;
}

String OctoPrintClient::getEstimatedPrintTime() {
  return printerData.estimatedPrintTime;
}

String OctoPrintClient::getFileName() {
  return printerData.fileName;
}

String OctoPrintClient::getFileSize() {
  return printerData.fileSize;
}

String OctoPrintClient::getLastPrintTime(){
  return printerData.lastPrintTime;
}

String OctoPrintClient::getProgressCompletion() {
  return String(printerData.progressCompletion.toInt());
}

String OctoPrintClient::getProgressFilepos() {
  return printerData.progressFilepos;
}

String OctoPrintClient::getProgressPrintTime() {
  return printerData.progressPrintTime;
}

String OctoPrintClient::getProgressPrintTimeLeft() {
  return printerData.progressPrintTimeLeft;
}

String OctoPrintClient::getState() {
  return printerData.state;
}

boolean OctoPrintClient::isPrinting() {
  boolean printing = false;
  if (printerData.state == "Printing") {
    printing = true;
  }
  return printing;
}

boolean OctoPrintClient::isOperational() {
  boolean operational = false;
  if (printerData.state == "Operational") {
    operational = true;
  }
  return operational;
}

String OctoPrintClient::getError() {
  return printerData.error;
}
