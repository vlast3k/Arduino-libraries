/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef vPWMRead_h
#define vPWMRead_h
#include <WString.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Math.h>
#include <algorithm>    // std::min
#include <Streaming.h>
// library interface description
class vPWMRead {
  public:
  volatile uint32_t lastTime = 0;
  uint32_t th = 0;
  uint32_t tl = 0;
  bool oldState = false;
  int pin;
  void (*onPWM)(uint32_t,uint32_t);
  void (*pinChanged)();
  vPWMRead(int p, void (*_onPWM)(uint32_t,uint32_t), void (*_pinChanged)() = NULL) {
    pin = p;
    onPWM = _onPWM;
    pinChanged = _pinChanged;
  }

  void start() {
    Serial << "Attaching PWM Reader: "<< pin << endl;
    lastTime = 0;
    attachInterrupt(pin, pinChanged, CHANGE);
  }

  void stop() {
    detachInterrupt(pin);
  }
  void onPinChanged() {
    uint32_t time = micros();
    uint32_t duration = time - lastTime;
    duration /= 1000;
    if (digitalRead(pin) == 0) th = duration;
    else {
      tl = duration;
      onPWM(tl, th);
    }
    lastTime = time;
  }
  void update() {
    if (digitalRead(pin) == oldState) return;
    else {
      uint32_t dur = micros() - lastTime;
      lastTime += dur;
      if (oldState) th = dur;
      else {
        tl = dur;
        //Serial << "D1: ---ime: " << tl/1000 << ","<< th/1000 << endl;
        onPWM(tl/1000, th/1000);
      }
      oldState = !oldState;
    }
  }
  void reset() {
    tl = th = 0;
    oldState = digitalRead(pin);
    lastTime = micros();
  }
};
#endif

