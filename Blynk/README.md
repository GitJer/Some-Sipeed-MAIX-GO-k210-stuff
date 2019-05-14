This is code to get [Blynk](https://github.com/blynkkk/blynk-library), see also blynk.io, working on the MaixGo using [Maixduino](https://github.com/sipeed/Maixduino). For now I'll assume using a local Blynk server, it should not be much work to make it more general and use the internet Blynk server.

Two things are needed: a MaixGo version of the Blynk include file, and a change to a WiFiEsp library.

The Blynk include file for the MaixGo is almost the same as the already existing file "BlynkSimpleWiFiShield101.h" since basically the ESP8285 connected to the K210 is the same as a wifi shield.

The change in the WiFiEsp library is due to such an overwhelming amount of status requests that interesting information may get swamped out. See [here](https://github.com/bportaluri/WiFiEsp/issues/180) for an explanation.

How to get it working:
* Download Maixduino (not needed if you use [platformio](https://platformio.org/))
* In the Maixduino directory search for the file WiFiEspClient.cpp (if you use platformio it is ~/.platformio/packages/framework-maixduino/libraries/WiFiEsp/src/WiFiEspClient.cpp) and replace it with the file in this repository.
* Download Blynk to the lib directory you use and add the file BlynkSimpleWifiMaixGo.h to the directory 'Blynk/src/'

You can use the example main.cpp. First fill in the token, ssid, pwd and IP address of your local Blynk server.

On your phone/tablet make a Blynk project and get the token. My project looks like this:

<img src="https://github.com/GitJer/Some-Sipeed-MAIX-GO-k210-stuff/blob/master/Blynk_project.jpg" width="100">

Where the button is connected to V1 and the zeRGBa is connected to V2:

<img src="https://github.com/GitJer/Some-Sipeed-MAIX-GO-k210-stuff/blob/master/Blynk_button_settings.jpg" width="100">

<img src="https://github.com/GitJer/Some-Sipeed-MAIX-GO-k210-stuff/blob/master/Blynk_zeRGBa_settings.jpg" width="100">


 

