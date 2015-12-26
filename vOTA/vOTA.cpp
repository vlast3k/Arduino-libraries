/*
  Test.h - Test library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/


// include this library's description file
#include <vOTA.h>

//
#include <WString.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Streaming.h>
#include <EEPROM.h>
#include <vUtils.h>
#include <vSAP_Auth.h>
#include <ArduinoOTA.h>

void vOTA::doHttpUpdate() {
  Serial << "Starting Web update" << endl;
  t_httpUpdate_return ret = ESPhttpUpdate.update("https://raw.githubusercontent.com/vlast3k/ESP8266_SerialOTATS/master/fw/latest.bin");
  switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("HTTP_UPDATE_FAILED");
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void vOTA::handleOTA() {
  if (startedOTA) ArduinoOTA.handle();
}

void vOTA::startOTA() {
  Serial << "Starting OTA..." << endl;
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("Ready");
  startedOTA = true;
}
