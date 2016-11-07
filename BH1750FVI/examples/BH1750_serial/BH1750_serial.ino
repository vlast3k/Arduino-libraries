
/*
  This is a simple sketch to test BH1750FVI Light Sensor
  which communicates using the I2C Protocol 
  
  It writes out the lux value over the serial port

  Connect this sensor as follows:
  VCC >>> 3.3V or 5V
  SDA >>> A4 
  SCL >>> A5
  addr >> Gnd or VCC
  Gnd >>> Gnd
  
  BH1750FVI Sensor Setup 
  
  ##### I2C Addresses #####
  Addr_LOW 0x23 // Device address when address pin LOW
  Addr_HIGH 0x5C // Device address when address pin High

   
  ##### Modes ##### 
  Continuous_H - 1 lx resolution (16 bit), 120ms sampling time
  Continuous_H2 -  0.5 lx resolution (18 bit), 120ms sampling time
  Continuous_L - 4 lx resolution (15 bit), 16ms sampling time
  OneTime_H
  OneTime_H2
  OneTime_L
  
  Default for Begin() is Addr_LOW and Continuous_H
  
 */
 
// The library includes
#include <Wire.h>
#include "BH1750FVI.h"
#include <Streaming.h>
// Pin that LED is connected to
// Connect LED With 100ohm resistor
#define LEDPin 13 

// Initialize the BH1750FVI class
BH1750FVI LightSensor;



void setup() {   
  // put your setup code here, to run once:
  
  // Start Serial communication
   Serial.begin(9600);
  Wire.begin(D6,D7);
  // Start the light sensor by turning on and initializing it.
  LightSensor.begin();
 
  // Sets the measurement time register
  // This allows for adjusting the sensitivity
  // It also allows for extension of the sensor's range.
  //LightSensor.SetMTReg(69);
  
  
  // Scales the sensitivity of the sensor by changing measurement time w/o re-scaling
  // Increasing the sensitivity accounts for something covering sensor (window)
  // Decreasing the sensitivity accounts 
  // The range in sensitivity scaling is 0.45 to 3.68.  Default is 1.00
  //LightSensor.SetSensitivity(1.00);

}

void measure() {
  LightSensor.SetMode(Continuous_H);
  delay(600);
  Serial.printf("H: %8.2f, ", LightSensor.GetLux());
  LightSensor.SetMode(Continuous_H2);
  delay(600);
  Serial.printf("H2: %8.2f, ", LightSensor.GetLux());
  LightSensor.SetMode(Continuous_L);
  delay(60);
  Serial.printf("L: %8.2f, ", LightSensor.GetLux());

}

void measureMT(uint8_t mt) {
  LightSensor.SetMTReg(mt);
  Serial.printf("MT=%3d : ", mt);
  measure();
  Serial << endl;  
}

void measureSens(float s) {
  LightSensor.SetSensitivity(s);
  Serial.printf("Sens=%4.2f : ", s);
  measure();
  Serial << endl;  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // Get the value in lux.  It returns a float that's significant to 1 or 2 digits
  LightSensor.SetSensitivity(1.00F);
  measureMT(31);
//  measureMT(33);
//  measureMT(37);
//  measureMT(40);
//  measureMT(50);
//  measureMT(60);
  measureMT(69);
  measureMT(254);  
//  measureSens(0.45F);  
//  measureSens(1.0F);  
//  measureSens(2.0F);  
//  measureSens(3.68F);  
  // Measure every second
  Serial << "--------------------------------------" << endl;
  delay(1000);
}
