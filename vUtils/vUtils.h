/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef vUtils_h
#define vUtils_h
#include <WString.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Math.h>
#include <algorithm>    // std::min
#include <Streaming.h>
// library interface description
class vUtils
{
  // user-accessible "public" interface
  public:
static char *extractStringFromQuotes(const char* src, char *dest, int destSize) {
  
  if (!*src) {
    *dest = 0;
    return NULL;
  }
  char *p1, *p2;
  if (!(p1 = strchr(src, '\"'))) return 0;
  if (!(p2 = strchr(p1+1, '\"'))) return 0;
  int len = std::min(p2-p1, destSize);  
  strncpy(dest, p1+1, len-1);
  dest[len-1] = 0;
  return p2 + 1;
}


static void storeToEE(int address, const char *str) {
  //char tmp[30];
  //strcpy(tmp, str);
  while (*str)  EEPROM.write(address++, (byte)*(str++));
  EEPROM.write(address, 0);
  EEPROM.commit();
}

static void heap(const char * str) {
  Serial << "Heap " << str << ": " << ESP.getFreeHeap() << endl;
}
};

#endif

