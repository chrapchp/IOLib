/**
 *  @file    DA_Output.h
 *  @author  peter c
 *  @date    09/25/2017
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Simple base class to Discrete and Analog Outputs

 */


#ifndef DA_OUTPUT_H
#define DA_OUTPUT_H
#include <Stream.h>


#include "DA_IO.h"

class DA_Output {
public:

  DA_Output(IO_TYPE aOutputType,
            uint8_t aPin);
  void         _disable();
  void         enable();
  bool         isDisabled();

  inline void setEnabled( bool aMode ) __attribute__((always_inline))
  {
    disabled = !aMode;

  }

  virtual void serialize(Stream *aOutputStream, bool includeCR) {}

protected:

  int pin;
  virtual void write(bool aValue) = 0;


  IO_TYPE outputType;


  //  virtual void doAlarmCheck() = 0;

  unsigned int rawValue = 0;

  // T scaledValue;

private:

  bool disabled = false;
};


#endif // ifndef DA_OUTPUT_H
