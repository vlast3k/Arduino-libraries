/*
  Test.h - Test library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/


// include this library's description file
#include "vSAP_Auth.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <Streaming.h>

#define SERIAL Serial


// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances

vSAP_Auth::vSAP_Auth(const char *user, const char *pass, bool dbg) {
  strcpy(SAP_AUTH_USER, user);
  strcpy(SAP_AUTH_PASS, pass);
  DEBUG = dbg;
  checkSAPAuth();
}

vSAP_Auth::vSAP_Auth(int EE_USER, int EE_PASS, bool dbg) {
  EEPROM.get(EE_USER, SAP_AUTH_USER);
  EEPROM.get(EE_PASS, SAP_AUTH_PASS);
  DEBUG = dbg;
  checkSAPAuth();
}

/*void vSAP_Auth::on302() {
  char str[30];
  EEPROM.get(EE_WIFI_SSID_30B, str);
  if (DEBUG) SERIAL << F("Stored Network is:") << str << endl;
  if (strstr(str, "SAP-Guest") == 0) {
    checkSAPAuth();
    sendPing();
  }
}*/

int vSAP_Auth::checkSAPAuth() {
  if (strstr(WiFi.SSID().c_str(), "SAP-Guest") == 0) {
    if (DEBUG) SERIAL << F("Network is not SAP-Guest : ") << WiFi.SSID() << endl;
    return -1;
  }
  SERIAL << F("Checking SAP Authentication") << endl;
  if (WiFi.status() != WL_CONNECTED) {
    SERIAL << F("Cannot set SAP Guest credentials. No WiFi!") << endl;
    return -1;
  }
  httpAuthSAP();
  if (sendPing() == 302) {
    if (httpAuthSAP() == 302) {
      SERIAL << F("SAP-Guest User/Pass - incorrect") << endl;
      return -1;
    }
    if (sendPing() != 200) {
      SERIAL << F("Could not authenticate for SAP") << endl;
      return -1;
    }
  }
  SERIAL << F("SAP Auth - OK!") << endl;
  return 1;
}

char *vSAP_Auth::sapAuthPreparePostData(char *postData) {
  if (SAP_AUTH_USER[0] < 2) {
    SERIAL << F("Missing SAP-Guest user/pass") << endl;
    return NULL;
  }
  sprintf(postData, String(F("user=%s&password=%s&cmd=authenticate&url=http%3A%2F%2Fgoogle.bg%2F&Login=Log+In")).c_str(), SAP_AUTH_USER, SAP_AUTH_PASS);
  return postData;
}



int vSAP_Auth::httpAuthSAP() {
  char postData[100];
  if ( ! sapAuthPreparePostData(postData) ) return -1;
//  char sapAuthHeaders* = "\"Content-Type\", \"application/x-www-form-urlencoded\",\"Referer\" \"https://emea-guest.wlan.sap.com/guest/sap_guest_register_login.php?_browser=1\"";    
  HTTPClient http;
  http.begin(F("https://securelogin.wlan.sap.com/cgi-bin/login"));
  http.addHeader("Content-Type", String(F("application/x-www-form-urlencoded")).c_str());
  http.addHeader("Referer", String(F("https://emea-guest.wlan.sap.com/guest/sap_guest_register_login.php?_browser=1")).c_str());
  int rc = http.POST((uint8_t*)postData, strlen(postData));
  if (rc < 0 || rc == 200) return 1;
  else return -1; 
//  int rc = sendHTTP("securelogin.wlan.sap.com", "POST", "/cgi-bin/login", sapAuthHeaders, postData, true, true);
//  SERIAL << "SAP Auth Response is: " << rc << endl;
//  if (rc == -1 || rc == 200) return 1;
//  else return -1;
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

