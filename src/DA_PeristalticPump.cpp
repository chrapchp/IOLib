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
#include <math.h>
#include "DA_PeristalticPump.h"

DA_PeristalticPump::DA_PeristalticPump(uint8_t aPin,
                                       bool    aActiveState) :
  DA_DiscreteOutputTmr(aPin,
                       aActiveState,
                       DA_PERISTALTICPUMP_DEFAULT_ACTIVE,
                       DA_PERISTALTICPUMP_DEFAULT_INACTIVE)
{
  maxFlowRate = DA_PERISTALTICPUMP_DEFAULT_MAX_FLOW_RATE;
  maxVolPerSecond = maxFlowRate / 60.;
  volume = 0;
}

void DA_PeristalticPump::serialize(Stream *aOutputStream, bool includeCR)
{
  DA_DiscreteOutputTmr::serialize(aOutputStream, false);

  // *aOutputStream << "{pumpState:" <<  pumpState <<
  //  " pumpMode:" << pumpMode;

  // startMode:" <<
  //  startMode << " timerCurrentValue:" << timerCurrentValue << "}";

  if (includeCR) *aOutputStream << endl;
}

// aFlowRate in ml/min 
bool DA_PeristalticPump::setMaxFlowRate(uint16_t aFlowRate)
{
  bool rv = false;

  if (aFlowRate > 0)
  {
    maxFlowRate = aFlowRate;
    maxVolPerSecond = maxFlowRate / 60.;
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
    volume = aVolume;
    // Serial << "pumpduration:" << pumpDuration << endl;
    stop();
    setActiveDuration(pumpDuration);
    setInactiveDuration((uint16_t) 1);
    start(OneShot);
    rv = true;
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
      volume = aVolume;
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
 * Accepts duration of multiples of 60s.
 * @param  aVolume   volume in ml
 * @param  aDuration duration in secondScreen
 * @return           false if params are invalid
 */
bool DA_PeristalticPump::dispenseVolumeOver(uint16_t aVolume, uint16_t aDuration)
{
  bool rv = false;

  if (((float)aVolume >= maxVolPerSecond) && (aDuration % 60 == 0))
  {
    float activeCounts = (aDuration * maxFlowRate) / (60 * aVolume);

    // >=1 implies that this can be achieved with the given pump
    if ((activeCounts >= 1))
    {
      activeCounts = ceil(activeCounts);
      float totalActiveDuration = 60 * aVolume / maxFlowRate;
      float activeDuration      = totalActiveDuration / activeCounts;
      float inactiveDuration;

      // non-zero duration not allowed, but in this case give it something
      // small as it will behave like a oneshot.
      if (activeCounts == 1) inactiveDuration = 1;
      else inactiveDuration = (aDuration -  totalActiveDuration) /
                              (activeCounts - 1);
      volume = aVolume;
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
  }
  return rv;
}

float DA_PeristalticPump::getDispensedVolume()
{
  return getActiveTotalizer() / 1000. * maxVolPerSecond;
}
