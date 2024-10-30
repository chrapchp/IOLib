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

DA_DiscreteOutputTmr::DA_DiscreteOutputTmr(uint8_t aPin,
                                           bool aActiveState,
                                           unsigned int aActiveDurationInSec,
                                           unsigned int aInactiveDurationInSec,
                                           bool aStartActive,
                                           TimerMode aTimerMode,
                                           TimerState aTimerState) : DA_DiscreteOutput(aPin,
                                                                                       aActiveState)
{
  activeDurationInMilliSec = (unsigned long)aActiveDurationInSec * 1000ul;
  inactiveDurationInMilliSec = (unsigned long)aInactiveDurationInSec * 1000ul;
  setStartActive(aStartActive);
  timerState = aTimerState;
  timerMode = aTimerMode;
  onTimeEvent = NULL;
}

void DA_DiscreteOutputTmr::doSetActiveDuration(
    unsigned long aActiveDurationInmillis)
{
  activeDurationInMilliSec = aActiveDurationInmillis;
  if (isTimerActiveState())
  {
    timerPreset = activeDurationInMilliSec;
    timerCurrentValue = timerPreset;
  }
}

void DA_DiscreteOutputTmr::doSetInactiveDuration(unsigned long aInActiveDurationInmillis)
{
  inactiveDurationInMilliSec = aInActiveDurationInmillis;

  if (!isTimerActiveState())
  {
    timerPreset = inactiveDurationInMilliSec;
    timerCurrentValue = timerPreset;
  }
}





bool DA_DiscreteOutputTmr::setActiveDuration(uint16_t aActiveDurationInSec)
{
  bool retVal = false;

  if (aActiveDurationInSec > 0)
  {

    doSetActiveDuration((unsigned long)(aActiveDurationInSec * 1000ul));
    retVal = true;
  }
  return retVal;
}

bool DA_DiscreteOutputTmr::setInactiveDuration(uint16_t aInactiveDurationInSec)
{
  doSetInactiveDuration((unsigned long)(aInactiveDurationInSec * 1000ul));
  return true;
}

void DA_DiscreteOutputTmr::restart()
{
  DA_DiscreteOutput::reset();
  firstRun = true;
}

void DA_DiscreteOutputTmr::serialize(Stream *aOutputStream, bool includeCR)
{
  DA_DiscreteOutput::serialize(aOutputStream, false);
  *aOutputStream << "{activeDurationInMilliSec:" << activeDurationInMilliSec << " inactiveDurationInMilliSec:" << inactiveDurationInMilliSec << " startActive:" << startActive << " timerCurrentValue:" << timerCurrentValue << " timerMode:" << timerMode << " timerState:" << timerState << " activePulseTargetCount:" << activePulseTargetCount << " activePulseCount:" << activePulseCount << " activeTotalizer:" << activeTotalizer << "}";

  if (includeCR)
    *aOutputStream << endl;
}

void DA_DiscreteOutputTmr::setStartActive(bool aStartActive)
{
  startActive = aStartActive;
}

bool DA_DiscreteOutputTmr::refresh()
{
  unsigned long currentEpoch = millis();

  if (isTimerPaused() || isTimerStopped() || isTimerCycleCompleted())
  {
    return true;
  }

  if (firstRun)
  {
    previousEpoch = currentEpoch;

    // force a start with active state
    if (startActive)
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

  //  unsigned long deltaT       = (firstRun) ? 0 : abs(currentEpoch -
  // previousEpoch);
  //unsigned long deltaT = abs(currentEpoch - previousEpoch);
  
  unsigned long deltaT = currentEpoch - previousEpoch;

  if (((deltaT) >= timerPreset))
  {

    if (isTimerActiveState())
    {
      activeTotalizer += activeDurationInMilliSec;
      timerPreset = inactiveDurationInMilliSec;

      if (isOneShot())
      {
        onOneShot();
      }

      if (isCycleUntil())
      {
        if (++activePulseCount == getActivePulseTargetCount())
          onCycleCompleted();
      }
      runInactiveStateTimer();
    }
    else
    {
      timerPreset = activeDurationInMilliSec;
      runActiveStateTimer();
    }

    previousEpoch = currentEpoch;
  }

  timerCurrentValue = timerPreset - deltaT;
  firstRun = false;
  return true;
}

unsigned long DA_DiscreteOutputTmr::getCurrentActiveDuration()
{
  if (isTimerActiveState() && !isDisabled())
    return timerCurrentValue;

  return activeDurationInMilliSec;
}

unsigned long DA_DiscreteOutputTmr::getCurrentInactiveDuration()
{
  if (!isTimerActiveState() && !isDisabled())
    return timerCurrentValue;

  return inactiveDurationInMilliSec;
}

void DA_DiscreteOutputTmr::pauseTimer()
{
  timerState = Paused;
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

    if (isTimerActiveState())
      DA_DiscreteOutput::activate();
    else
      DA_DiscreteOutput::reset();
  }
}

void DA_DiscreteOutputTmr::start(TimerMode aTimerMode)
{
  timerMode = aTimerMode;
  activePulseCount = 0;
  restart();
  timerState = Running;
}

void DA_DiscreteOutputTmr::stop()
{
  timerState = Stopped;
  DA_DiscreteOutput::reset();
}

void DA_DiscreteOutputTmr::onOneShot()
{
  timerState = CycleCompleted;

  // Serial << " time active" << getActiveTotalizer() << endl;
  DA_DiscreteOutput::reset();
}

void DA_DiscreteOutputTmr::onCycleCompleted()
{
  timerState = CycleCompleted;
  //Serial << "activeTotalizer:" << activeTotalizer << endl;
  DA_DiscreteOutput::reset();
}

void DA_DiscreteOutputTmr::setOnTimeEvent(void (*callBack)(bool aValue))
{
  onTimeEvent = callBack;
}
