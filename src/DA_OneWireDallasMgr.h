/**
 *  @file    DA_OneWireDallasMgr.h
 *  @author  peter c
 *  @date    2017MNar11
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Manage the polling and caching of DallasTemperature one wire
 * temperatureSensors
 *
 */

 #ifndef DA_ONEWIREDALLASMGR_H
 #define DA_ONEWIREDALLASMGR_H
 #include <DA_Input.h>
 #include <OneWire.h>
 #include <DallasTemperature.h>
#include <HardwareSerial.h>

#define DA_READ_WAIT_TIME 900 // time to wait for conversiont to complete in ms
#define DA_MAX_ONE_WIRE_SENSORS 7

// default temperature when not enabled
// -127 reserved for one wh
#define DA_NO_TEMPERATURE -126

#define DA_ONEWIRE_BUS_PIN 4 // pin
#define ONE_TEMPERATURE_PRECISION 9


struct _oneWireEntry
{
  union
  {
    DeviceAddress address;
    uint64_t      val;
  };
  bool    enabled = false;
  bool    crcErr  = false;
  float   curTemperature; // "cached temperature"
  uint8_t idx;
};

typedef _oneWireEntry OneWireEntry;


class DA_OneWireDallasMgr : public DA_Input {
public:

  // DA_OneWireDallasMgr(OneWire *aBus);
  DA_OneWireDallasMgr(int aPin);

  void    enableSensor(uint8_t anIndex);
  void    disableSensor(uint8_t anIndex);
  bool    isEnabled(uint8_t anIndex);
  uint8_t scanSensors();
  float   getTemperature(int index);
  void    serialize(HardwareSerial *tracePort,
                    bool            includeCR);
  void    init();
  void    onRefresh();
  void    setBlockingRead(bool aMode); // block on request for temperatures
  void setOnPollCallBack(void());

private:

  void (*onPoll)() = NULL;
  void serialize(HardwareSerial *tracePort,
                 DeviceAddress   anAddress);
  void readTemperatures();
  OneWireEntry temperatureSensorConfig[DA_MAX_ONE_WIRE_SENSORS];
  OneWire oneWireBus = NULL;
  DallasTemperature temperatureSensors;
  bool readPending  = false;
  bool blockingRead = true; // blocking read  default
  unsigned int savedPollingInterval;
};


#endif // ifndef DA_ONEWIREDALLASMGR_H
