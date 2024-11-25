/**
 *  @file    HOASwitch.h
 *  @author  peter c
 *  @date    10/6/2017
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Hand-Off-Auto Switch

 */


#ifndef DA_HOASwitch_h
#define DA_HOASwitch_h
#include <Stream.h>
#include "DA_DiscreteInput.h"

// HOA Switch - physical only
//            - virtual (soft) Only (HOA switch that is soft e.g. remote)
//            - hybrid (can be both, requires local/remote control set
// accordingingly)
class DA_HOASwitch {
public:

  enum HOASwitchType { physical, soft, hybrid };
  enum HOADetectType  { Unknown, Hand, Off, Auto  };
  DA_HOASwitch(uint8_t aHandPin,
               uint8_t aOffPin,
               uint8_t aAutoPin);
  DA_HOASwitch();


  void         setRemoteControl(bool aMode);

  inline  void setSwitchType(HOASwitchType aType)  __attribute__((always_inline))
  {
    switchType = aType;
  }

  /**
   * Change the HOA state of the HOA switch if is either a hybrid or soft
   * @param  aType  Hand, Off , Auto
   * @return       true if state change is valid and changed state occured
   */
  bool  setRemoteState(HOADetectType aType);
  bool  setRemoteState(uint16_t aType);


  inline  bool isRemoteControl()  __attribute__((always_inline))
  {
    return remoteActive;
  }

  inline  HOASwitchType getSwitchType()  __attribute__((always_inline))
  {
    return switchType;
  }

  void setOnStateChangeDetect(void (*callBack)(HOADetectType aState));

  // HOA
  // Auto switch  Off Switch   STATE       ACTION
  // LOW                High       Auto      Enable Timer
  // HIGH               LOW        Hand      Disable Timer, Force Fan on
  // HIGH               HIGH       Off       Disable Timer
  // otherwise do nothing
  void          refresh();
  HOADetectType getCurrentState();
  void          serialize(Stream *aOutputStream,
                          bool    includeCR);

protected:

private:

  DA_DiscreteInput autoSwitch;
  DA_DiscreteInput handSwitch;
  DA_DiscreteInput offSwitch;
  HOASwitchType switchType = physical;
  HOADetectType localState = Unknown;
  HOADetectType remoteState = Unknown;
  HOADetectType pendingRemoteState = Unknown;
  void (*onStateChangeDetect)(HOADetectType aState) = NULL;
  bool invokeCallBack = false;
  bool remoteActive   = false; // false = local panel control
  

  bool        doPhysicalHOA();
  bool        doVirtualHOA();


  inline bool stateChanged(HOADetectType aPendingState)
  {
    return localState != aPendingState;
  }

  inline bool remoteStateChanged(HOADetectType aPendingState)
  {
    return pendingRemoteState != aPendingState;
  }

  inline void changeLocalState(HOADetectType aToState)
  {
    localState = aToState;

    // invokeCallBack = true;
  }
};

#endif // ifndef DA_HOASwitch_h
