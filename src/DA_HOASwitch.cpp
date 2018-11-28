/**
 *  @file    DA_HOASwitch.h
 *  @author  peter c
 *  @date    10/6/2017
 *  @version 0.1
 *
 *
 *  @section DESCRIPTION
 *  Hand-Off-Auto Switch
 */
#include <Streaming.h>
#include "DA_HOASwitch.h"
DA_HOASwitch::DA_HOASwitch(uint8_t aHandPin, uint8_t aOffPin, uint8_t aAutoPin)
{
  autoSwitch = DA_DiscreteInput(aAutoPin, DA_DiscreteInput::ToggleDetect, true);
  autoSwitch.setPollingInterval(250);

  handSwitch = DA_DiscreteInput(aHandPin, DA_DiscreteInput::ToggleDetect, true);
  handSwitch.setPollingInterval(250);
  switchType = physical;

  // autoPin = aAutoPin ;
}

DA_HOASwitch::DA_HOASwitch() {
  switchType   = soft;
  remoteActive = true;
}

void DA_HOASwitch::setOnStateChangeDetect(void (*callBack)(HOADetectType aState))
{
  onStateChangeDetect = callBack;
}

// HOA
// Auto switch  Off Switch   STATE
// LOW                  High       Auto
// HIGH                 LOW        Hand
// HIGH                 HIGH       Off
// otherwise do nothing
bool DA_HOASwitch::doPhysicalHOA()
{
  handSwitch.refresh();
  autoSwitch.refresh();

  bool handSwitchSample = handSwitch.getSample();
  bool autoSwitchSample = autoSwitch.getSample();

  bool changedState = false;

  if (!autoSwitchSample && handSwitchSample)
  {
    if (stateChanged(Auto))
    {
      localState   = Auto;
      changedState = true;
    }
  }
  else if (autoSwitchSample && !handSwitchSample) {
    if (stateChanged(Hand))
    {
      localState   = Hand;
      changedState = true;
    }
  }
  else if (autoSwitchSample && handSwitchSample) {
    if (stateChanged(Off))
    {
      localState   = Off;
      changedState = true;
    }
  }
  return changedState;
}

bool DA_HOASwitch::doVirtualHOA()
{
  bool changedState = false;

  if (pendingRemoteState != remoteState)
  {
    remoteState  = pendingRemoteState;
    changedState = true;
  }
  return changedState;
}

void DA_HOASwitch::setRemoteControl(bool aMode)
{
  if (switchType != physical) remoteActive = aMode;
}

bool DA_HOASwitch::setRemoteState(HOADetectType aType)
{
  bool retVal = false;

  if (switchType != physical)
  {
    if (aType != getCurrentState())
    {
      pendingRemoteState = aType;
      retVal             = true;
    }
  }
  return retVal;
}

bool DA_HOASwitch::setRemoteState(uint16_t aType)
{
  bool retVal = false;

  switch (aType) {
  case 1:
    retVal = setRemoteState(Hand);
    break;

  case 2:
    retVal = setRemoteState(Off);
    break;

  case 3:
    retVal = setRemoteState(Auto);
    break;

  default:
    break;
  }
  return retVal;
}

void DA_HOASwitch::refresh()
{
  bool invokeCallBack = false;

  if (!isRemoteControl() &&
      (switchType != soft)) invokeCallBack = doPhysicalHOA();
  else invokeCallBack = doVirtualHOA();


  if (invokeCallBack && (onStateChangeDetect != NULL)) onStateChangeDetect(
      getCurrentState());
}

DA_HOASwitch::HOADetectType DA_HOASwitch::getCurrentState()
{
  HOADetectType lState = localState;

  if (isRemoteControl() && (switchType != physical)) lState = remoteState;

  return lState;
}

void DA_HOASwitch::serialize(Stream *aOutputStream, bool includeCR)
{
  *aOutputStream << "HOA:hand, auto status" << endl;
  handSwitch.serialize(aOutputStream, true);
  autoSwitch.serialize(aOutputStream, true);
  *aOutputStream << "{remoteActive:" << remoteActive << "}";
  *aOutputStream << "{switchType:" << switchType << "}";
  *aOutputStream << "{remoteState:" << remoteState << "}";
  *aOutputStream << "{pendingRemoteState:" << pendingRemoteState << "}";
  *aOutputStream << "{getCurrentState():" << getCurrentState() << "}";
  *aOutputStream << "{localState:" << localState << "}" << endl;
}
