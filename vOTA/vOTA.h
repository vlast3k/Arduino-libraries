/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef vOTA_h
#define vOTA_h
#include <Arduino.h>
#include <WString.h>
#include <vUtils.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>


// library interface description
class vOTA
{
  // user-accessible "public" interface
  public:
    static void doHttpUpdate();
    static void handleOTA();
    static void startOTA();
    static bool startedOTA;
};

#endif

