#line 2 "main.cpp"
#include <Arduino.h>
#include <Streaming.h>
#include <HardwareSerial.h>
#include <ArduinoUnit.h>

#include <DA_OneWireDallasMgr.h>
#include <DA_PeristalticPump.h>
#include <DA_NonBlockingDelay.h>
#include <DA_DiscreteInput.h>


#define WIRE_BUS_PIN 20 // pin
#define ONE_TEMPERATURE_PRECISION 9
#define XY_001_PIN 2


// DA_OneWireDallasMgr temperatureMgr = DA_OneWireDallasMgr(&oneWire);
DA_DiscreteInput dispenseVolumeInput = DA_DiscreteInput(66,
                                            DA_DiscreteInput::FallingEdgeDetect,
                                            false);

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


void on_DispenseVolume()
{
  Serial << "dispense..." << endl;
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
  //  Serial << "(80,120) == 1 1.75,40, 1.25 " << XY_001.dispenseVolumeOver( 80,
  // 120 ) << endl;
  // assertTrue(XY_001.dispenseVolumeOver( 80, 120 ));
  assertTrue(XY_001.dispenseVolume(90));
  assertEqual(XY_001.getCurrentActiveDuration(),     77142);
  assertEqual(XY_001.getTimerMode(),  DA_DiscreteOutputTmr::OneShot);
  assertEqual(XY_001.getTimerState(), DA_DiscreteOutputTmr::Running);
}

test(DA_PeristalticPumpdispenseVolume2)
{
  //  Serial << "(80,120) == 1 1.75,40, 1.25 " << XY_001.dispenseVolumeOver( 80,
  // 120 ) << endl;
  // assertTrue(XY_001.dispenseVolumeOver( 80, 120 ));
  assertFalse(XY_001.dispenseVolume(0));
  assertTrue(XY_001.dispenseVolume(12));
}

test(DA_PeristalticPumpdispenseVolume3)
{
  assertTrue(XY_001.dispenseVolume(50));
  assertEqual(XY_001.getCurrentActiveDuration(),     42857);
  assertEqual(XY_001.getTimerMode(),  DA_DiscreteOutputTmr::OneShot);
  assertEqual(XY_001.getTimerState(), DA_DiscreteOutputTmr::Running);
  XY_001.stop();
  assertEqual(XY_001.getTimerState(), DA_DiscreteOutputTmr::Stopped);
}

test(DA_PeristalticPumpdispenseVolumeOver1)
{
  assertFalse(XY_001.dispenseVolumeOver(0, 10));
  assertFalse(XY_001.dispenseVolumeOver(50, 0));
  assertFalse(XY_001.dispenseVolumeOver(115, 60));
  assertFalse(XY_001.dispenseVolumeOver(70, 61));
}

test(DA_PeristalticPumpdispenseVolumeOver2)
{
  assertTrue(XY_001.dispenseVolumeOver(70, 60));
  assertEqual(XY_001.getTimerMode(), DA_DiscreteOutputTmr::CycleUntil);
  assertEqual(XY_001.getCurrentActiveDuration(),    60000);
  assertEqual(XY_001.getCurrentInactiveDuration(),   1000);
  assertEqual(XY_001.getActivePulseTargetCount(),       1);
}

test(DA_PeristalticPumpdispenseVolumeOver3)
{
  assertTrue(XY_001.dispenseVolumeOver(115, 360));
  assertEqual(XY_001.getTimerMode(), DA_DiscreteOutputTmr::CycleUntil);
  assertEqual(XY_001.getCurrentActiveDuration(),    24642);
  assertEqual(XY_001.getCurrentInactiveDuration(),  87142);
  assertEqual(XY_001.getActivePulseTargetCount(),       4);
}

test(DA_PeristalticPumpdispenseVolumeOver4)
{
  assertTrue(XY_001.dispenseVolumeOver(170, 180));
  assertEqual(XY_001.getTimerMode(), DA_DiscreteOutputTmr::CycleUntil);
  assertEqual(XY_001.getCurrentActiveDuration(),    72857);
  assertEqual(XY_001.getCurrentInactiveDuration(),  34285);
  assertEqual(XY_001.getActivePulseTargetCount(),       2);
}

test(DA_PeristalticPumpdispenseVolumeOver5)
{
  assertTrue(XY_001.dispenseVolumeOver(50, 1800));
  assertEqual(XY_001.getTimerMode(), DA_DiscreteOutputTmr::CycleUntil);
  assertEqual(XY_001.getCurrentActiveDuration(),     1020);
  assertEqual(XY_001.getCurrentInactiveDuration(),  42857);
  assertEqual(XY_001.getActivePulseTargetCount(),      42);
}

