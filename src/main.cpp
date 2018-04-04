#include <Arduino.h>
#include <Streaming.h>

#include <DA_OneWireDallasMgr.h>

#include <HardwareSerial.h>

#define WIRE_BUS_PIN 20 // pin
#define ONE_TEMPERATURE_PRECISION 9


// DA_OneWireDallasMgr temperatureMgr = DA_OneWireDallasMgr(&oneWire);

DA_OneWireDallasMgr temperatureMgr = DA_OneWireDallasMgr(WIRE_BUS_PIN);
HardwareSerial     *tracePort      = &Serial;

unsigned long ctr = 0;
unsigned long ctr2 = 0;

void onTemperatureRead()
{

  *tracePort << "New Sample:" << ctr-ctr2 << endl;
  for (int i = 0; i < DA_MAX_ONE_WIRE_SENSORS; i++)
  {
    if( temperatureMgr.isEnabled( i ))
      *tracePort << "idx:" << i << "temp:" << temperatureMgr.getTemperature(i) << endl;
  }
  ctr2 = ctr;
}

void setup() {
  Serial.begin(9600);

    temperatureMgr.init();

  temperatureMgr.setPollingInterval(2000);
  temperatureMgr.setBlockingRead(false);
  temperatureMgr.setOnPollCallBack(onTemperatureRead);
  temperatureMgr.scanSensors();
// temperatureMgr.disableSensor(1);
 //temperatureMgr.disableSensor(2);
//  temperatureMgr.disableSensor(3);
//  temperatureMgr.disableSensor(4);
//  temperatureMgr.disableSensor(5);

  temperatureMgr.serialize(tracePort, true);

}


void loop() {
  temperatureMgr.refresh();
  ctr++;

}
