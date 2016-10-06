//
//    FILE: RunningAverage.h
//  AUTHOR: Rob dot Tillaart at gmail dot com
// VERSION: 0.2.08
//    DATE: 2015-apr-10
// PURPOSE: RunningAverage library for Arduino
//     URL: http://arduino.cc/playground/Main/RunningAverage
// HISTORY: See RunningAverage.cpp
//
// Released to the public domain
//
// backwards compatibility
// clr()   clear()
// add(x)  addValue(x)
// avg()   getAverage()

#ifndef RunningAverageSt_h
#define RunningAverageSt_h

#define RUNNINGAVERAGE_LIB_VERSION "0.2.08"

#include "Arduino.h"

#define RASIZE 6
class RunningAverageSt
{
public:
    RunningAverageSt(void) : _size(RASIZE){clear();}; 
    //RunningAverageSt(uint8_t);
    //~RunningAverage();

    void clear();
    void addValue(double);
    void fillValue(double, uint8_t);

    double getAverage();


    double getElement(uint8_t idx);
    uint8_t getSize() { return _size; }
    uint8_t getCount() { return _cnt; }

protected:
    uint8_t _size;
    uint8_t _cnt;
    uint8_t _idx;
    double _sum;
    double _ar[RASIZE];
};

#endif
// END OF FILE