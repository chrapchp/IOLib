/**
 *  @file    DA_AtlasMgr.h
 *  @author  peter c
 *  @date    05/15/2018
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Atlas Scientific Sensor Manager
 *  Using:https://www.atlas-scientific.com/product_pages/kits/env-sds-kit.html
 *  and 8:1 serial port exander
 *
 *  Future: Add calibration code.
 *
 */

 #ifndef DA_ATLAS_MGR_H
 #define DA_ATLAS_MGR_H

#include "DA_Input.h"
#define DA_ATLAS_MAX_SENSORS 5

#define DA_ATLAS_NO_RESPONSE -127 // or disabled

#define DA_ATLAS_TX_BUF_SZ 20
#define DA_ATLAS_RX_BUF_SZ 30

// array position based on physical channel layout
#define DA_ATLAS_EC 0
#define DA_ATLAS_ORB 1
#define DA_ATLAS_PH 2
#define DA_ATLAS_RTD 3
#define DA_ATLAS_DO 4
#define DA_ATLAS_MAX_CHANNELS 5

#define DA_ATLAS_POLL_INTERVAL 5000 // ms

struct _atlasSensor
{
  // bool enabled = true;
  float cachedValue = DA_ATLAS_NO_RESPONSE; // "cached current se
  char  type;
};

typedef _atlasSensor AtlasSensorEntry;

class DA_AtlasMgr : public DA_Input {
public:

  char txBuffer[DA_ATLAS_TX_BUF_SZ];
  char rxBuffer[DA_ATLAS_RX_BUF_SZ];

  DA_AtlasMgr(Stream& s,
              uint8_t as1,
              uint8_t as2,
              uint8_t as3);


  void init();

  bool readRaw(char   *aBuffer,
               uint8_t aSearchChar,
               uint8_t aRepChar);
  void sendRaw(uint8_t     aChannel,
               const char *aCommand,
               char       *aResult,
               uint16_t    aBlockingDelay);

  // void receiveRaw( char* aResult );
  void serialize(HardwareSerial *tracePort,
                 bool            includeCR);
 char getSensorType( uint8_t aChannel );
 float getCachedValue( uint8_t aChannel );

protected:

  void onRefresh();
  void refreshAll();

private:

  enum STATE
  {
    IDLE, READING
  };
  void    openChannel(uint8_t aChannel);
  uint8_t getNextProbeChannel();
  AtlasSensorEntry atlasSensors[DA_ATLAS_MAX_SENSORS];
  Stream& serialPort;
  uint8_t s1, s2, s3;
  STATE state;
  uint8_t currentChannel;
  uint32_t prevSamplingRate;
};
#endif // ifndef DA_ATLAS_MGR_H
