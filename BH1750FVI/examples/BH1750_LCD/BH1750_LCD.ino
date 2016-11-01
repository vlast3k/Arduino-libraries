/*
  This is a simple sketch to test BH1750FVI Light Sensor
  which communicates using the I2C Protocol 
  
  It uses PWM to regulate the brightness of an LED based on the ambient light

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
#include <BH1750FVI.h>
#include <LiquidCrystal.h> // Vanilla LCD Library
// Using F Malpartida's NewLiquidCrystal library
// https://bitbucket.org/fmalpartida/new-liquidcrystal
//#include <LiquidCrystal_I2C.h>

// Initialize the BH1750FVI class
BH1750FVI LightSensor;

// Initialize the Liquid Crystal
// Vanilla LCD library
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Using F Malpartida's NewLiquidCrystal library
// https://bitbucket.org/fmalpartida/new-liquidcrystal
//LiquidCrystal_I2C  lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  


void setup() {
  // put your setup code here, to run once:
  
  // Start Serial communication
  Serial.begin(9600);
  
  // Start the LCD in 16x2 mode
  lcd.begin(16, 2);
  
  // Display startup sequence
  lcd.setCursor(0, 0);
  lcd.print("BH1750FVI Sensor");
  lcd.setCursor(1, 1);
  lcd.print("Please wait..."); 
  

 
  // Start the light sensor by turning on and initializing it.
  LightSensor.Begin(Addr_LOW, Continuous_H);
 
  // Sets the measurement time register
  // This allows for adjusting the sensitivity
  // It also allows for extension of the sensor's range.
  //LightSensor.SetMTReg(69);
  
  
  // Scales the sensitivity of the sensor by changing measurement time w/o re-scaling
  // Increasing the sensitivity accounts for something covering sensor (window)
  // Decreasing the sensitivity accounts 
  // The range in sensitivity scaling is 0.45 to 3.68.  Default is 1.00
  //LightSensor.SetSensitivity(1.00);

  // Show the display for 1 second
  delay(2000);

  // Clear the LCD
  lcd.clear();
  
  // Setup the LCD for measurement
  lcd.setCursor(0, 0);
  lcd.print("Light Intensity");
 
}
 
 void loop() {
  // put your main code here, to run repeatedly: 
  
  // Get the lux value from the sensor
  int lux = LightSensor.GetLux();

  // Clear the 2nd row of the lcd
  lcd.setCursor(0,1); 
  lcd.print("         ");
  lcd.setCursor(0,1); 
  
  // Display the Lux value
  lcd.print(lux);
  lcd.print(" Lux");
  
  // Delay one second between readings
  delay(1000);
   
  
}