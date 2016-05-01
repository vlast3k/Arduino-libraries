/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef CubicGasSensors_h
#define CubicGasSensors_h
#include <RunningAverage.h>
#include <Streaming.h>
#include <EEPROM.h>



//#ifndef SERIAL
#define SERIAL Serial
//#endif

#define NDIR_READ_TIMEOUT 10000L

// library interface description
class CubicGasSensors
{
  // user-accessible "public" interface
  public:
    CubicGasSensors(uint8_t _rx, uint8_t _tx, uint16_t _eepromReset) : rx(_rx), tx(_tx), eepromReset(_eepromReset), raCM1106(2) {}
    int getCO2(boolean dbg=false);
    void printDebugInfo();
    int rawReadCM1106_CO2();
    boolean hasStarted() {
        return startedCO2Monitoring;
    }
  private:
    RunningAverage raCM1106;//(2);
    uint32_t lastNDIRRead = 0;
    uint8_t rx, tx;
    uint16_t eepromReset;
    boolean startedCO2Monitoring = false;
    boolean DEBUG = false;

    byte getCS(byte* buf);
    boolean validateCS(byte* resp);
    boolean sendCmd(uint8_t *cmd, uint8_t *resp);
    void co2Set400ppm();
    void checkForReset();
    void dump(uint8_t *r);
    boolean timePassed(unsigned long since, unsigned long interval) {
       return (unsigned long)(millis() - since) > interval;
    }
};

#endif

