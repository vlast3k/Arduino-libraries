Ever since those little WiFi modules came to us it has been a problem to connect them to SAP's networks. SAP-Corporate and SAP-Internet are out of question for two reasons - WPA-Enterprise and limitation of the types of devices that can connect to those networks.

SAP-Guest required logging in with a Guest user, yet the login screen was using HTTPS. Just until recently HTTPS on the ESP8266 could only be consumed in case one develops a C program using the Esspresiv toolchain, which is quite a demanding task for the majority of current software developers.

Luckiely, with the latest (GIT) version of the Arduino IDE for ESP8266 project (https://github.com/esp8266/Arduino) has introduced support for HTTPS and now the task of logging into SAP-Guest has become very simple.

First, you need to go to this link from inside SAP-Corporate network: https://emea-guestvoucher.wlan.sap.com/guest/auth_login.php?target=%2Fguest%2F

And create a guest user. You can choose a validity up to 1 year and once it is created, you can modify to user to allow also connections from more than 2 computers. (at least for me this only worked if i modify it from "Modify multiple")

Then you provide the user/pass as in the example in the sketch.

Usually once authenticated, the network knows the MAC adress for the device and only if you disconnect for more than 2 hours you need to reauthenticate.

If you are not authenticated - then if you request a web site - you are redirected to the login page for SAP-Guest. This is also the logic of the sketch. So if it is run each time the ESP8266 boots, it will ensure that you always have connection to SAP-Guest.

Currently it is hosted in my Git Repository, i need to move it to the D-Shop one in the next days

https://github.com/vlast3k/ESP8266_SAPGuest

Any feedback and comments are welcome!

