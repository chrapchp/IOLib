/**
 *  @file    DA_PeristalticPump.cpp
 *  @author  peter c
 *  @date    5/3/2018
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Implements nutrient pump control (peristalitic)
 *
 *
 */
#include <Streaming.h>
#include <Math.h>
#include "DA_PeristalticPump.h"

DA_PeristalticPump::DA_PeristalticPump(uint8_t aPin,
                                       bool    aActiveState) :
  DA_DiscreteOutputTmr(aPin,
                       aActiveState,
                       DA_PERISTALTICPUMP_DEFAULT_ACTIVE,
                       DA_PERISTALTICPUMP_DEFAULT_INACTIVE)
{
  maxFlowRate = DA_PERISTALTICPUMP_DEFAULT_MAX_FLOW_RATE;
}

void DA_PeristalticPump::serialize(HardwareSerial *tracePort, bool includeCR)
{
  DA_DiscreteOutputTmr::serialize(tracePort, false);

  // *tracePort << "{pumpState:" <<  pumpState <<
  //  " pumpMode:" << pumpMode;

  // startMode:" <<
  //  startMode << " timerCurrentValue:" << timerCurrentValue << "}";

  if (includeCR) *tracePort << endl;
}

bool DA_PeristalticPump::setMaxFlowRate(uint16_t aFlowRate)
{
  bool rv = false;

  if (aFlowRate > 0)
  {
    maxFlowRate = aFlowRate;
    rv          = true;
  }
  return rv;
}

/**
 * Dispense fluid at maximum flow rate  until the given volume has been
 * dispensed. It calculates the active duration for a DA_DiscreteOutputTmr
 * that reflects the desired volume. One can assume a 1 second setResolution
 * and anything else may not yield the desired volume. For example,
 * asking for 1 ml when the pump can only provide 1.2 ml/s.
 *
 * @param  aVolume volume in ml
 * @return         false if volume < 1
 */
bool DA_PeristalticPump::dispenseVolume(uint16_t aVolume)
{
  bool  rv = false;
  float pumpDuration;

  if ((float)aVolume >= maxVolPerSecond)
  {
    pumpDuration = (aVolume / maxFlowRate) * 60;

    // Serial << "pumpduration:" << pumpDuration << endl;
    stop();
    setActiveDuration(pumpDuration);
    start(OneShot);
    rv = true;
  }
  return rv;
}

/**
 * Checks id the combimation of volume and duration is valid
 * @param  a Volume   volume in ml
 * @param  aDuration duration in seconds
 * @return           false if params are invalid
 */
bool DA_PeristalticPump::isValidVolumeAndDuration(uint16_t aVolume,
                                                  uint16_t aDuration)
{
  bool rv = false;

  if ((aVolume > 0) && (aDuration > 0))
  {
    // check if volume request can be delivered by the pump
    if ((float)aVolume >= maxVolPerSecond)
    {
      // check for unrealistic volume/duration requests.
      // e.g. too much volume over too short of a time
      if (aVolume / (float)aDuration <= maxVolPerSecond)
      {
        float pumpDuration = (aVolume / maxFlowRate) * 60;

        // check if the calulated pump on duration exeeds the duration
        if (pumpDuration <= aDuration)
        {
          rv = true;
        }
      }
    }
  }
  return rv;
}

/**
 * Dispense volume every aInterval seconds
 * if interval is less than the time to run pump to dispence liquid
 * then this is not a valid request
 * @param  aVolume   in ml
 * @param  aInterval in seconds
 * @return           false if params are invalid
 */
bool DA_PeristalticPump::dispenseVolumeEvery(uint16_t aVolume, uint16_t aInterval)
{
  bool rv = false;

  if ((float)aVolume >= maxVolPerSecond)
  {
    float pumpDuration = (aVolume / maxFlowRate) * 60;
    // check if asking for impossible
    if (aInterval > pumpDuration)
    {
      setActiveDuration(pumpDuration);
      setInactiveDuration(aInterval);
      stop();
      start(CycleEvery);
      rv = true;
    }
  }
  return rv;
}

/**
 * Dispense a volume over a period in equal intervals.
 * The DA_DiscreteOutputTmr active and inactive times are set to
 * such that the sum of (activeTime) over the duration = the desired volume
 * If the count of active events < 1, then a regular oneshot is setup
 * @param  aVolume   volume in ml
 * @param  aDuration duration in secondScreen
 * @return           false if params are invalid
 */
bool DA_PeristalticPump::dispenseVolumeOver(uint16_t aVolume, uint16_t aDuration)
{
  bool rv = false;

  if (isValidVolumeAndDuration(aVolume, aDuration))
  {
    float activeCounts     = (aDuration * maxFlowRate) / (60 * aVolume);
    float activeDuration   = 60 * aVolume / (float)aDuration;
    float inactiveDuration = (aDuration - activeCounts * activeDuration) /
                             activeDuration;

    if ((activeCounts > 1) && (inactiveDuration > 0))
    {
      stop();
      setTimerMode(CycleUntil);
      setActiveDuration(activeDuration);
      setInactiveDuration(inactiveDuration);
      setActivePulseTargetCount(activeCounts);
      start(CycleUntil);

      // Serial << "activeCounts:" << activeCounts << " activeDuration:" <<
      //  activeDuration << " inactiveDuration:" << inactiveDuration << endl;
      rv = true;
    }
    else
    {
      // Serial << "special case oneshot" << endl;
      rv = dispenseVolume(aVolume);
    }
  }
  return rv;
}

/*
   bool DA_PeristalticPump::dispenseContinuous()
   {
   bool  rv = false;
   pumpMode = Continuous;
   }
 */
void DA_PeristalticPump::onOneShot()
{
  DA_DiscreteOutputTmr::onOneShot();

  //  pumpState = Standby;
}
