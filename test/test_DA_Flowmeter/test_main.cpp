/*
   FlowMeter Unit Test - using platformIO unit testing 
   Pulse stream is simulated using pin TEST_PULSE_PIN and fed to FT_001_SENSOR_INTERUPT_PIN pin
   Author: Peter
   Date: 2k20
 */

#include <Arduino.h>
#include <Streaming.h>
#include <unity.h>
#include <DA_Flowmeter.h>
#include <DA_DiscreteOutputTmr.h>
#include <DA_NonBlockingDelay.h>
#include <DA_DiscreteOutput.h>

#define FT_001_SENSOR_INTERUPT_PIN 2
#define DELTA_T 1 // 1 second
#define ENABLE_FT001_SENSOR_INTERRUPTS attachInterrupt(digitalPinToInterrupt(           \
                                                           FT_001_SENSOR_INTERUPT_PIN), \
                                                       onFT_001_PulseIn,                \
                                                       RISING)
#define DISABLE_FT001_SENSOR_INTERRUPTS detachInterrupt(digitalPinToInterrupt( \
    FT_001_SENSOR_INTERUPT_PIN))
#define TEST_PULSE_PIN 5
#define TEST_PULSE_FREQ_25_HZ 25
DA_DiscreteOutput XY_001 = DA_DiscreteOutput(TEST_PULSE_PIN, LOW);

#define FLOW_TEST_PASSES 5 // how many flow rate tests to make

void do_Pulse();
void do_OnCalcFlowRate();

DA_FlowMeter FT_001(FT_001_SENSOR_INTERUPT_PIN, DELTA_T);

DA_NonBlockingDelay KI_002 = DA_NonBlockingDelay(DELTA_T * 1000,
                                                 do_OnCalcFlowRate);

DA_NonBlockingDelay KI_001 =
    DA_NonBlockingDelay(1000 / TEST_PULSE_FREQ_25_HZ / 2, do_Pulse);

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_meter_factor_default()
{
    TEST_ASSERT_EQUAL_FLOAT(1, FT_001.getMeterFactor());
}

void test_meter_factor()
{
    FT_001.setMeterFactor(7.52);
    TEST_ASSERT_EQUAL_FLOAT(7.52, FT_001.getMeterFactor());
}


void test_meter_deltaT()
{

    TEST_ASSERT_EQUAL(DELTA_T, FT_001.getDeltaT());
}

void test_flow_rate()
{
    TEST_ASSERT_FLOAT_WITHIN(1,TEST_PULSE_FREQ_25_HZ,  FT_001.getCurrentFlowRate());
}

void test_flow_accumulator()
{
    TEST_ASSERT_FLOAT_WITHIN(1,TEST_PULSE_FREQ_25_HZ * FLOW_TEST_PASSES, FT_001.getCummulativeVolume());
}

void test_flow_avg_duration()
{
    TEST_ASSERT_EQUAL(3, FT_001.getAverageFlowDuration());
}

void setup()
{

    pinMode(TEST_PULSE_PIN, OUTPUT);
    delay(500);
    UNITY_BEGIN();
    RUN_TEST(test_meter_factor_default);
    RUN_TEST(test_meter_factor);
    RUN_TEST(test_meter_deltaT);
    UNITY_END();
    // set up conditions for the rest of the tests
    FT_001.setMeterFactor(1.0/60.0);

}

int flowPasses = 0;
enum testState
{
    FlowTest,
    FlowBeginPause,
    FlowPaused,
    FlowResumed,
    DurationTest,
    Complete
};

enum testState state = FlowTest;

void loop()
{
    KI_001.refresh();
    KI_002.refresh();
    //*debugOutputStream << "on_InletValve_Process HS_002, LSHH_002" << endl;
}

void onFT_001_PulseIn()
{
    FT_001.handleFlowDetection();
}

void do_Pulse()
{
    //digitalWrite(TEST_PULSE_PIN, !digitalRead(TEST_PULSE_PIN));
    XY_001.toggle();
}

void doFlowTests()
{

    if (flowPasses != 0 and flowPasses < FLOW_TEST_PASSES)
        RUN_TEST(test_flow_rate);
    if (flowPasses == FLOW_TEST_PASSES)
    {
        RUN_TEST(test_flow_accumulator);
        state = Complete; //FlowBeginPause

    }
    flowPasses++;
}


void doFlowPaused()
{
    if( flowPasses > FLOW_TEST_PASSES )
        state =  FlowResumed;
    flowPasses++;   

}

void doFlowDuration()
{
    if( flowPasses > FLOW_TEST_PASSES)
    {
        state =  Complete;
        RUN_TEST(test_flow_avg_duration);
    }
    flowPasses++;   
}


void do_OnCalcFlowRate()
{
    DISABLE_FT001_SENSOR_INTERRUPTS;
    FT_001.end();
    FT_001.begin();

    ENABLE_FT001_SENSOR_INTERRUPTS;

    switch( state)
    {
        case FlowTest:
            doFlowTests();
            break;
        case FlowBeginPause:
            XY_001.disable();
            flowPasses = 0;
            state = FlowPaused;
            break;
      //  case FlowPaused:
      //      doFlowPaused();
      //      break;
      //  case FlowResumed:
      //      XY_001.enable();
      //      flowPasses = 0;
      //      state = DurationTest;
       //     break;
       // case DurationTest:
       //     doFlowDuration();
       //     break;
        case Complete:
            UNITY_END();
            break;
        default:
            break;
    }   

}
