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
enum TimerMode { Continuous=0, OneShot };
enum TimerState {Stopped=0, Running, Paused};
// active state if Low - during ontime, output will be low, otherwise it will
// be set high, and visa
// versa
DA_DiscreteOutputTmr(uint8_t aPin,
                     bool aActiveState,
                     unsigned int aActiveDurationInSec,
                     unsigned int aInactiveDurationInSec);
bool          setActiveDuration(unsigned int aActiveDurationInSec);     // true if ok,
// false if time
// was zero
bool          setInactiveDuration(unsigned int aInactiveDurationInSec);   // true if ok,
// false if time
// was zero
void          restart();                                        // start from
                                                                // the begining
void          serialize(HardwareSerial *tracePort,
                        bool includeCR);
void          setStartDefault(bool aStartMode);                 // true = begin
                                                                // with active
                                                                // cycle, false
                                                                // beging with
                                                                // inactive cycle,
                                                                // true default
bool          refresh();                                        // update timer
                                                                // status true
                                                                // if ok, false
                                                                // if time was
                                                                // zero,
                                                                // bypassed if
                                                                // disabled
unsigned long getCurrentInactiveDuration();
unsigned long getCurrentActiveDuration();
void start();    // start timer
void stop();     // stop timer

// pseudo timer
void          pauseTimer();
void          resetTimer();
void          resumeTimer();

inline bool   isTimerActiveState() {
        return activeTimerStatus == true;
}

inline void runActiveStateTimer() {
        activeTimerStatus = true;
        DA_DiscreteOutput::activate();
}

inline void runInactiveStateTimer() {
        activeTimerStatus = false;
        reset();
}


inline bool isOneShot() {
        return timerMode == OneShot;
}

inline void setTimerMode(TimerMode aMode)
{
        timerMode = aMode;
}

inline bool isTimerRunning() {
        return timerState == Running;
}

inline bool isTimerStopped() {
        return timerState == Stopped;
}


inline bool isTimerPaused()
{
        return timerState == Paused;
}
protected:

private:

TimerMode timerMode = Continuous;
TimerState timerState = Stopped;
unsigned long activeDurationInMilliSec  = 0;   // 0 is invalid
unsigned long inactiveDurationInMilliSec = 0;   // 0 is invalid
//unsigned long pausedDuration        = 0;   // pause time in millis
//unsigned long pausedStart           = 0;   // millis on pause

bool startMode                  = true;    // when first run, output will go
                                           // active
unsigned long previousEpoch     = 0;
unsigned long timerPreset       = 0;
unsigned long timerCurrentValue = 0;
bool firstRun                   = true;
bool activeTimerStatus              = false;

};

#endif // ifndef DA_DISCRETE_OUTPUT_TMR_H
