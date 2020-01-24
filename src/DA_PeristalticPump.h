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
void          serialize(Stream *aOutputStream,
                        bool includeCR);

float getDispensedVolume();

bool dispenseVolume( uint16_t aVolume );
bool dispenseVolumeEvery(uint16_t aVolume, uint16_t aInterval);
bool dispenseVolumeOver(uint16_t aVolume, uint16_t aDuration );
//bool dispenseContinuous();
/**
 * set the max flow rate of the pump (ml per min)
 */
bool setMaxFlowRate( uint16_t aFlowRate );

inline uint16_t getVolume( )
{
  return volume;
}


private:

uint16_t volume;
float maxFlowRate;
float maxVolPerSecond ;

};

#endif // ifndef DA_PERISTALTICPUMP_H
