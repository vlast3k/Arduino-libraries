/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef vWifiHelpers_h
#define vWifiHelpers_h
#include <Arduino.h>
#include <WString.h>
#include <vUtils.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define EE_WIFI_SSID_30B 0
#define EE_WIFI_P1_30B 30
#define EE_WIFI_P2_30B 60

// library interface description
class vWifiHelpers
{
  // user-accessible "public" interface
  public:
    static int wifiConnectToStoredSSID();
    static void connectToWifi(const char *s1, const char *s2, const char *s3) ;
    static void wifiScanNetworks();
    static int processResponseCodeATFW(HTTPClient *http, int rc);
    static int sendPing();
};

#endif

