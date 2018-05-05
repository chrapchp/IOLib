/**
 *  @file    DA_PeristalticPump.h
 *  @author  peter c
 *  @date    5/03/2018
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Control a nutruient pump (Peristaltic)
 *
 *
 */

#ifndef DA_PERISTALTICPUMP_H
#define DA_PERISTALTICPUMP_H
#include "DA_DiscreteOutputTmr.h"

#define DA_PERISTALTICPUMP_DEFAULT_ACTIVE 2
#define DA_PERISTALTICPUMP_DEFAULT_INACTIVE 30
#define DA_PERISTALTICPUMP_DEFAULT_MAX_FLOW_RATE 70 // ml per min

class DA_PeristalticPump : public DA_DiscreteOutputTmr {
public:
enum   Mode {  Continuous, Volume, VolumeOverTime  };
enum  State { Off, Dispencing };
// active state if Low - during ontime, output will be low, otherwise it will
// be set high, and visa
// versa
DA_PeristalticPump(uint8_t aPin, bool aActiveState );
void          serialize(HardwareSerial *tracePort,
                        bool includeCR);

inline State getPumpState() __attribute__((always_inline))
{
        return pumpState;
}
inline Mode getPumpMode() __attribute__((always_inline))
{
        return pumpMode;
}

inline void resetTotalizer() __attribute__((always_inline))
{
        totalizer = 0;
}

inline uint32_t getTotalizer() __attribute__((always_inline))
{
        return totalizer ;
}


bool dispenseVolume( uint16_t aVolume );

/**
 * set the max flow rate of the pump (ml per min)
 */
bool setMaxFlowRate( uint16_t aFlowRate );

protected:

private:

/**
 * for the given pumpMode, calculate the on/off durations
 */
bool setDurations();
State pumpState = Off;
Mode pumpMode = Continuous;
float maxFlowRate = DA_PERISTALTICPUMP_DEFAULT_MAX_FLOW_RATE;
uint32_t totalizer = 0;
};

#endif // ifndef DA_PERISTALTICPUMP_H
