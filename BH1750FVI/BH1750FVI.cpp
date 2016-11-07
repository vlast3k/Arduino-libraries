#include "BH1750FVI.h"
#include "Arduino.h"
#include <Streaming.h>

BH1750FVI::BH1750FVI(){  

    // Set defaults for MTReg and Sensitivity on class initiation
    MTReg = 69;
    Sensitivity = 1.00;
 }
  
  bool BH1750FVI::begin(uint8_t Addr, uint8_t Mode){

    // Set the address
    SetAddress(Addr);

    // Start the I2C bus
    Wire.begin(); 

    // Power on the sensor
    if (!PowerOn()) return false;

    // Set the mode
    SetMode(Mode);
    return true;

  }

  bool BH1750FVI::PowerOn(void){
    return I2CWrite(Power_On ); // Power on the sensor
  }

  void BH1750FVI::Sleep(void){
    I2CWrite(Power_Down ); //Turn it off , Reset operator won't work in this mode
  }

  void BH1750FVI::Reset(void){
    I2CWrite(Power_On ); //Turn it on again
    I2CWrite(Reset_Sensor ); //Reset
    
  }

  void BH1750FVI::SetAddress(uint8_t Addr){

    // Set the address
    Address=Addr;

  }

  void BH1750FVI::SetMode(uint8_t Mode){

    // Write the mode to the sensor
    I2CWrite(Mode);

    // Handle mode-specific changes
    // Note: delay for enough time to allow the sensor to get a measurement
    switch(Mode){

        case Continuous_H:
          Resolution = 1;
          delay(180);
          break;

        case Continuous_H2:
          Resolution = 0.5;
          delay(180);
          break;

        case Continuous_L:
          Resolution = 1;
          delay(24);      
          break;

        case OneTime_H:
          Resolution = 1;
          delay(180);
          break;

        case OneTime_H2:
          Resolution = 0.5;
          delay(180);
          break;

        case OneTime_L:  
          Resolution = 1;
          delay(24);
          break;
    }
  }

  void BH1750FVI::SetMTReg(uint8_t MT){

    // constrain MT to [31, 254]
    MT = min(max(MT, 31), 254);
    
    // Set the MTReg class variable so we can account for it while measuring
    MTReg = MT;
    
    // Shift the first 3 bytes of MT to the last 3
    // Then OR 64 (01000000) to add the 01000 prefix
    uint8_t highbyte = (MT >> 5) | 64;

    // Get rid of the first 3 bytes in MT by ANDing 31 (00011111)
    // Then OR 96 (01100000) to add the 011 prefix
    uint8_t lowbyte = (MT & 31) | 96;
    //Serial << "\nSet MT: " << MTReg << ", " << _BIN(highbyte) << "," << _BIN(lowbyte) << endl;;

    // Send the two MTReg bytes
    Wire.beginTransmission(Address);
    Wire.write(highbyte);
    Wire.endTransmission();
    delay(5);
    Wire.beginTransmission(Address);
    Wire.write(lowbyte);
    Wire.endTransmission();

  }

  void BH1750FVI::SetSensitivity(float Sens){

    // Convert sensitivity ratio to MT
    int MT = round(Sens * 69.0);

    // constrain MT to [31, 254]
    MT = min(max(MT, 31), 254);

    // Set the Sensitivity class variable by calculating the ratio
    // Note we use MT/69 here, which might be different from sens
    Sensitivity = (float)MT / 69.0;
   // Serial << "SetSens Sens: " << Sensitivity << ", MT = " << MT << endl;

    // Set the Measurement Time to MT
    SetMTReg(MT);

  }
  
  float BH1750FVI::GetLux(void){

    uint16_t Intensity;
    float lux;
    
    // Ask the sensor for two bytes
    Wire.beginTransmission(Address);
    Wire.requestFrom((int)Address, 2);

    // Read the two bytes and OR them together for a 16 bit number
    Intensity = Wire.read();
    Intensity <<= 8;
    Intensity |= Wire.read();

    // End the transmission
    Wire.endTransmission();

    // Check to see if we've changed the sensitivity. 
    if(Sensitivity != 1.00){

      // According to datasheet, divide by 1.2 & multiply by Resolution
      // Sensitivity has been changed, so don't re-scale
      lux = Intensity / 1.2 * Resolution;

    } else {
      Serial.printf("{%5d}", Intensity);
      // According to datasheet, divide by 1.2 & multiply by Resolution
      // Rescale to account for Measurement Time change
      lux = Intensity / 1.2 * Resolution * 69.0F / MTReg;

    }

    // Return a float.  Only to 2 significant digits though
    return ((long)(lux * 100 ))/ 100.0;
    
  }
  
  bool BH1750FVI::I2CWrite(uint8_t Opcode){

    // Write a byte to the sensor
    Wire.beginTransmission(Address);
    Wire.write(Opcode);
    return Wire.endTransmission() == 0;

  }

  float BH1750FVI::getLuxAutoScale() {
    SetSensitivity(1.00F);
    SetMTReg(31);
    SetMode(Continuous_H);
    delay(200);
    float lux = GetLux();
    if (lux < 500) {
        SetMTReg(254);
        SetMode(Continuous_H2);
        lux = GetLux();
    }
    return lux;
  }
