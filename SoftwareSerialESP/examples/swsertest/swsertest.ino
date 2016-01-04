
#include <SoftwareSerialESP.h>

SoftwareSerialESP swSer(D1, D2, 128);

void setup() {
  Serial.begin(115200L);
  swSer.begin(9600L);
  Serial.println("\nSoftware serial test started");
  delay(5000);
  swSer.write(0x11);
  swSer.write(0x03);
  swSer.write(0x0c);
  swSer.write(0x02);
  swSer.write(0x1e);
  swSer.write(0xC0);
//  for (char ch = ' '; ch <= 'z'; ch++) {
//    swSer.write(ch);
//  }
Serial.println("dadsadsa");
  //swSer.println("");

}

void loop() {
  swSer.write(0x11);
  swSer.write(0x03);
  swSer.write(0x0c);
  swSer.write(0x02);
  swSer.write(0x1e);
  swSer.write(0xC0);

  while(!swSer.available()) {
    delay (10);
    Serial.print(".");
  }
  while (swSer.available()) {
    Serial.println(swSer.read(), HEX);  
  }
  delay(20000);
//  delay(1000);
//  Serial.println(swSer.available());
//  Serial.println(swSer.read(), HEX);
//  delay(1000);
//  Serial.println(swSer.available());
//  Serial.println(swSer.read(), HEX);
//  delay(1000);
//  Serial.println(swSer.available());
//  Serial.println(swSer.read(), HEX);
//  delay(1000);
//  Serial.println(swSer.available());
//  Serial.println(swSer.read(), HEX);
//  delay(1000);
//  Serial.println(swSer.available());
//  Serial.println(swSer.read(), HEX);
//  delay(1000);
//  Serial.println(swSer.available());
//  Serial.println(swSer.read(), HEX);
//  delay(5000);
//  if (swSer.available()) {
//    Serial.write(swSer.read());
//  }
//  if (Serial.available()) {
//    swSer.write(Serial.read());
//  }

}
