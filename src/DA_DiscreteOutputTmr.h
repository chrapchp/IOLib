/**
 *  @file    DA_DiscreteOutputTmr.h
 *  @author  peter c
 *  @date    4/10/2017
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Simple class to represent an arduinio discrete output with on/off duration
 *  - On/Off Time with either active low or active high setting
 *  - operateates in Oneshot, continuous or until a specified time has elapsed
 *
 *
 */

#ifndef DA_DISCRETE_OUTPUT_TMR_H
#define DA_DISCRETE_OUTPUT_TMR_H
#include "DA_DiscreteOutput.h"

class DA_DiscreteOutputTmr : public DA_DiscreteOutput
{
public:
        enum TimerMode
        {
                Continuous = 0,
                OneShot,
                CycleUntil,
                CycleEvery
        };
        enum TimerState
        {
                Stopped = 0,
                Running,
                Paused,
                CycleCompleted
        };
        // active state if Low - during ontime, output will be low, otherwise it will
        // be set high, and visa
        // versa

        DA_DiscreteOutputTmr(uint8_t aPin,
                             bool aActiveState,
                             unsigned int aActiveDurationInSec,
                             unsigned int aInactiveDurationInSec,
                             bool aActiveStart = true,
                             TimerMode aTimerMode = Continuous,
                             TimerState aTimerState = Stopped);
        bool setActiveDuration(uint16_t aActiveDurationInSec);
        bool setInactiveDuration(uint16_t aInactiveDurationInSec);

        void restart(); // start from
                        // the begining
        void serialize(Stream *aOutputStream,
                       bool includeCR);
        void setStartActive(bool aStartActive); // true = begin
                                                // with active
                                                // cycle, false
                                                // beging with
                                                // inactive cycle,
                                                // true default
        bool refresh();                         // update timer
                                                // status true
                                                // if ok, false
                                                // if time was
                                                // zero,
                                                // bypassed if
                                                // disabled
        unsigned long getCurrentInactiveDuration();
        unsigned long getCurrentActiveDuration();
        void start(TimerMode aTimerMode); // start timer
        void stop();                      // stop timer

        // pseudo timer
        void pauseTimer();
        void resetTimer();
        void resumeTimer();

        inline unsigned long getActiveTotalizer()
        {
                return activeTotalizer;
        }

        inline void clearActiveTotalizer()
        {
                activeTotalizer = 0;
        }

        inline bool isTimerActiveState()
        {
                return activeTimerStatus == true;
        }

        inline void runActiveStateTimer()
        {

                activeTimerStatus = true;
                if (onTimeEvent != NULL)
                {
                        onTimeEvent(isActive());
                }
                DA_DiscreteOutput::activate();
        }

        inline void runInactiveStateTimer()
        {
                activeTimerStatus = false;
                if (onTimeEvent != NULL)
                {
                        onTimeEvent(isActive());
                }
                reset();
        }

        inline TimerMode getTimerMode()
        {
                return timerMode;
        }

        inline TimerState getTimerState()
        {
                return timerState;
        }

        inline bool isOneShot()
        {
                return timerMode == OneShot;
        }

        inline bool isCycleUntil()
        {
                return timerMode == CycleUntil;
        }
        inline void setTimerMode(TimerMode aMode)
        {
                timerMode = aMode;
        }

        inline bool isTimerRunning()
        {
                return timerState == Running;
        }

        inline bool isTimerStopped()
        {
                return timerState == Stopped;
        }

        inline bool isTimerCycleCompleted()
        {
                return timerState == CycleCompleted;
        }
        inline bool isTimerPaused()
        {
                return timerState == Paused;
        }

        inline uint32_t getActiveDuration()
        {
                return activeDurationInMilliSec;
        }

        inline uint32_t getInActiveDuration()
        {
                return inactiveDurationInMilliSec;
        }

        inline void setActivePulseTargetCount(unsigned int aActivePulseTargetCount)
        {
                activePulseTargetCount = aActivePulseTargetCount;
        }

        inline unsigned int getActivePulseTargetCount()
        {
                return activePulseTargetCount;
        }

        inline unsigned int getActivePulseCount()
        {
                return activePulseCount;
        }

        inline void setActivePulseCount(unsigned int aActivePulseCount)
        {
                activePulseCount = aActivePulseCount;
        }

        void setOnTimeEvent(void (*callBack)(bool aValue));


protected:
        virtual void onOneShot();
        virtual void onCycleCompleted();
        //bool setActiveDuration(uint16_t aActiveDurationInSec);
        //bool setInactiveDuration(uint16_t aInactiveDurationInSec);

private:
        void doSetActiveDuration(unsigned long aActiveDurationInmillis);
        void doSetInactiveDuration(unsigned long aInActiveDurationInmillis);
        TimerMode timerMode;
        TimerState timerState;
        unsigned long activeDurationInMilliSec = 0;   // 0 is invalid
        unsigned long inactiveDurationInMilliSec = 0; // 0 is invalid

        bool startActive; // when first run, output will go
        // active
        unsigned long previousEpoch = 0;
        unsigned long timerPreset = 0;
        unsigned long timerCurrentValue = 0;
        bool firstRun = true;
        bool activeTimerStatus = false;
        unsigned int activePulseTargetCount = 0;
        unsigned int activePulseCount = 0;
        unsigned long activeTotalizer = 0;
        void (*onTimeEvent)(bool state);

};

#endif // ifndef DA_DISCRETE_OUTPUT_TMR_H
