#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "vSAP_Auth.h"

//url : https://emea-guestvoucher.wlan.sap.com/guest/auth_login.php?target=%2Fguest%2F

void connectWifi(const char *ssid, const char *pass) {
  WiFi.begin("SAP-Guest", "not-relevant");
  Serial.printf("\nConnecting to %s\n", WiFi.SSID().c_str());  
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP address: %s\n\n", WiFi.localIP().toString().c_str());
}

void setup() {
  Serial.begin(115200);
  delay(10);

  connectWifi("SAP-Guest", "not-used");
  vSAP_Auth("63597688", "w6QQckVn");
}

void loop() {

}
