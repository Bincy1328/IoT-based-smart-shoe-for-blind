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


#ifndef __XLR8Ubidots_
#define __XLR8Ubidots_

#include <SPI.h>
#include <string.h>
#include <WiFi101.h>

// Ubidots Server Parameters - Confidgured for TCP
#define SERVER "translate.ubidots.com"
#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds)
#define PORT 9012

#define FIRMWARE "19.5.2"
#define USER_AGENT "XLR81500/1.0"

#define MAX_VALUES 3

typedef struct Value {
  char *idName;
  double idValue;
  char *ctext = NULL;
  unsigned long timestamp = NULL;
} Value;

class Ubidots {

 private:
  char* _token;
  char* _server;
  char* _dsTag;
  char* _dsName;
  uint8_t _currentValue;
  Value * _val;
  WiFiClient _client;
  bool _debug = false;
  void MacAsLabel();
  void printWifiStatus();

 public:
  Ubidots(char* token, char* server = SERVER);
  void initialize();
  void setDebug();
  void setDeviceName(char* deviceName);
  void setDeviceLabel(char* deviceLabel);
  float getValue(char* id);
  float getValueWithDeviceLabel(char* device, char* variable);
  void add(char *variable_label, double value);
  void add(char *variable_label, double value, char* ctext);
  void add(char *variable_label, double value, char* ctext, unsigned long timestamp);
  bool sendAll();
  void wifiConnection(char* ssid, char* pass);

};

#endif

