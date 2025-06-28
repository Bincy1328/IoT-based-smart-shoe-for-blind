/*
  
Copyright (c) 2013-2018 Ubidots.
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Adapted from code by Ubidots Inc (ubidots.com)

09 March 2018: Modified by Jason Pecor - Alorium Technology
26 April 2018: Modified by Bryan Craker - Alorium Technology
============================================================
Modfied to support Alorium Technology XLR8 and Snō development
boards connected via the ATWINC1500 WiFi module.

*/

#include "UbidotsXLR8.h"

/**
 * Constructor.
 */
Ubidots::Ubidots(char* token, char* server = SERVER) {
  _token = token;
  _server = server;
  _currentValue = 0;
  _val = (Value *)malloc(MAX_VALUES*sizeof(Value));
}

/**
 * Initialize the WiFi interface
 */
void Ubidots::initialize() {

  Serial.println(F("Initializing..."));
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  char* fv = WiFi.firmwareVersion();
  if (strcmp(fv, FIRMWARE)) {
    Serial.println("Please upgrade the firmware");
  }

  MacAsLabel();

}

/**
 * This function turns on various debugging statement in the code
 */
void Ubidots::setDebug() {
  _debug = true;
}

/**
 * This function sets device label to the WiFi device's Mac address
 */
void Ubidots::MacAsLabel() {
  byte mac[6];
  WiFi.macAddress(mac);
  char* defaultLabel = (char *) malloc(sizeof(char) * 13);
  sprintf(defaultLabel, "%02x%02x%02x%02x%02x%02x\0", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  setDeviceLabel(defaultLabel);
}

/**
 * This function sets the private device name value
 * @arg deviceName the device name to set in requests
 */
void Ubidots::setDeviceName(char* deviceName) {
  _dsName = deviceName;
}

/**
 * This function sets the private device label value
 * @arg deviceLabel the device label to set in requests
 */
void Ubidots::setDeviceLabel(char* deviceLabel) {
  _dsTag = deviceLabel;
}

/**
 * This function is to get value from the Ubidots API
 * @arg id the id where you will get the data
 * @return num the data that you get from the Ubidots API
 */
float Ubidots::getValue(char* id) {

  int timeout = 0;
  float num;
  uint8_t max_retries = 0;
  char* response;
  char * pch;
  char* data = (char *) malloc(sizeof(char) * 300);

  // Build the GET command
  sprintf(data, "%s|GET|%s|%s|end", USER_AGENT, _token, id);

  _client.connect(_server, PORT); // Initial connection

  while(!_client.connected()) {
    if (_debug) {
      Serial.println("Attempting to connect");
    }
    _client.connect(_server, PORT);
    max_retries++;
    if (max_retries > 5){
      if (_debug) {
        Serial.println("Connection failed");
      }
      free(data);
      return NULL;
    }
    delay(5000);
  }

  if (_debug) {
    Serial.println("Getting variable:");
    Serial.println(data);
  }
  _client.print(data);
  free(data);

  while(!_client.available() && timeout < 5000) {
    timeout++;
    delay(1);
    if(timeout>=5000){
      if (_debug) {
        Serial.println("Max timeout reached");
      }
      _client.stop();
      delay(5);
      return NULL;
    }
  }

  response = (char *) malloc(sizeof(char) * 300);

  while (_client.available()) {
    char c = _client.read();
    if (c == -1){
      if (_debug) {
        Serial.println("Error reading from server");
      }
      _client.stop();
      delay(5);
      free(response);
      return NULL;
    }
    if (((c >= '0') && (c <= '9')) || (c == '.') || (c == '-')) {
      sprintf(response, "%s%s", response, c);
    }
    delay(10);
  }

  num = atof(response);
  free(response);

  if (_debug) {
    Serial.print("Got: ");
    Serial.println(num);
  }

  _client.stop();
  delay(5);
  return num;

}

/**
 * This function is to get value from the Ubidots API
 * @arg device the device data is coming from
 * @arg variable the variable name
 * @return num the data that you get from the Ubidots API
 */
float Ubidots::getValueWithDeviceLabel(char* device, char* variable) {

  char * response;
  char * pch;
  int timeout = 0;
  float num;
  uint8_t max_retries = 0;
  char* data = (char *) malloc(sizeof(char) * 300);

  sprintf(data, "%s|LV|%s|%s:%s|end", USER_AGENT, _token, device, variable);

  _client.connect(_server, PORT); // Initial connection

  while(!_client.connected()) {
    _client.connect(_server, PORT);
    max_retries++;
    if(max_retries>5){
      free(data);
      return NULL;
    }
    delay(5000);
  }

  if (_debug) {
    Serial.println(data);
  }
  _client.print(data);
  free(data);

  while(!_client.available() && timeout < 5000) {
    timeout++;
    delay(1);
    if(timeout>=5000){
      _client.stop();
      delay(5);
      return NULL;
    }
  }

  response = (char *) malloc(sizeof(char) * 300);

  while (_client.available()) {
    char c = _client.read();
    if (c == -1){
      _client.stop();
      delay(5);
      free(response);
      return NULL;
    }
    if (((c >= '0') && (c <= '9')) || (c == '.') || (c == '-')) {
      sprintf(response, "%s%c", response, c);
    }
    delay(10);
  }

  num = atof(response);
  free(response);

  _client.stop();
  delay(5);
  return num;

}

/**
 * Add a value of variable to save
 * @arg variable_label variable label to save in a struct
 * @arg value variable value to save in a struct
 */
void Ubidots::add(char *variable_label, double value) {
  add(variable_label, value, NULL, NULL);
}

/**
 * Add a value of variable to save
 * @arg variable_label variable label to save in a struct
 * @arg value variable value to save in a struct
 * @arg ctext variable context value to save in a struct
 */
void Ubidots::add(char *variable_label, double value, char *ctext) {
  add(variable_label, value, ctext, NULL);
}

/**
 * Add a value of variable to save
 * @arg variable_label variable label to save in a struct
 * @arg value variable value to save in a struct
 * @arg ctext variable context value to save in a struct
 * @arg timestamp variable timestamp value to save in a struct in milliseconds
 */
void Ubidots::add(char *variable_label, double value, char *ctext, unsigned long timestamp) {
  (_val+_currentValue)->idName = variable_label;
  (_val+_currentValue)->idValue = value;
  (_val+_currentValue)->ctext = ctext;
  (_val+_currentValue)->timestamp = timestamp;
  _currentValue++;
  if (_currentValue > MAX_VALUES) {
    _currentValue = MAX_VALUES;
  }
}

/**
 * Send all data of all variables that you saved
 * @return true upon success, false upon error.
 */
bool Ubidots::sendAll() {

  char * response;
  int timeout = 0;
  uint8_t max_retries = 0;
  char* allData = (char *) malloc(sizeof(char) * 300);

  sprintf(allData, "%s|POST|%s|%s:%s=>", USER_AGENT, _token, _dsTag, _dsName);

  //
  for (int i = 0; i < _currentValue; ) {

    char* flc = (char *) malloc(sizeof(char) * 20);
    dtostrf(((_val + i)->idValue), 4, 2, flc);
    sprintf(allData, "%s%s:%s", allData, (_val + i)->idName, flc);
    free(flc);

    if ((_val + i)->ctext != NULL) {
      sprintf(allData, "%s%s", allData, (_val + i)->ctext);
    }

    if ((_val + i)->timestamp != NULL) {
      sprintf(allData, "%s@%d", allData, (_val + i)->timestamp);
    }

    i++;
    if (i < _currentValue) {
      sprintf(allData, "%s,", allData);
    }

  }

  sprintf(allData, "%s|end", allData);

  if (_debug) {
    Serial.println(allData);
  }

  _client.connect(_server, PORT);

  while (!_client.connected()) {
    _client.connect(_server, PORT);
    max_retries++;
    if (max_retries > 5) {
      if (_debug) {
        Serial.println("Connection failed");
      }
      free(allData);
      _currentValue = 0;
      _client.stop();
      return false;
    }
    delay(5000);
  }

  if (_debug) {
    Serial.println("Sending data");
  }
  _client.print(allData);
  free(allData);

  while (!_client.available() && timeout < 5000) {
    timeout++;
    delay(1);
    if (timeout >= 5000) {
      if (_debug) {
        Serial.println("Max timeout reached");
      }
      _client.stop();
      delay(5);
      return false;
    }
  }

  response = (char *) malloc(sizeof(char) * 300);

  while (_client.available()) {
    char c = _client.read();
    if (c == -1) {
      if (_debug) {
        Serial.println("Error connecting to server");
      }
      _client.stop();
      delay(5);
      return false;
    }
    sprintf(response, "%s%c", response, c);
    delay(10);
  }

  if (_debug) {
    Serial.print("Response: ");
    Serial.println(response);
  }
  free(response);

  _client.stop();
  _currentValue = 0;
  return true;

}

/**
 * Connects to the WiFi service
 * @arg ssid the ssid of the WiFi
 * @arg pass the pass of the WiFi
 * @arg security
 */
void Ubidots::wifiConnection(char* ssid, char* pass) {

  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.print("Connect to: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (_client.connect(_server, PORT)) {
    Serial.println("connected to server");
  }

}

/**
 * Prints the WiFi connection information
 */
void Ubidots::printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.print(rssi);
  Serial.println(" dBm");

}

