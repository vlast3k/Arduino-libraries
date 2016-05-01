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


byte CubicGasSensors::getCS(byte* buf) {
  int sum=0;
  for (int i=0; i < buf[1]+2; i++) sum += buf[i];
  return (byte) 0xFF & (0x100 - sum);
}
boolean CubicGasSensors::validateCS(byte* resp) {
  if (resp[0] != 0x16) {
    SERIAL << F("bad resp") << endl;
    return false;
  }
  if (resp[resp[1]+2] != getCS(resp)) {
    dump(resp);
    SERIAL << F("bad cs:") << _HEX(getCS(resp)) << F(", ") << _HEX(resp[resp[1]+2]) << endl;  
    return false;
  }
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
  uint8_t cmdReadCO2[] = {6, 0x11, 0x03, 0x03, 1, 0x90, 0x58};
  uint8_t resp[30];
  SERIAL << sendCmd(cmdReadCO2, resp);
  SERIAL << F("done") <<endl;
  
}

void CubicGasSensors::checkForReset() {
  if (millis() > 5L*60*1000 && EEPROM.read(eepromReset) == 1) {
    EEPROM.write(eepromReset, 0);
    #ifdef ESP8266
      EEPROM.commit();
    #endif
    co2Set400ppm();
  }
}

int CubicGasSensors::rawReadCM1106_CO2() {
  uint8_t cmdReadCO2[] = {4, 0x11, 0x01, 0x01, 0xed};
  uint8_t resp[30];
  sendCmd(cmdReadCO2, resp);
  uint16_t value = ((uint16_t)256)*resp[3] + resp[4];
  if (DEBUG) SERIAL << value << endl;
  raCM1106.addValue(value);
  return value;  
}


int CubicGasSensors::getCO2(boolean dbg) {
  DEBUG = dbg;
  if (timePassed(lastNDIRRead, NDIR_READ_TIMEOUT) == false) return (int)raCM1106.getAverage();
  checkForReset();
  lastNDIRRead = millis();
  //SERIAL <<"reading\n";
  uint16_t value = rawReadCM1106_CO2();
  if (value == -1) return 0;
  else if ((millis() < 130L*1000) && (value == 550)) {
    startedCO2Monitoring = false;
    return 0;
  } else {
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
