#include <Arduino.h>
#include <Streaming.h>

#include <DA_OneWireDallasMgr.h>
#include <DA_PeristalticPump.h>
#include <DA_NonBlockingDelay.h>
#include <HardwareSerial.h>

#define WIRE_BUS_PIN 20 // pin
#define ONE_TEMPERATURE_PRECISION 9
#define XY_001_PIN 2


//DA_OneWireDallasMgr temperatureMgr = DA_OneWireDallasMgr(&oneWire);

DA_OneWireDallasMgr temperatureMgr = DA_OneWireDallasMgr(WIRE_BUS_PIN);
HardwareSerial     *tracePort      = &Serial;

//DA_DiscreteOutputTmr XY_001 = DA_DiscreteOutputTmr(XY_001_PIN, LOW, 1, 2);
DA_PeristalticPump  XY_001 = DA_PeristalticPump(XY_001_PIN, LOW);

DA_DiscreteOutputTmr XY_002 = DA_DiscreteOutputTmr(3, HIGH, 1, 2);

bool oneshot = false;
void onHeartBeat()
{

  if( XY_001.isTimerStopped() && !oneshot)
  {
  XY_001.dispenseVolume(70);
  oneshot = true;
}
if( XY_002.isTimerStopped())
{
XY_002.start();
}
//  else
//  XY_001.stop();
}

// DA_On
//
DA_NonBlockingDelay KI_001 = DA_NonBlockingDelay(2000, onHeartBeat);

// DA_PeristalticPump XY_001 = DA_PeristalticPump(XY_001_PIN, HIGH );

unsigned long ctr  = 0;
unsigned long ctr2 = 0;

void onTemperatureRead()
{
  *tracePort << "New Sample:" << ctr - ctr2 << endl;

  for (int i = 0; i < DA_MAX_ONE_WIRE_SENSORS; i++)
  {
    if (temperatureMgr.isEnabled(i)) *tracePort << "idx:" << i << "temp:" <<
      temperatureMgr.getTemperature(i) << endl;
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
  // temperatureMgr.disableSensor(2);
  //  temperatureMgr.disableSensor(3);
  //  temperatureMgr.disableSensor(4);
  //  temperatureMgr.disableSensor(5);

  temperatureMgr.serialize(tracePort, true);

  //XY_001.setTimerMode(DA_DiscreteOutputTmr::OneShot);
  // XY_001.stop();
  // XY_001.stop();
  // XY_002.stop();
  //XY_001.stop();
  //XY_002.stop();
//  XY_001.serialize(tracePort, true);
//  XY_002.serialize(tracePort, true);
//  Serial << "Delaying...";
  //delay(2000);
//  Serial << "starting..." << endl;
//  XY_001.start();
//  XY_002.start();
//XY_002.setStartDefault( false );
}

void loop() {
  //temperatureMgr.refresh();
  ctr++;
  KI_001.refresh();
  XY_001.refresh();
  XY_002.refresh();
}
