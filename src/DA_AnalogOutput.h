/**
 *  @file    DA_AnalogOutput.h
 *  @author  peter c
 *  @date    11/04/2018
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Simple class to represent an arduinio analog output
 *
 *
 */

#ifndef DA_ANALOG_OUTPUT_H
#define DA_ANALOG_OUTPUT_H
//#include <Arduino.h>
#include "DA_Output.h"

#define DA_MIN_VALUE 0    // minimum value for AO
#define DA_MAX_VALUE 255  // maximum value for AO


class DA_AnalogOutput : public DA_Output {
public:

  DA_AnalogOutput(uint8_t aPin);
  DA_AnalogOutput(uint8_t aPin, uint8_t aInitialValue );


  void write(bool aValue);
  void writeAO( uint8_t aValue );
  void serialize(Stream *aOutputStream,
                 bool            includeCR);



private:

  uint8_t currentValue = 0;
};

#endif
