/*
  Test.h - Test library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/


// include this library's description file
#include "vWifiHelpers.h"
//
#include <WString.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Streaming.h>
#include <EEPROM.h>
#include <vUtils.h>
#include <vSAP_Auth.h>


void vWifiHelpers::connectToWifi(const char *s1, const char *s2, const char *s3) {
  vUtils::storeToEE(EE_WIFI_SSID_30B, s1);
  vUtils::storeToEE(EE_WIFI_P1_30B, s2);
  vUtils::storeToEE(EE_WIFI_P2_30B, s3);

  vWifiHelpers::wifiConnectToStoredSSID();
}

int vWifiHelpers::wifiConnectToStoredSSID() {
  char ssid[30], pass[30], sappass[30];
  EEPROM.get(EE_WIFI_SSID_30B, ssid);
  EEPROM.get(EE_WIFI_P1_30B, pass);
  EEPROM.get(EE_WIFI_P2_30B, sappass);
  Serial << "Connecting to: " << ssid << "," << pass << "." << endl;
  WiFi.disconnect();
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial << "Connection failed status: " << WiFi.status() << endl;
    switch (WiFi.status()) {
      case WL_IDLE_STATUS: Serial << "WL_IDLE_STATUS" << endl; break;
      case WL_NO_SSID_AVAIL: Serial << "WL_NO_SSID_AVAIL" << endl;break;
      case WL_CONNECT_FAILED: Serial << "WL_CONNECT_FAILED" << endl;break;
      case WL_CONNECTION_LOST: Serial << "WL_CONNECTION_LOST" << endl;break;
      case WL_DISCONNECTED: Serial << "WL_DISCONNECTED" << endl;break;
    }
  } else {
    vSAP_Auth(pass, sappass);
    Serial << "IP address: " << WiFi.localIP() << endl << "GOT IP" << endl; 
  }
}

void vWifiHelpers::wifiScanNetworks() {
  Serial.println("scan start");
  WiFi.disconnect();
  delay(500);

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again

}

int vWifiHelpers::sendPing() {
  HTTPClient http;
  http.begin("http://ping.eu/");
  return processResponseCodeATFW(&http, http.sendRequest("HEAD", (uint8_t*)"", 0));

  //302 - no connection
  //200 - connection ok
}

int vWifiHelpers::processResponseCodeATFW(HTTPClient *http, int rc) {
  Serial << "Response Code: " << rc << endl;
  if (rc > 0) {
    Serial << "Payload: [" << http->getString() << "]" << endl;
    Serial << "CLOSED" << endl; // for compatibility with AT FW
  } else {
    Serial << "Failed" << endl;
  }
  return rc;
}

