/*
  Test.h - Test library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/


// include this library's description file
#include <CubicGasSensors.h>
#include <Streaming.h>
#ifdef ESP8266
  #include <SoftwareSerialESP.h>
#else
  #include <SoftwareSerial.h>
#endif


CubicGasSensors::CubicGasSensors(CubicStatusCb _cb, uint16_t _eepromReset, uint8_t _rx, uint8_t _tx): rx(_rx), tx(_tx), eepromReset(_eepromReset), raCM1106(2), statusCb(_cb) {}

void CubicGasSensors::init() {
    for (int j=0; j<5; j++) {
        for (int i=0; i < 4; i++) {
            rx = ports[i][0];
            tx = ports[i][1];
           // SERIAL << "Trying : " << rx <<","<< tx << endl ;
            if (sensorType = getSWVersion(DEBUG)) {
                if (sensorType == CM1106) {
                    SERIAL << F("CM1106 Single Beam NDIR sensor\n");
                } else if (sensorType == CM1102) {
                    SERIAL << F("CM1102 Dual Beam NDIR sensor\n");
                } else {
                    SERIAL << F("Sensor Type not recognized\n");
                }
                return ;
            }
        }
        SERIAL << F("+") << endl;
        delay(2000);
    }
    SERIAL << F("ERROR: Could not locate CO2 Sensor") << endl;
    return ;
}

byte CubicGasSensors::getCS(byte* buf) {
  int sum=0;
  for (int i=0; i < buf[1]+2; i++) sum += buf[i];
  return (byte) 0xFF & (0x100 - sum);
}
boolean CubicGasSensors::validateCS(byte* resp) {
  if (resp[0] != 0x16) {
    //SERIAL << F("bad resp") << endl;
    return false;
  }
  if (resp[resp[1]+2] != getCS(resp)) {
    dump(resp);
    SERIAL << F("bad cs:") << _HEX(getCS(resp)) << F(", ") << _HEX(resp[resp[1]+2]) << endl;
    return false;
  }
  return true;
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
  if (DEBUG) dump(resp);
  return validateCS(resp);
}

void CubicGasSensors::co2Set400ppm() {
    SERIAL << F("Setting current CO2 level as 400 ppm\n");
    if (sensorType == CM1106) {
      uint8_t cmdReadCO2[] = {6, 0x11, 0x03, 0x03, 1, 0x90, 0x58};
      uint8_t resp[30];
      SERIAL << sendCmd(cmdReadCO2, resp);
    } else {
        pinMode(13, OUTPUT); //d7
        digitalWrite(13, LOW);
        delay(4000); //wait for 4 seconds
        pinMode(13, INPUT); //release
    }
  SERIAL << F("done") <<endl;

}

int CubicGasSensors::getSWVersion(bool dbg) {
  DEBUG = dbg;
  if (dbg) SERIAL << F("Getting CO2 Sensor Software Version\n");
  uint8_t cmdReadCO2[] = {4, 0x11, 0x01, 0x1E, 0xD0};
  uint8_t resp[30];
  boolean res = sendCmd(cmdReadCO2, resp);
  if (!res) return 0;
  char version[13];
  for (int i=0; i < 12; i++) version[i] = (char) resp[i+3];
  version[12] = 0;
  SERIAL << F("CO2 Sensor SW Ver: ") << version << endl;
  if (dbg) SERIAL << F("done") <<endl;
  return version[0];

}


void CubicGasSensors::setStatus(CubicStatus newStatus) {
    if (currentStatus != newStatus) {
        currentStatus = newStatus;
        statusCb(currentStatus);
    }
}

bool CubicGasSensors::checkForReset() {
  if (EEPROM.read(eepromReset) != 1) return false;
  setStatus(CB_CALIBRATE);
  if (millis() > 5L*60*1000) {
    EEPROM.write(eepromReset, 0);
    #ifdef ESP8266
      EEPROM.commit();
    #endif
    return false;
  } else if (!sentResetCmd && millis() > 3L*60*1000) {
    co2Set400ppm();
    sentResetCmd = true;
  }
  SERIAL << F("Calibrating:") << millis()/1000 << F("s, ") << rawReadCM1106_CO2(DEBUG) << F("ppm\n");
  return true;
}

int CubicGasSensors::rawReadCM1106_CO2(bool dbg) {
  DEBUG = dbg;
  uint8_t cmdReadCO2[] = {4, 0x11, 0x01, 0x01, 0xed};
  uint8_t resp[30];
  int res = sendCmd(cmdReadCO2, resp);
  if (!res) {
      SERIAL << F("Error in communication with CO2 Sensor\n");
      return 0;
  }
  uint16_t value = ((uint16_t)256)*resp[3] + resp[4];
  if (DEBUG) SERIAL << "sss:" << value << endl;
  raCM1106.addValue(value);
  return value;
}


int CubicGasSensors::getCO2(boolean dbg) {
  DEBUG = dbg;
  //Serial << "11111\n";
  if (checkForReset()) return 0;
  //Serial << "22222\n";
  if (timePassed(lastNDIRRead, NDIR_READ_TIMEOUT) == false) return (int)raCM1106.getAverage();
  //Serial << "3333\n";
  //checkForReset();
  lastNDIRRead = millis();
  //SERIAL <<"reading\n";
  uint16_t value = rawReadCM1106_CO2(DEBUG);
  if (value == 0) return 0;
  else if ((millis() < 130L*1000) && (value == 550)) {
    setStatus(CB_WARMUP);
    startedCO2Monitoring = false;
    return 0;
  } else {
    setStatus(CB_STARTED);
    startedCO2Monitoring = true;
    return (int)raCM1106.getAverage();
  }
  //SERIAL << "co2: " << (int)raCM1106.getAverage() << endl;

}

void CubicGasSensors::dump(uint8_t *r) {
  for (int i=0; i<24; i++) SERIAL << _HEX(*(r++)) << ",";
  SERIAL.println();
}

void CubicGasSensors::printDebugInfo() {
  SERIAL << F("CM1106 raw: ") << rawReadCM1106_CO2() << endl;
}
