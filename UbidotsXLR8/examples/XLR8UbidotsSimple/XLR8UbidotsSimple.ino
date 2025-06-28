//--------------------------------------------------------------------
//  XLR8UbidotsSimple.ino
//--------------------------------------------------------------------
// Copyright (c) 2018 Alorium Technology.  All rights reserved.
// by Jason Pecor Alorium Technology
//  
// This file is a simple example for using the Alorium Technology
// XLR8 development board with Ubidots.
//
// The hardware setup for this example requires SPI connection to an
// Adafruit ATWINC1500 WiFi breakout module.
//--------------------------------------------------------------------
#include <UbidotsXLR8.h> 
#include <SPI.h>
#include <WiFi101.h>

WiFiClient client;

// Ubidots Token
#define TOKEN ""

char ssid[] = "";  // WiFi SSID
char pass[] = "";  // WiFi Password

Ubidots ubidots(TOKEN);

float control0;
int sensor0, sensor1, sensor2;

void setup() {

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  // Configure SPI for ATWINC Module
  WiFi.setPins(8,7,4);

  // Initialize and connect to Ubidots
  ubidots.initialize();
  ubidots.setDeviceName("XLR8");
  ubidots.setDeviceLabel("XLR8");
  ubidots.wifiConnection(ssid,pass);

}

void loop() {

  // Send random values for sensor variables
  sensor0 = random(100,199);
  sensor1 = random(100,199);
  sensor2 = random(100,199);
  
  Serial.println("Send sensor data");
  Serial.print("sensor0: "); Serial.println(sensor0);
  Serial.print("sensor1: "); Serial.println(sensor1);
  Serial.print("sensor2: "); Serial.println(sensor2);

  ubidots.add("sensor0", sensor0);
  ubidots.add("sensor1", sensor1);
  ubidots.add("sensor2", sensor2);
  ubidots.sendAll();

  
  // Get values from ubidots control widget
  Serial.println("Get values");
  control0 = ubidots.getValueWithDeviceLabel("XLR8", "control0");
  
  Serial.print("control0:");
  Serial.println(control0);
  Serial.println("");
  
  
  // Delay 
  delay(10000);  
  
}

