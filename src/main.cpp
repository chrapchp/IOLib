#line 2 "main.cpp"
#include <Arduino.h>
#include <Streaming.h>
#include <HardwareSerial.h>
#include <ArduinoUnit.h>

#include <DA_OneWireDallasMgr.h>
#include <DA_PeristalticPump.h>
#include <DA_NonBlockingDelay.h>


#define WIRE_BUS_PIN 20 // pin
#define ONE_TEMPERATURE_PRECISION 9
#define XY_001_PIN 2


// DA_OneWireDallasMgr temperatureMgr = DA_OneWireDallasMgr(&oneWire);

DA_OneWireDallasMgr temperatureMgr = DA_OneWireDallasMgr(WIRE_BUS_PIN);
HardwareSerial     *tracePort      = &Serial;

// DA_DiscreteOutputTmr XY_001 = DA_DiscreteOutputTmr(XY_001_PIN, LOW, 1, 2);
DA_PeristalticPump XY_001 = DA_PeristalticPump(XY_001_PIN, LOW);

DA_DiscreteOutputTmr XY_002 = DA_DiscreteOutputTmr(3, HIGH, 1, 2);

bool oneshot = false;
void onHeartBeat()
{
  /*
  if (XY_001.isTimerStopped() && !oneshot)
  {
    XY_001.dispenseVolume(10);
    oneshot = true;
  }

  if (XY_002.isTimerStopped())
  {
    XY_002.start();
  }
*/
  //  else
  //  XY_001.stop();
}

// DA_On
//
DA_NonBlockingDelay KI_001 = DA_NonBlockingDelay(2000, onHeartBeat);

// DA_PeristalticPump XY_001 = DA_PeristalticPump(XY_001_PIN, HIGH );

unsigned long ctr  = 0;
unsigned long ctr2 = 0;

// current max flow of 70 ml/min in DA_PeristalticPump.h
// used for tests
test(DA_PeristalticPumpdispenseVolume1)
{

  //  Serial << "(80,120) == 1 1.75,40, 1.25 " << XY_001.dispenseVolumeOver( 80, 120 ) << endl;
//assertTrue(XY_001.dispenseVolumeOver( 80, 120 ));
assertTrue(XY_001.dispenseVolume( 90 ));
assertEqual(XY_001.getCurrentActiveDuration(),77142);
assertEqual(XY_001.getTimerMode(),DA_DiscreteOutputTmr::OneShot);
assertEqual(XY_001.getTimerState(),DA_DiscreteOutputTmr::Running);

}

test(DA_PeristalticPumpdispenseVolume2)
{

  //  Serial << "(80,120) == 1 1.75,40, 1.25 " << XY_001.dispenseVolumeOver( 80, 120 ) << endl;
//assertTrue(XY_001.dispenseVolumeOver( 80, 120 ));
assertFalse(XY_001.dispenseVolume( 0 ));

}

test(DA_PeristalticPumpdispenseVolume3)
{
  assertTrue(XY_001.dispenseVolume( 50 ));
  assertEqual(XY_001.getCurrentActiveDuration(),42857);
  assertEqual(XY_001.getTimerMode(),DA_DiscreteOutputTmr::OneShot);
  assertEqual(XY_001.getTimerState(),DA_DiscreteOutputTmr::Running);
  XY_001.stop();
  assertEqual(XY_001.getTimerState(),DA_DiscreteOutputTmr::Stopped);
}

test(DA_PeristalticPumpdispenseVolumeOver1)
{

  assertFalse(XY_001.dispenseVolumeOver( 0,10 ));
  assertFalse(XY_001.dispenseVolumeOver( 50,0 ));
  assertFalse(XY_001.dispenseVolumeOver( 1,1 ));
  assertFalse(XY_001.dispenseVolumeOver( 50,1 ));
}

test(DA_PeristalticPumpdispenseVolumeOver2)
{
  assertTrue(XY_001.dispenseVolumeOver( 5,10 ));
  assertEqual(XY_001.getTimerMode(),DA_DiscreteOutputTmr::OneShot);
  assertEqual(XY_001.getCurrentActiveDuration(),4285);
}

test(DA_PeristalticPumpdispenseVolumeOver3)
{
  assertFalse(XY_001.dispenseVolumeOver( 120,80 ));
}

test(DA_PeristalticPumpdispenseVolumeOver4)
{
  assertTrue(XY_001.dispenseVolumeOver( 120,130 ));
  assertEqual(XY_001.getTimerMode(),DA_DiscreteOutputTmr::CycleUntil);
  assertEqual(XY_001.getCurrentActiveDuration(),55384);
  assertEqual(XY_001.getCurrentInactiveDuration(),1083);

}

test(DA_PeristalticPumpdispenseVolumeEvery1)
{
  assertFalse(XY_001.dispenseVolumeEvery( 0,10 ));
  assertFalse(XY_001.dispenseVolumeEvery( 50,0 ));
  assertFalse(XY_001.dispenseVolumeEvery( 1,10 ));
  assertFalse(XY_001.dispenseVolumeEvery( 70,50 ));
  assertFalse(XY_001.dispenseVolumeEvery( 35,29 ));
}

test(DA_PeristalticPumpdispenseVolumeEvery2)
{
  assertTrue(XY_001.dispenseVolumeEvery(10, 30));
  assertEqual(XY_001.getTimerMode(),DA_DiscreteOutputTmr::CycleEvery);
  assertEqual(XY_001.getCurrentActiveDuration(),8571);
  assertEqual(XY_001.getCurrentInactiveDuration(),30000);

}

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

  // XY_001.setTimerMode(DA_DiscreteOutputTmr::OneShot);
  // XY_001.stop();
  // XY_001.stop();
  // XY_002.stop();
  // XY_001.stop();
  // XY_002.stop();
    XY_001.serialize(tracePort, true);
  //  XY_002.serialize(tracePort, true);
  //  Serial << "Delaying...";
  // delay(2000);
  //  Serial << "starting..." << endl;
  //  XY_001.start();
  //  XY_002.start();
  // XY_002.setStartDefault( false );
//  Test::exclude("*");
//  Test::include("*VolumeEvery*");

}

void loop() {
  // temperatureMgr.refresh();
//  ctr++;
//  KI_001.refresh();
//  XY_001.refresh();
//  XY_002.refresh();
//    Test::exclude("*");

  Test::run();
}
