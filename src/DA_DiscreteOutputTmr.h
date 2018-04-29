/**
 *  @file    DA_DiscreteOutputTmr.h
 *  @author  peter c
 *  @date    4/10/2017
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Simple class to represent an arduinio discrete output with on/off duration
 *
 *
 */

#ifndef DA_DISCRETE_OUTPUT_TMR_H
#define DA_DISCRETE_OUTPUT_TMR_H
#include "DA_DiscreteOutput.h"


class DA_DiscreteOutputTmr : public DA_DiscreteOutput {
public:

  // active state if Low - during ontime, output will be low, otherwise it will
  // be set high, and visa
  // versa
  DA_DiscreteOutputTmr(uint8_t      aPin,
                       bool         aActiveState,
                       unsigned int aActiveDurationInSec,
                       unsigned int aInactiveDurationInSec);
  bool          setActiveDuration(unsigned int aActiveDurationInSec);   // true if ok,
                                                                // false if time
                                                                // was zero
  bool          setInactiveDuration(unsigned int aInactiveDurationInSec); // true if ok,
                                                                // false if time
                                                                // was zero
  void          restart();                                      // start from
                                                                // the begining
  void          serialize(HardwareSerial *tracePort,
                          bool            includeCR);
  void          setStartDefault(bool aStartMode);               // true = beging
                                                                // with on
                                                                // cycle, false
                                                                // beging with
                                                                // off cycle,
                                                                // true default
  bool          refresh();                                      // update timer
                                                                // status true
                                                                // if ok, false
                                                                // if time was
                                                                // zero,
                                                                // bypassed if
                                                                // disabled
  unsigned long getCurrentInactiveDuration();
  unsigned long getCurrentActiveDuration();

  // pseudo timer
  void          pauseTimer();
  void          resetTimer();
  void          resumeTimer();
  bool          isTimerPaused();
  inline bool   isTimerActiveState() {
    return onTimerStatus == true;
  }

  inline void runActiveStateTimer() {
    onTimerStatus = true;
  }

  inline void runInactiveStateTimer() {
    onTimerStatus = false;
  }

protected:

private:

  unsigned long activeDurationInMilliSec  = 0; // 0 is invalid
  unsigned long inactiveDurationInMilliSec = 0; // 0 is invalid
  unsigned long pausedDuration        = 0; // pause time in millis
  unsigned long pausedStart           = 0; // millis on pause

  bool startMode                  = true;  // when first run, output will go
                                           // active
  unsigned long previousEpoch     = 0;
  unsigned long timerPreset       = 0;
  unsigned long timerCurrentValue = 0;
  bool timerPaused                = false;
  bool firstRun                   = true;
  bool onTimerStatus              = false;
};

#endif // ifndef DA_DISCRETE_OUTPUT_TMR_H
