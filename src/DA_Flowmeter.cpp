/*
   DA_FlowMeter based on
   Flow meter from http://www.seeedstudio.com/wiki/G1/2_Water_Flow_sensor +/- 3%
      acuracy
   Author: Peter
   Date: 2k13
 */

#include <Streaming.h>
#include "DA_FlowMeter.h"


DA_FlowMeter::DA_FlowMeter(int aPin, int aDeltaT)
{
  mPin = aPin;
  pinMode(mPin, INPUT);
  mCurrentFlowDuration = 0;
  mDeltaT              = aDeltaT;
  mCurrentFlowRate     = 0.0;
  mPreviousFlowRate    = 0.0;
  mPulseCount          = 0;
  mPrevPulseCount      = 0;
  resetStatistics();

  // attachInterrupt (digitalPinToInterrupt(aPin), interrupHdlr, RISING);  //
  // attach interrupt handler
}

void DA_FlowMeter::begin()
{
  mPrevPulseCount = mPulseCount;
  mPulseCount     = 0;
}

void DA_FlowMeter::end()
{
  computeFlowRate();
  updateCummulativeVolume();

  updateFlowRunTime();
}

// can't rely on the mPulseCount it is cleared on every interval to calculate
// flow per unit time
unsigned int DA_FlowMeter::getCurrentPulses()
{
  return mPrevPulseCount;
}

void DA_FlowMeter::updateCummulativeVolume()
{
  mCummulativeVolume += getCurrentFlowRate();
}



void DA_FlowMeter::updateFlowRunTime()
{
  if (getCurrentFlowRate() == 0.)
  {
    if (getPreviousFlowRate() > 0.) // water flow just stopped
    {
      // mCurrentFlowDuration += mDeltaT;
      // mTotalFlowDuration += mDeltaT;


      if (mCurrentFlowDuration >
          mMaxFlowDuration) mMaxFlowDuration =  mCurrentFlowDuration;

      if ((mMinFlowDuration == 0) ||
          (mCurrentFlowDuration <
           mMinFlowDuration)) mMinFlowDuration =  mCurrentFlowDuration;

      mAverageFlowDuration = (long)mTotalFlowDuration / mFlowCounts;
      mCurrentFlowDuration = 0;
    }
  }
  else
  {
    // water flow just started (rising edge)

    if (getPreviousFlowRate()  == 0) mFlowCounts++;

    // else
    //	{
    // water still flowing after 1 epoch (deltaT)
    mCurrentFlowDuration += mDeltaT;
    mTotalFlowDuration   += mDeltaT;

    //	}
  }
}

// Pulse frequency (Hz) in Horizontal Test= 7.5Q, Q is flow rate in L/min
// Pulse Count / 7.5 / 60 = Q L/sec
float DA_FlowMeter::computeFlowRate()
{
  float errorCompensation = mDeltaT * 1000 / (millis() - mActualFlowCalcTime);

  mCurrentFlowRate = errorCompensation * mPulseCount / meterFactor;

  mPrevPulseCount     = mPulseCount;
  mActualFlowCalcTime = millis();

   mCurrentFlowRate /= 60.;

  
  return mCurrentFlowRate;
}

void DA_FlowMeter::handleFlowDetection()
{
  mPulseCount++;
}

float DA_FlowMeter::getCurrentFlowRate()
{
  return mCurrentFlowRate;
}

float DA_FlowMeter::getPreviousFlowRate()
{
  return mPreviousFlowRate;
}

float DA_FlowMeter::getCummulativeVolume()
{
  return mCummulativeVolume;
}

void DA_FlowMeter::dayRollOver()
{
  float curVolume = mCummulativeVolume;

  resetStatistics();
  mYDAYCumulativeVolume = curVolume;
}

void DA_FlowMeter::resetStatistics()
{
  mYDAYCumulativeVolume = 0;
  mCummulativeVolume    = 0.;
  mMaxFlowDuration      = 0;
  mMinFlowDuration      = 0;
  mAverageFlowDuration  = 0.;
  mTotalFlowDuration    = 0;
  mFlowCounts           = 0;
}

long DA_FlowMeter::getMaxFlowDuration()
{
  return mMaxFlowDuration;
}

long DA_FlowMeter::getMinFlowDuration()
{
  return mMinFlowDuration;
}

long DA_FlowMeter::getAverageFlowDuration()
{
  return mAverageFlowDuration;
}

long DA_FlowMeter::getTotalFlowDuration()
{
  return mTotalFlowDuration;
}

void DA_FlowMeter::setMeterFactor(float aMeterFactor)
{
  meterFactor = (aMeterFactor > 0 ? aMeterFactor : meterFactor);
}

void DA_FlowMeter::serialize(Stream *aOutputStream, bool includeCR)
{
  *aOutputStream << "{pin:" << mPin << " deltaT:" << mDeltaT << "s curFlowRate:" <<
    getCurrentFlowRate() << " L/s" ;

  *aOutputStream << " minFlowDuration:" << getMinFlowDuration() <<
    "s maxFlowDuration:" << getMaxFlowDuration();
  *aOutputStream << " s cummulativeVolume:" << getCummulativeVolume() <<
    "L totalFlowDuration:" << getTotalFlowDuration();
  *aOutputStream << " s ydayVolume:" << mYDAYCumulativeVolume << "L " <<
    "pules/sec:" << mPulseCount << "}";

  if (includeCR) *aOutputStream << endl;
}
