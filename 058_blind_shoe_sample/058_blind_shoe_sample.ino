/////username : blindshoe123 ///password :blind@1234
#include "Ubidots.h"
bool bufferSent = false;

const char* UBIDOTS_TOKEN = "BBFF-gtq8vZQSRNxR9k41n7Ai2MIFWQr9jB";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "blindshoe";      // Put here your Wi-Fi SSID
const char* WIFI_PASS = "12345678";      // Put here your Wi-Fi password
Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);

const int pingPin = D1; 
const int echoPin = D2; 
const int accPin = A0;    //x-axis of the Accelerometer 

void setup() {
  Serial.begin(9600);
  pinMode(D5,OUTPUT);//buzze&vib motor
  //pinMode(D6,OUTPUT);
  digitalWrite(D5,LOW);
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  
  pinMode(pingPin,OUTPUT);
  pinMode(echoPin,INPUT);
}

void loop() {
  int acc = analogRead(accPin);
  delay(100);
  long duration, inches, cm;
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   cm = microsecondsToCentimeters(duration);

   if (cm < 10)
   {
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       
       ubidots.add("DISTANCE =", cm); 
       bufferSent = false;
       bufferSent = ubidots.send();
       Serial.println("ACCIDENT"); 
   }
      
  if (acc > 480)
  {
       Serial.print("MEMS    ");Serial.println(accPin);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       digitalWrite(D5,HIGH);delay(250);digitalWrite(D5,LOW);delay(250);
       
    ubidots.add("MEMS SENSOR =", 1); 
    bufferSent = false;
    bufferSent = ubidots.send();
    Serial.println("ACCIDENT");
  }
 
    ubidots.add("DISTANCE =", cm); 
    ubidots.add("MEMS SENSOR =", 0); 
    bufferSent = false;
    bufferSent = ubidots.send();
   delay(50);

}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
