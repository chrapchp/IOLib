#include <Streaming.h>
#include <DA_OneWireDallasMgr.h>
#include <Stream.h>

DA_OneWireDallasMgr::DA_OneWireDallasMgr(int aPin) : DA_Input(oneWireTemp, aPin)
{
  // OneWire oneWire(WIRE_BUS_PIN);
  oneWireBus = OneWire(aPin);

  temperatureSensors.setOneWire(&oneWireBus);
}

bool DA_OneWireDallasMgr::mapSensor(uint8_t x, uint8_t y)
{
  bool rv = false;

  if ((x >= 0) && (x < DA_MAX_ONE_WIRE_SENSORS) && (y >= 0) &&
      (y < DA_MAX_ONE_WIRE_SENSORS))
  {
    oneWireTemperatureMap[x] = y;
    rv                       = true;
  }
  return rv;
}

void DA_OneWireDallasMgr::resetMaps()
{
  for (uint8_t i=0; i < DA_MAX_ONE_WIRE_SENSORS; i++) oneWireTemperatureMap[i] = i;
}

/*
   DA_OneWireDallasMgr::DA_OneWireDallasMgr(OneWire *aBus)
   {
   oneWireBus = aBus;
   temperatureSensors.setOneWire(oneWireBus);
   }
 */
void DA_OneWireDallasMgr::init()
{
  temperatureSensors.begin();
}

void DA_OneWireDallasMgr::setBlockingRead(bool aMode)
{
  blockingRead = aMode;
  temperatureSensors.setWaitForConversion(aMode);
  savedPollingInterval = getPollingInterval();
}

void DA_OneWireDallasMgr::disableSensor(uint8_t anIndex)
{
  if ((anIndex >= 0) &&
      (anIndex <
       DA_MAX_ONE_WIRE_SENSORS)) temperatureSensorConfig[anIndex].enabled = false;
}

bool DA_OneWireDallasMgr::isEnabled(uint8_t anIndex)
{
  if ((anIndex >= 0) &&
      (anIndex <
       DA_MAX_ONE_WIRE_SENSORS)) return temperatureSensorConfig[anIndex].enabled;

  return false;
}

void DA_OneWireDallasMgr::enableSensor(uint8_t anIndex)
{
  if ((anIndex >= 0) &&
      (anIndex <
       DA_MAX_ONE_WIRE_SENSORS)) temperatureSensorConfig[anIndex].enabled = true;
}

void DA_OneWireDallasMgr::setEnabled(bool aMode, uint8_t anIndex)
{
  if (aMode) enableSensor(anIndex);
  else disableSensor(anIndex);
}

uint8_t DA_OneWireDallasMgr::scanSensors()
{
  byte addr[8];
  byte idx = 0;


  while (oneWireBus.search(addr) && idx < DA_MAX_ONE_WIRE_SENSORS)
  {
    temperatureSensorConfig[idx].idx     = idx;
    temperatureSensorConfig[idx].enabled = true;
    memcpy(temperatureSensorConfig[idx].address, addr, sizeof(addr));

    if (OneWire::crc8(temperatureSensorConfig[idx].address,
                      7) !=
        temperatureSensorConfig[idx].address[7]) temperatureSensorConfig[idx].
      crcErr =
        true;

    idx++;
  }
  oneWireBus.reset_search();
  return idx;
}

void DA_OneWireDallasMgr::serialize(Stream *aOutputStream,
                                    DeviceAddress   anAddress)
{
  for (byte i = 0; i < 8; i++) {
    *aOutputStream << "0x";

    if (anAddress[i] < 16) *aOutputStream << '0';

    *aOutputStream << _HEX(anAddress[i]);

    if (i < 7) *aOutputStream << ",";
  }

  if (OneWire::crc8(anAddress, 7) != anAddress[7]) *aOutputStream << "(CRC Invalid)";
}

void DA_OneWireDallasMgr::serialize(Stream *aOutputStream, bool includeCR)
{
  *aOutputStream << endl << "Mapping rules" << endl;
  for( uint8_t i =0; i<DA_MAX_ONE_WIRE_SENSORS; i++)
  {
    *aOutputStream << "TI_00" << i << "->" << oneWireTemperatureMap[i] << " cachedValue:" << getTemperature(i)<< endl;
  }
   *aOutputStream << endl << "Sensor Data" << endl;
  for (int i = 0; i < DA_MAX_ONE_WIRE_SENSORS; i++)
  {
    *aOutputStream << "{ index:" <<    temperatureSensorConfig[i].idx << ", address:";
    serialize(aOutputStream, temperatureSensorConfig[i].address);
    *aOutputStream << ", enabled:" <<   temperatureSensorConfig[i].enabled << " cachedValue:" << temperatureSensorConfig[i].curTemperature << "}" <<
      endl;
  }

  if (includeCR) *aOutputStream << endl;
}

float DA_OneWireDallasMgr::getTemperature(int aIndex)
{
  if (aIndex < DA_MAX_ONE_WIRE_SENSORS)
  {
    //  temperatureSensorConfig[aIndex].curTemperature =
    // temperatureSensors.getTempC(temperatureSensorConfig[aIndex].address);
    return temperatureSensorConfig[oneWireTemperatureMap[aIndex]].curTemperature;
  }
  else return DA_NO_TEMPERATURE;
}

/*
   if (!sensors.getAddress(aDevice, aIndex))
   {
   // TODO Alarm

 #ifdef PROCESS_TERMINAL
 * aOutputStream << "Unable to find address for Device at index " << aIndex <<
    "address:";
   printOneWireAddress(aOutputStream, aDevice, true);
 #endif // ifdef PROCESS_TERMINAL

   sensors.setResolution(aDevice, ONE_TEMPERATURE_PRECISION);
   }
 */
void DA_OneWireDallasMgr::readTemperatures()
{
  for (int i = 0; i < DA_MAX_ONE_WIRE_SENSORS; i++)
  {
    temperatureSensorConfig[i].
      curTemperature = DA_NO_TEMPERATURE;
    if (temperatureSensorConfig[i].enabled) temperatureSensorConfig[i].
      curTemperature = temperatureSensors.getTempC(
        temperatureSensorConfig[i].address);
  }

  if (onPoll != NULL)
  {
    onPoll();
  }
}

void DA_OneWireDallasMgr::onRefresh()
{
  // synchronous read request and wait for temps to be ready
  if (blockingRead)
  {
    temperatureSensors.requestTemperatures();
    readTemperatures();
  }

  // asynchronous read change poll rate
  // to minumum time to wait for conversion
  // when read, revert to original poll rate
  else
  {
    if (!readPending)
    {
      temperatureSensors.requestTemperatures();
      readPending = true;
      setPollingInterval(DA_READ_WAIT_TIME);
    }
    else
    {
      readTemperatures();
      readPending = false;
      setPollingInterval(savedPollingInterval);
    }
  }
}

uint64_t DA_OneWireDallasMgr::getUIID(uint8_t anIndex)
{
  if ((anIndex >= 0) &&
      (anIndex <
       DA_MAX_ONE_WIRE_SENSORS))
  {
    return temperatureSensorConfig[oneWireTemperatureMap[anIndex]].uuid;
  }
  else return 0;
}

void DA_OneWireDallasMgr::setOnPollCallBack(void (*callBack)())
{
  onPoll = callBack;
}
