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

// active state if Low - during ontime, output will be low, otherwise it will
// be set high, and visa
// versa
DA_PeristalticPump(uint8_t aPin, bool aActiveState );
void          serialize(HardwareSerial *tracePort,
                        bool includeCR);



inline void resetTotalizer() __attribute__((always_inline))
{
        totalizer = 0;
}

inline uint32_t getTotalizer() __attribute__((always_inline))
{
        return totalizer ;
}


bool dispenseVolume( uint16_t aVolume );
bool dispenseVolumeEvery(uint16_t aVolume, uint16_t aInterval);
bool dispenseVolumeOver(uint16_t aVolume, uint16_t aDuration );
//bool dispenseContinuous();
/**
 * set the max flow rate of the pump (ml per min)
 */
bool setMaxFlowRate( uint16_t aFlowRate );

protected:
void onOneShot();

private:
bool isValidVolumeAndDuration( uint16_t aVolume, uint16_t aDuration );

float maxFlowRate = DA_PERISTALTICPUMP_DEFAULT_MAX_FLOW_RATE;
float maxVolPerSecond = maxFlowRate / 60.;
uint32_t totalizer = 0;
};

#endif // ifndef DA_PERISTALTICPUMP_H
