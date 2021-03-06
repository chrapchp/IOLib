/*
   FlowMeter based on
   Flow meter from http://www.seeedstudio.com/wiki/G1/2_Water_Flow_sensor +/- 3%
      acuracy
   Author: Peter
   Date: 2k13
 */

#ifndef DA_FlowMeter_h
#define DA_FlowMeter_h

// #include "WProgram.h"
// #include <Arduino.h>  // was WProgram.h, changed to Arduino.h in 1.0
#include <Stream.h>

class DA_FlowMeter {
public:

  DA_FlowMeter(int aPin,
            int aDeltaT); // deltaT in ms

  void         begin();
  void         end();
  unsigned int getCurrentPulses();     // pulses/sec
  float        getCurrentFlowRate();   // in L/sec or L/Min
  float        getPreviousFlowRate();  // in L/sec or L/Min
  float        getCummulativeVolume(); // in L

  void         resetStatistics();
  void         dayRollOver();

  long         getMinFlowDuration();     // in seconds
  long         getMaxFlowDuration();     // in seconds
  long         getAverageFlowDuration(); // in seconds
  long         getTotalFlowDuration();
  void         handleFlowDetection();
  void         setMeterFactor( float aMeterFactor );
  void         serialize(Stream *aOutputStream,
                         bool            includeCR);

  inline float getMeterFactor() __attribute__((always_inline))
  {
    return meterFactor;
  }

  inline int getDeltaT() __attribute__((always_inline))
  {
    return mDeltaT;
  }

protected:

  float computeFlowRate();

private:

  //   init( int aPin );
  void updateCummulativeVolume();
  void updateFlowRunTime();
  float mCurrentFlowRate;          
  float mPreviousFlowRate;
  float mCummulativeVolume;
  float mYDAYCumulativeVolume;
  unsigned long mMaxFlowDuration;
  unsigned long mMinFlowDuration;
  long mAverageFlowDuration;
  int mPin;
  volatile unsigned int mPulseCount;
  unsigned int mPrevPulseCount;
  int mDeltaT;  // s
  unsigned long mActualFlowCalcTime = 0; // ms
  unsigned long mCurrentFlowDuration;
  unsigned long mTotalFlowDuration;
  long mFlowCounts;
  float meterFactor = 1.0; // default pulse / s or  min
};

#endif // ifndef FlowMeter_h
