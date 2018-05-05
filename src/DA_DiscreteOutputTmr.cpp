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
#include <Streaming.h>
#include "DA_DiscreteOutputTmr.h"
DA_DiscreteOutputTmr::DA_DiscreteOutputTmr(uint8_t      aPin,
                                           bool         aActiveState,
                                           unsigned int aActiveDurationInSec,
                                           unsigned int aInactiveDurationInSec) :
  DA_DiscreteOutput(aPin, aActiveState)
{
  activeDurationInMilliSec   = (unsigned long)aActiveDurationInSec * 1000;
  inactiveDurationInMilliSec = (unsigned long)aInactiveDurationInSec * 1000;
}

bool DA_DiscreteOutputTmr::setActiveDuration(unsigned int aActiveDurationInSec)
{
  bool retVal = false;

  if (aActiveDurationInSec > 0)
  {
    activeDurationInMilliSec = (unsigned long)aActiveDurationInSec * 1000;
    if (isTimerActiveState()) {
      timerPreset       = activeDurationInMilliSec;
      timerCurrentValue = timerPreset;
    }
    retVal = true;
  }
  return retVal;
}

bool DA_DiscreteOutputTmr::setInactiveDuration(unsigned int aInactiveDurationInSec)
{
  bool retVal = false;

  if (aInactiveDurationInSec > 0)
  {
    inactiveDurationInMilliSec = (unsigned long)aInactiveDurationInSec * 1000;

    if (!isTimerActiveState()) {
      timerPreset       = inactiveDurationInMilliSec;
      timerCurrentValue = timerPreset;
    }
    retVal = true;
  }
  return retVal;
}

void DA_DiscreteOutputTmr::restart()
{
  DA_DiscreteOutput::reset();
  firstRun = true;
}

void DA_DiscreteOutputTmr::serialize(HardwareSerial *tracePort, bool includeCR)
{
  DA_DiscreteOutput::serialize(tracePort, false);
  *tracePort << "{activeDurationInMilliSec:" <<  activeDurationInMilliSec <<
    " inactiveDurationInMilliSec:" << inactiveDurationInMilliSec <<
    " startMode:" <<
    startMode << " timerCurrentValue:" << timerCurrentValue << " timerMode:" <<
    timerMode << " timerState:" << timerState << "}";

  if (includeCR) *tracePort << endl;
}

void DA_DiscreteOutputTmr::setStartDefault(bool aStartMode)
{
  startMode = aStartMode;
}

bool DA_DiscreteOutputTmr::refresh()
{
  if (isTimerPaused() || isTimerStopped())
  {
    return true;
  }

  if (firstRun)
  {
    // force a start with active state
    if (startMode)
    {
      timerPreset = activeDurationInMilliSec;
      runActiveStateTimer();
      timerCurrentValue = timerPreset;
    }
    else
    {
      timerPreset = inactiveDurationInMilliSec;
      runInactiveStateTimer();
      timerCurrentValue = timerPreset;
    }
  }


  unsigned long currentEpoch = millis();
  unsigned long deltaT       = (firstRun) ? 0 : abs(currentEpoch - previousEpoch);

  // if (((deltaT -  pausedDuration) >= timerPreset) && !firstRun)
  if (((deltaT) >= timerPreset))
  {
    if (isTimerActiveState())
    {
      timerPreset = inactiveDurationInMilliSec;

      if (isOneShot()) stop();
      else runInactiveStateTimer();
    }
    else
    {
      timerPreset =  activeDurationInMilliSec;
      runActiveStateTimer();
    }

    previousEpoch = currentEpoch;

    // currentEpoch   = 0;
    // pausedDuration = 0;
  }

  // timerCurrentValue =  timerPreset - deltaT + pausedDuration;
  timerCurrentValue =  timerPreset - deltaT;
  firstRun          = false;
  return true;
}

unsigned long DA_DiscreteOutputTmr::getCurrentActiveDuration()
{
  if (isTimerActiveState() && !isDisabled()) return timerCurrentValue;

  return activeDurationInMilliSec;
}

unsigned long DA_DiscreteOutputTmr::getCurrentInactiveDuration()
{
  if (!isTimerActiveState() && !isDisabled()) return timerCurrentValue;

  return inactiveDurationInMilliSec;
}

void DA_DiscreteOutputTmr::pauseTimer()
{
  timerState = Paused;

  // pausedStart = millis();
}

void DA_DiscreteOutputTmr::resetTimer()
{
  DA_DiscreteOutput::reset();
}

void DA_DiscreteOutputTmr::resumeTimer()
{
  if (isTimerPaused())
  {
    timerState = Running;

    // pausedDuration = abs(millis() - pausedStart);

    if (isTimerActiveState()) DA_DiscreteOutput::activate();
    else DA_DiscreteOutput::reset();
  }
}

void DA_DiscreteOutputTmr::start()
{
  restart();
  timerState = Running;
}

void DA_DiscreteOutputTmr::stop()
{
  timerState = Stopped;
  DA_DiscreteOutput::reset();
}
