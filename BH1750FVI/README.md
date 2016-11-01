### Introduction ###
The BH1750FVI is a great little digital ambient light sensor.  It supports 16-bit output over the I2C bus with a wide range and high resolution from 1-65535 lux.  It can be configured to measure ambient light either once or continuously and with different resolutions and measurement times.

The sensor supports changing the sensitivity by adjusting the measurement time.  This allows sensitivity to be increased with a maximum sensitivity of 0.11 lux at the longest measurement times.  In addition, it allows for compensation when the sensor is behind a semi-opaque optical window.  

[Full datasheet here](http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1750fvi-e.pdf)

### Arduino Setup ###
Connect this sensor as follows:
* VCC >>> 3.3V or 5V (Most configurations are 5V tolerant and also work with 3.3V)
* SDA >>> A4 (SDA)
* SCL >>> A5 (SCL)
* addr >> Gnd or VCC
* Gnd >>> Gnd
  
### I2C Addresses ###
* Addr_LOW - 0x23 (The default) Device address when address pin LOW
* Addr_HIGH - 0x5C Device address when address pin High

   
### Sampling Modes ###
The sensor supports either continuous or one-time sampling at various resolutions.  In the continuous modes, the sensor sends data continuously over I2C, while one-time modes only sends one value and goes to sleep.  Thus, one-time modes can be more power efficient in applications where constant logging of ambient light is not needed.  Both continuous and one-time modes support various resolutions, with low resolution sampling considerably faster.  
* **Continuous_H** - (The default) 1 lx resolution (16 bit), 120ms sampling time
* **Continuous_H2** - 0.5 lx resolution (18 bit), 120ms sampling time
* **Continuous_L** - 4 lx resolution (15 bit), 16ms sampling time
* **OneTime_H** - 1 lx resolution (16 bit), 120ms sampling time
* **OneTime_H2** -  0.5 lx resolution (18 bit), 120ms sampling time
* **OneTime_L** - 4 lx resolution (15 bit), 16ms sampling time


### Class Setup and Functions ###
* **BH1750FVI LightSensor;** - Create a class instance named LightSensor
* **LightSensor.Begin(Addr_LOW, Continuous_H);** - The default for Begin() is Addr_LOW and Continuous_H
* **LightSensor.SetSensitivity(1.00);** - Changes the sensitivity of the sensor by changing the measurement time without re-scaling the resulting value.  Thus increasing the sensitivity compensates for something partially blocking light to the sensor.  The range in scaling is 0.45 to 3.68.  Default is 1.00
* **Light.SetMTReg(69);** - Sets the measurement time register, which tells the sensor how long to measure for.  SetSensitivity() uses this same process, but doesn't re-scale the result.  Using SetMTReg() results in the same value (re-scaled) but with a different resolution.  The range is 31 to 254 with 69 as the default.  At 254, resolution is 0.11 lx in _H2 modes
* **float lux = LightSensor.GetLux();** - Gets the light sensor output as a float

I've increased documentation and added more support for sensitivity and modes, but this was originally written by [Mohannad Rawawashdeh](https://github.com/Genotronex/BH1750FVI_Master)

For more information: http://www.instructables.com/id/BH1750-Digital-Light-Sensor/

