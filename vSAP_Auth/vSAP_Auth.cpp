/*
  Test.h - Test library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/


// include this library's description file
#include "vSAP_Auth.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances

vSAP_Auth::vSAP_Auth(const char *user, const char *pass)
{
    checkSAPAuth(user, pass);
}


// Private Methods /////////////////////////////////////////////////////////////
// Functions only available to other functions in this library


int vSAP_Auth::checkSAPAuth(const char *user, const char *pass) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to WiFi");
    return -1;
  }
  if (strstr(WiFi.SSID().c_str(), "SAP-Guest") == 0) {
    Serial.printf("Network is not SAP-Guest : %s\n", WiFi.SSID().c_str());
    return -1;
  }
  strcpy(SAP_AUTH_USER, user);
  strcpy(SAP_AUTH_PASS, pass);
  Serial.println("Checking SAP Authentication");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot set SAP Guest credentials. No WiFi!");
    return -1;
  }
  if (sendPing() == 302) {
    if (httpAuthSAP() == 302) {
      Serial.println("SAP-Guest User/Pass - incorrect");
      return -1;
    }
    if (sendPing() != 200) {
      Serial.println("Could not authenticate for SAP");
      return -1;
    }
  }
  Serial.println("SAP Auth - OK!");
  return 1;
}


int vSAP_Auth::httpAuthSAP() {
  char postData[100];
  sprintf(postData, "user=%s&password=%s&cmd=authenticate&url=http%3A%2F%2Fgoogle.bg%2F&Login=Log+In", SAP_AUTH_USER, SAP_AUTH_PASS);
  HTTPClient http;
  http.begin("https://securelogin.wlan.sap.com/cgi-bin/login");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Referer", "https://emea-guest.wlan.sap.com/guest/sap_guest_register_login.php?_browser=1");
  int rc = http.POST((uint8_t*)postData, strlen(postData));
  if (rc < 0 || rc == 200) return 1;
  else return -1; 
}
  // rc == -1 - timeout connect = connection exists
  // rc == 302 - bad user pass
  // rc == 200 - authenticated
  

int vSAP_Auth::sendPing() {
  HTTPClient http;
  http.begin("http://ping.eu/");
  return http.sendRequest("HEAD", (uint8_t*)"", 0);

  //302 - no connection
  //200 - connection ok
}

