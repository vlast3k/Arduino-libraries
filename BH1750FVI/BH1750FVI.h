/* This library is for the Digital Light sensor BH1750FVI

  It uses the I2C Communication protocol, 
  SDA, SCL are required to interface with this sensor

  The sensor's Addr pin also needs to be HIGH or LOW
  
  Written By : Mohannad Rawashdeh
  www.genotronex.com

  Edits by James Mickley
  */
  
#ifndef BH1750FVI_h
#define BH1750FVI_h

#include "Arduino.h"
#include "Wire.h"

// I2C device addresses
#define Addr_LOW 0x23 // Device address when address pin LOW
#define Addr_HIGH 0x5C // Device address when address pin High

//All commands here are taken from Data sheet OPECODE Table page 5

// Sensor Commands
#define Power_Down 0x00
#define Power_On 0x01
#define Reset_Sensor 0x07

// Measurement Modes
#define Continuous_H  0x10 // 1 lx resolution (16 bit), 120ms sampling time
#define Continuous_H2  0x11 // 0.5 lx resolution (18 bit), 120ms sampling time
#define Continuous_L  0x13 // 4 lx resolution (15 bit), 16ms sampling time
#define OneTime_H  0x20
#define OneTime_H2  0x21
#define OneTime_L  0x23 //As well as address value

  class BH1750FVI {    

    // Public class methods/variables
    public:

      // Class constructor
      BH1750FVI(); 

      // Set the I2C address, turn on the sensor, set the mode
      bool begin(uint8_t Addr = Addr_LOW, uint8_t Mode = Continuous_H);

      // Power on the sensor
      bool PowerOn(void); 

      // Sleep the sensor
      void Sleep(void); 

      // Turn on and reset the sensor
      void Reset(void);

      // Set the sensor's I2C address & turn on Addr pin
      void SetAddress(uint8_t Addr);

      // Sets the sensor mode
      void SetMode(uint8_t Mode);

      // Sets the measurement time register
      // This allows for adjusting the sensitivity
      // It also allows for extension of the sensor's range.
      // Default is 69, range is from 31 to 254
      void SetMTReg(uint8_t MT);

      // Scales the sensitivity of the sensor by changing measurement time w/o re-scaling
      // Increasing the sensitivity accounts for something covering sensor (window)
      // Decreasing the sensitivity accounts 
      // The range in sensitivity scaling is 0.45 to 3.68.  Default is 1.00
      void SetSensitivity(float Sens);

      // Gets the value in lux from the sensor and returns a float
      // Note the float is only good to 1 or 2 digits
      float GetLux(void);
      
      float getLuxAutoScale();
    
    // Private class methods/variables
    private:

      // Write an opcode byte to the sensor
      bool I2CWrite(uint8_t Opcode);  

      // The sensor's I2C address, either 0x23 or 0x5C
      uint8_t Address;

      // The measurement time register variable
      uint8_t MTReg;
      
      // The sensitivity scaling factor
      float Sensitivity;

      // The resolution scaling factor: 0.5 for H2, 1 for H, and 4 for L
      float Resolution;

  };
  #endif