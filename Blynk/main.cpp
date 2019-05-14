#include <WiFiEsp.h>
#include <Arduino.h>
#include <SPI.h>
#include "BlynkSimpleWifiMaixGo.h"

char auth[] = "YOUR TOKEN";
char ssid[] = "YOUR SSID";                        // your network SSID (name)
char pass[] = "YOUR PWD";                         // your network password
IPAddress Local_Blynk_server(XXX, XXX, XXX, XXX); // The IP address of the local Blynk server

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  Serial.print("I received the value: ");
  Serial.println(pinValue);
}

BLYNK_WRITE(V2)
{
  int pinValue1 = param[0].asInt();
  int pinValue2 = param[1].asInt();
  int pinValue3 = param[2].asInt();

  Serial.print("I received the values: ");
  Serial.print(pinValue1);
  Serial.print(" ");
  Serial.print(pinValue2);
  Serial.print(" ");
  Serial.println(pinValue3);
}

void setup()
{
  Serial.begin(115200);

  // initialize ESP module
  Serial1.begin(115200);
  WiFi.init(&Serial1);
  Serial.println("Wifi init done");
  Blynk.begin(auth, ssid, pass, Local_Blynk_server, 8080);
  Serial.println("Blynk begin done");
}

void loop()
{
  // put your main code here, to run repeatedly:
  Blynk.run();
}