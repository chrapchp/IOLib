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
  pumpState   = Off;
  pumpMode    = Continuous;
  maxFlowRate = DA_PERISTALTICPUMP_DEFAULT_MAX_FLOW_RATE;
}

void DA_PeristalticPump::serialize(HardwareSerial *tracePort, bool includeCR)
{
  DA_DiscreteOutputTmr::serialize(tracePort, false);

  *tracePort << "{pumpState:" <<  pumpState <<
    " pumpMode:" << pumpMode;

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

bool DA_PeristalticPump::dispenseVolume(uint16_t aVolume)
{
  bool  rv = false;
  float pumpDuration;

  if (aVolume > 0)
  {
    pumpDuration = (aVolume / maxFlowRate) * 60;
    Serial << "pumpduration:" << pumpDuration << endl;
    pumpMode = Volume;
    stop();
    setTimerMode(OneShot);
    setActiveDuration((uint16_t)(round(pumpDuration)));
    start();
    rv = true;
  }
  return rv;
}

bool DA_PeristalticPump::setDurations()
{
  bool rv = false;

  return rv;
}
