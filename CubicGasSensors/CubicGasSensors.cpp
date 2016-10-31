/*
  Test.h - Test library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/


// include this library's description file
#include "CubicGasSensors.h"
#include <Streaming.h>
#ifdef ESP8266
  #include <SoftwareSerialESP.h>
#else
  #include <SoftwareSerial.h>
#endif


CubicGasSensors::CubicGasSensors(CubicStatusCb _cb, uint16_t _eepromReset, uint8_t _rx, uint8_t _tx): rx(_rx), tx(_tx), eepromReset(_eepromReset), raCM1106(2), statusCb(_cb) {}
bool CubicGasSensors::isInSkippedList(int8_t *list, uint8_t gpio) {
  for (int i=0; i < 10 && *list > -1; list++) {
    if (*list == gpio) return true;
  }
  return false;

}
bool CubicGasSensors::init(bool DEBUG, int8_t *skipGPIOs) {
    for (int j=0; j<2; j++) {
        for (int i=0; i < 2; i++) {
            rx = ports[i][0];
            tx = ports[i][1];
            if (isInSkippedList(skipGPIOs, rx) || isInSkippedList(skipGPIOs, tx)) continue;
            if (DEBUG) SERIAL_PORT << "Trying : " << rx <<","<< tx << endl ;
            if (sensorType = getSWVersion(DEBUG)) {
                if (sensorType == CM1106) {
                    SERIAL_PORT << F("CM1106 Single Beam NDIR sensor\n");
                } else if (sensorType == CM1102) {
                    SERIAL_PORT << F("CM1102 Dual Beam NDIR sensor\n");
                } else {
                    SERIAL_PORT << F("Sensor Type not recognized\n");
                }
                return true;
            }
        }
        if (DEBUG) SERIAL_PORT << F("+");
        delay(200);
    }
    if (DEBUG) SERIAL_PORT << F("ERROR: Could not locate Cubic CO2 Sensor") << endl;
    return false;
}

int CubicGasSensors::getSWVersion(bool dbg) {
  DEBUG = dbg;
  if (dbg) SERIAL_PORT << F("Getting CO2 Sensor Software Version\n");
  uint8_t cmdReadCO2[] = {4, 0x11, 0x01, 0x1E, 0xD0};
  uint8_t resp[30];
  boolean res = sendCmd(cmdReadCO2, resp);
  if (!res) return 0;
  char version[13];
  for (int i=0; i < 12; i++) version[i] = (char) resp[i+3];
  version[12] = 0;
  SERIAL_PORT << F("CO2 Sensor SW Ver: ") << version << endl;
  if (dbg) SERIAL_PORT << F("done") <<endl;
  return version[0];

}

boolean CubicGasSensors::sendCmd(uint8_t *cmd, uint8_t *resp) {
  #ifdef ESP8266
    SoftwareSerialESP PM1106_swSer(rx, tx, 128);
  #else
    SoftwareSerial PM1106_swSer(rx, tx);
  #endif
  PM1106_swSer.begin(9600);
  PM1106_swSer.write(&cmd[1], cmd[0]);
  delay(100);
  for (int i=0; i < 24 && PM1106_swSer.available(); i++) resp[i] = PM1106_swSer.read();
  #ifndef ESP8266
  PM1106_swSer.end();
  #endif
  //PM1106_swSer.enableRx(false);
  if (DEBUG) dump(resp);
  return validateCS(resp);
}


boolean CubicGasSensors::validateCS(byte* resp) {
  if (resp[0] != 0x16) {
    //SERIAL_PORT << F("bad resp") << endl;
    return false;
  }
  if (resp[resp[1]+2] != getCS(resp)) {
    dump(resp);
    SERIAL_PORT << F("bad cs:") << _HEX(getCS(resp)) << F(", ") << _HEX(resp[resp[1]+2]) << endl;
    return false;
  }
  return true;
}


byte CubicGasSensors::getCS(byte* buf) {
  int sum=0;
  for (int i=0; i < buf[1]+2; i++) sum += buf[i];
  return (byte) 0xFF & (0x100 - sum);
}

void CubicGasSensors::co2Set400ppm() {
    SERIAL_PORT << F("Setting current CO2 level as 400 ppm\n");
  //  if (sensorType == CM1106) {
      uint8_t cmdReadCO2[] = {6, 0x11, 0x03, 0x03, 1, 0x90, 0x58};
      uint8_t resp[30];
      SERIAL_PORT << sendCmd(cmdReadCO2, resp);
    // } else {
    //     pinMode(13, OUTPUT); //d7
    //     digitalWrite(13, LOW);
    //     delay(4000); //wait for 4 seconds
    //     pinMode(13, INPUT); //release
    // }
  SERIAL_PORT << F("done") <<endl;

}




void CubicGasSensors::setStatus(CubicStatus newStatus) {
    if (currentStatus != newStatus) {
        currentStatus = newStatus;
        statusCb(currentStatus);
    }
}

#define CO2_CAL_END 300L*1000
#define CO2_CAL_ACT 240L*1000

bool CubicGasSensors::checkForReset() {
  if (EEPROM.read(eepromReset) != 1) return false;
  setStatus(CB_CALIBRATE);
  if (millis() > CO2_CAL_END) {
    EEPROM.write(eepromReset, 0);
    #ifdef ESP8266
      EEPROM.commit();
    #endif
    return false;
  } else if (!sentResetCmd && millis() > CO2_CAL_ACT) {
    co2Set400ppm();
    sentResetCmd = true;
  }
  SERIAL_PORT << F("Calibrating:") << millis()/1000 << F("s, ") << rawReadCM1106_CO2(DEBUG) << F("ppm\n");
  return true;
}

int CubicGasSensors::rawReadCM1106_CO2(bool dbg) {
  DEBUG = dbg;
  uint8_t cmdReadCO2[] = {4, 0x11, 0x01, 0x01, 0xed};
  uint8_t resp[30];
  int res = sendCmd(cmdReadCO2, resp);
  if (!res) {
      SERIAL_PORT << F("Error in communication with CO2 Sensor\n");
      return 0;
  }
  uint16_t value = ((uint16_t)256)*resp[3] + resp[4];
  if (DEBUG) SERIAL_PORT << "sss:" << value << endl;
  return value;
}


int CubicGasSensors::getCO2(boolean dbg) {
  DEBUG = dbg;
  //Serial << "11111\n";
  if (checkForReset()) return -1;
  //Serial << "22222\n";
  //if (timePassed(lastNDIRRead, NDIR_READ_TIMEOUT) == false) return (int)raCM1106.getAverage();
  //Serial << "3333\n";
  //checkForReset();
  //lastNDIRRead = millis();
  //SERIAL_PORT <<"reading\n";
  uint16_t value = rawReadCM1106_CO2(DEBUG);
  if (value == 0) return -1;
  else if ((millis() < 130L*1000) && (value == 550)) {
    setStatus(CB_WARMUP);
    startedCO2Monitoring = false;
    return -1;
  } else {
    setStatus(CB_STARTED);
    startedCO2Monitoring = true;
    raCM1106.addValue(value);
    return (int)raCM1106.getAverage();
  }
  //SERIAL_PORT << "co2: " << (int)raCM1106.getAverage() << endl;

}

void CubicGasSensors::dump(uint8_t *r) {
  for (int i=0; i<24; i++) SERIAL_PORT << _HEX(*(r++)) << ",";
  SERIAL_PORT.println();
}

void CubicGasSensors::printDebugInfo() {
  SERIAL_PORT << F("CM1106 raw: ") << rawReadCM1106_CO2() << endl;
}
