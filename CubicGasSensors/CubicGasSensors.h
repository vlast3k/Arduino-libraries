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
#include <Arduino.h>



//#ifndef SERIAL_PORT
#define SERIAL_PORT Serial
//#endif

#define NDIR_READ_TIMEOUT 10000L
#define CM1106 'C'
#define CM1102 'B'
enum CubicStatus {
    CB_INIT, CB_WARMUP, CB_CALIBRATE, CB_STARTED
};

typedef void (*CubicStatusCb)(CubicStatus);
// library interface description
class CubicGasSensors
{
  // user-accessible "public" interface
  public:
    CubicGasSensors(CubicStatusCb _cb, uint16_t _eepromReset, uint8_t _rx=100, uint8_t _tx=100 ) ;
    bool init(bool DEBUG, int8_t *skipGPIOs = NULL);
    int getCO2(boolean dbg=false);
    void printDebugInfo();
    int rawReadCM1106_CO2(bool dbg=false);
    int getSWVersion(bool dbg=false);
    boolean hasStarted() {
        return startedCO2Monitoring;
    }
  private:
    bool isInSkippedList(int8_t *list, uint8_t gpio);
    RunningAverage raCM1106;//(2);
    uint32_t lastNDIRRead = -100000L;
    uint8_t rx, tx;
    uint16_t eepromReset;
    boolean startedCO2Monitoring = false;
    boolean DEBUG = false;
    CubicStatusCb statusCb;
    CubicStatus currentStatus = CB_INIT;
    bool sentResetCmd = false;
    //uint8_t ports[4][2] ={{5,12},{12,5},{14,13},{13,14}};
    uint8_t ports[2][2] ={ {D7, D6}, {D6, D7}};
    uint8_t sensorType = CM1106;

    void setStatus(CubicStatus newStatus);
    byte getCS(byte* buf);
    boolean validateCS(byte* resp);
    boolean sendCmd(uint8_t *cmd, uint8_t *resp);
    void co2Set400ppm();
    bool checkForReset();
    void dump(uint8_t *r);
    boolean timePassed(unsigned long since, unsigned long interval) {
       return (unsigned long)(millis() - since) > interval;
    }
};

#endif
