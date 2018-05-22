/**
 *  @file    DA_DiscreteOutput.cpp
 *  @author  peter c
 *  @date    25/09/2017
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Simple class to represent an arduinio discrete output
 *
 *
 */
#include <Streaming.h>
#include "DA_AnalogOutput.h"
DA_AnalogOutput::DA_AnalogOutput(uint8_t aPin) : DA_Output(analog, aPin)
{
  // reset();
}

DA_AnalogOutput::DA_AnalogOutput(uint8_t aPin, uint8_t aInitialValue) : DA_Output(
    analog,
    aPin)
{
  writeAO(aInitialValue);
}

void DA_AnalogOutput::writeAO(uint8_t aValue)
{
  uint8_t value = aValue;

  if (isDisabled()) value = DA_MIN_VALUE;

  if ((value <= DA_MAX_VALUE) && (value >= DA_MIN_VALUE))
  {
    analogWrite(pin, value);
    currentValue = value;
  }
}

void DA_AnalogOutput::write(bool aValue)
{
  if (aValue) writeAO(DA_MAX_VALUE);
  else writeAO(DA_MIN_VALUE);
}

void DA_AnalogOutput::serialize(Stream *aOutputStream, bool includeCR)
{
  *aOutputStream << "{pin:" << pin <<  " isDisabled:" << isDisabled() <<
    " currentValue:" << currentValue << " }";

  if (includeCR) *aOutputStream << endl;
}
