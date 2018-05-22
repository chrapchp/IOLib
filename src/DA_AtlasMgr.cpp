/**
 *  @file    DA_AtlasMgr.cpp
 *  @author  peter c
 *  @date    05/15/2018
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Atlas Scientific Sensor Manager
 *
 *
 */
#include <Streaming.h>
#include "DA_AtlasMgr.h"


DA_AtlasMgr::DA_AtlasMgr(Stream& s, uint8_t as1, uint8_t as2,
                         uint8_t as3) : DA_Input(IO_TYPE::atlas), serialPort(s)
{
  // serialPort = s;
  // serialPort = s;
  s1 = as1;
  s2 = as2;
  s3 = as3;
  setPollingInterval(DA_ATLAS_POLL_INTERVAL);
  state          = IDLE;
  currentChannel = 0;
  prevSamplingRate = 0;
  atlasSensors[ DA_ATLAS_EC ].type = 'E';
  atlasSensors[ DA_ATLAS_ORB ].type = 'O';
  atlasSensors[ DA_ATLAS_PH ].type = 'P';
  atlasSensors[ DA_ATLAS_RTD ].type = 'R';
  atlasSensors[ DA_ATLAS_DO ].type = 'D';

}

void DA_AtlasMgr::init()
{
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  refreshAll();
}

void DA_AtlasMgr::openChannel(uint8_t aChannel)
{
  digitalWrite(s1, bitRead(aChannel, 0));
  digitalWrite(s2, bitRead(aChannel, 1));
  digitalWrite(s3, bitRead(aChannel, 2));
}

uint8_t DA_AtlasMgr::getNextProbeChannel()
{
  if (++currentChannel > DA_ATLAS_MAX_CHANNELS - 1) currentChannel = 0;
  return currentChannel;
}

/**
 * Reads sensors in a round robin manner
 * If nothing is returned the current value set to DA_ATLAS_NOT_RESPONSE
 * Future: calibration and other core functionality
 */
void DA_AtlasMgr::onRefresh()
{
  if (!isEnabled()) return;

  switch (state)
  {
  case IDLE:
    sendRaw(getNextProbeChannel(), "R", rxBuffer, 0);
    state = READING;
    prevSamplingRate = getPollingInterval();
    setPollingInterval(800); // time to wait for sensor to respond
    break;

  case READING:
    setPollingInterval(prevSamplingRate);

    if (readRaw(rxBuffer, 13, ' '))
    {
      atlasSensors[currentChannel].cachedValue =  atof(strtok(rxBuffer, " "));
    }
    else atlasSensors[currentChannel].cachedValue = DA_ATLAS_NO_RESPONSE;
    state = IDLE;
    break;
  }
}

bool DA_AtlasMgr::readRaw(char *aBuffer, uint8_t aSearchChar, uint8_t aRepChar)
{
  bool hasContent = false;

  while (serialPort.available())
  {
    *aBuffer = serialPort.read();

    if (*aBuffer == aSearchChar) *aBuffer = aRepChar;
    aBuffer++;
    hasContent = true;
  }

  *aBuffer = 0;
  return hasContent;
}

/**
 * Send a raw command as is to the given sensor on aChannel
 * This will block and wait 600 ms for response. It will
 * impact timing based code so to be used for diagnostics scenarios
 * or conditions where timing errors are ok
 * Two messages are expected data<CR>*OK<CR>
 *
 * @param aChannel 0-5 (no checks are made)
 * @param aCommand atlas sensor command string
 */
void DA_AtlasMgr::sendRaw(uint8_t     aChannel,
                          const char *aCommand,
                          char       *aResult,
                          uint16_t    aBlockingDelay)
{
  openChannel(aChannel);
  serialPort << aCommand << endl;

  if (aBlockingDelay > 0)
  {
    delay(aBlockingDelay);
    readRaw(aResult, 13, ' ');
  }
}

char DA_AtlasMgr::getSensorType( uint8_t aChannel )
{
  return   atlasSensors[aChannel].type;
}

float DA_AtlasMgr::getCachedValue( uint8_t aChannel )
{
  return  atlasSensors[aChannel].cachedValue;
}

void DA_AtlasMgr::serialize(HardwareSerial *tracePort,
                            bool            includeCR)
{
  for( int i=0; i<DA_ATLAS_MAX_CHANNELS;i++)
  {
    *tracePort << "channel:" << i << " type:" << atlasSensors[i].type << " cached:" << atlasSensors[i].cachedValue << endl;
  }
}

/**
 * Could be invoked on startup (setup)
 */
void DA_AtlasMgr::refreshAll()
{
  for( int i=0; i<DA_ATLAS_MAX_CHANNELS;i++)
  {
    sendRaw(i, "R", rxBuffer, 800);
    if(strlen(rxBuffer))
    {
      atlasSensors[i].cachedValue =  atof(strtok(rxBuffer, " "));
    }
    else atlasSensors[i].cachedValue = DA_ATLAS_NO_RESPONSE;
  }
}
