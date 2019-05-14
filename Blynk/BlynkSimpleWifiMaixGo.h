/**
 * @file       BlynkSimpleWifi.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Jan 2015
 * @brief
 *
 */

#ifndef BlynkSimpleWifiMaixGo_h
#define BlynkSimpleWifiMaixGo_h

#ifndef BLYNK_INFO_CONNECTION
#define BLYNK_INFO_CONNECTION "ESP8266"
#endif

// Fix for WiFi shield (it has a crazy ping duration)
// #define BLYNK_TIMEOUT_MS 6000UL
//#define BLYNK_SEND_ATOMIC
// #define BLYNK_SEND_CHUNK 1024

#include <WiFiEsp.h>
#include <Adapters/BlynkWiFiCommon.h>

static WiFiEspClient _blynkWifiClient;
static BlynkArduinoClient _blynkTransport(_blynkWifiClient);
BlynkWifiCommon Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#endif