test(DA_PeristalticPumpdispenseVolumeEvery1)
{
  assertFalse(XY_001.dispenseVolumeEvery(0, 10));
  assertFalse(XY_001.dispenseVolumeEvery(50, 0));
  assertFalse(XY_001.dispenseVolumeEvery(1, 10));
  assertFalse(XY_001.dispenseVolumeEvery(70, 50));
  assertFalse(XY_001.dispenseVolumeEvery(35, 29));
}

test(DA_PeristalticPumpdispenseVolumeEvery2)
{
  assertTrue(XY_001.dispenseVolumeEvery(10, 30));
  assertEqual(XY_001.getTimerMode(), DA_DiscreteOutputTmr::CycleEvery);
  assertEqual(XY_001.getCurrentActiveDuration(),     8571);
  assertEqual(XY_001.getCurrentInactiveDuration(),  30000);
}

class DA_PeristalticPumpOneShot : public Test {
public:

  uint32_t cur;
  uint32_t prev;
  uint16_t volume = 0;
  uint16_t duration = 0;
  DA_PeristalticPump XY_test = DA_PeristalticPump(XY_001_PIN, LOW);
  DA_PeristalticPumpOneShot(const char *name, uint16_t aVolume,
                            uint16_t aDuration)
    : Test(name)
  {
    cur  = millis();
    prev = cur;
    XY_test.clearActiveTotalizer();
    volume   = aVolume;
    duration = aDuration * 1000;
    XY_test.dispenseVolume(volume);
  }

  void loop()
  {
    if (abs(cur - prev) >= duration)
    {
      // XY_test.serialize(tracePort, true);
      assertNear(XY_test.getDispensedVolume(), (float)volume, 0.5);

      pass(); // if assertion is ok
    }
    else
    {
      cur = millis();
      XY_test.refresh();
    }
  }
};

class DA_PeristalticPumpOver : public Test {
public:

  uint32_t cur;
  uint32_t prev;
  uint16_t volume = 0;
  uint16_t duration = 0;
  DA_PeristalticPump XY_test = DA_PeristalticPump(XY_001_PIN, LOW);
  DA_PeristalticPumpOver(const char *name, uint16_t aVolume, uint16_t aDuration)
    : Test(name)
  {
    cur  = millis();
    prev = cur;
    XY_test.clearActiveTotalizer();
    volume   = aVolume;
    duration = aDuration;
    XY_test.dispenseVolumeOver(aVolume, aDuration);

      //XY_test.serialize(&Serial, true);
  }

  void loop()
  {
    if (abs(cur - prev) >= (duration + 10) * 1000)
    {
      // XY_test.serialize(tracePort, true);
      assertNear(XY_test.getDispensedVolume(), (float)volume, 0.5);

      pass(); // if assertion is ok
    }
    else
    {
      cur = millis();
      XY_test.refresh();
    }
  }
};

#ifdef UNIT_TEST
 DA_PeristalticPumpOneShot
 DA_PeristalticPumpOneShot1("DA_PeristalticPumpOneShot1", 5, 6);
 DA_PeristalticPumpOneShot
DA_PeristalticPumpOneShot2("DA_PeristalticPumpOneShot2", 12, 12);

// DA_PeristalticPumpOver DA_PeristalticPumpOver1("DA_PeristalticPumpOver1", 70, 60);
#endif

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
  dispenseVolumeInput.setOnEdgeEvent(&on_DispenseVolume);
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

  //  XY_002.serialize(tracePort, true);
  //  Serial << "Delaying...";
  // delay(2000);
  //  Serial << "starting..." << endl;
  //  XY_001.start();
  //  XY_002.start();
  // XY_002.setStartDefault( false );
  // Test::exclude("*");

   //Test::include("*DA_PeristalticPumpOneShot*");
  //  Test::include("*DA_Peristaltic*");

  // XY_001.dispenseVolumeOver(12, 5);
  // XY_001.dispenseVolume( 5 );
  //  XY_001.serialize(tracePort, true);
  //
  #ifndef UNIT_TEST
   //XY_001.dispenseVolumeOver(5,60);
   //XY_001.dispenseVolumeEvery(2, 5);
   //XY_001.serialize(tracePort, true);
  #endif
  //     XY_test.dispenseVolumeOver(aVolume, aDuration);
}

void loop() {
  // temperatureMgr.refresh();
  //  ctr++;
  //  KI_001.refresh();
  #ifndef UNIT_TEST
   //XY_001.refresh();
   dispenseVolumeInput.refresh();
  #endif
  //  XY_002.refresh();
  //    Test::exclude("*");
#ifdef UNIT_TEST
  Test::run();
  #endif
}
