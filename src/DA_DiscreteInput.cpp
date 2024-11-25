#include <Streaming.h>
#include "DA_DiscreteInput.h"

DA_DiscreteInput::DA_DiscreteInput(uint8_t aPin) : DA_Input(discrete, aPin)
{
  // pinMode(aPin, INPUT);
}

DA_DiscreteInput::DA_DiscreteInput(uint8_t                          aPin,
                                   DA_DiscreteInput::edgeDetectType aEdgeDectType,
                                   bool                             aEnableInternalPullup)
  : DA_Input(discrete, aPin)
{
  setEdgeDetectType(aEdgeDectType);

  if (aEnableInternalPullup) enableInternalPullup();
}

/*
   DA_DiscreteInput::DA_DiscreteInput(  uint8_t aPin,
      DA_DiscreteInput::edgeDetectType aEdgeDectType )
   {
   DA_DiscreteInput( aPin);
   }
 */
bool DA_DiscreteInput::getSample()
{
  return sampleDebounced;
}

bool DA_DiscreteInput::getRawSample()
{
  return currentRawSample;
}

bool DA_DiscreteInput::isInputToggled()
{
  if (currentRawSample != previousState)
  {
    previousState = currentRawSample;
    return true;
  }
  else return false;
}

void DA_DiscreteInput::doEdgeDetection()
{
  if (currentRawSample != previousState)
  {
    DA_DiscreteInput::edgeDetectType edgeDirection;
    bool invokeCallBack = false;

    if (previousState == HIGH)
    {
      edgeDirection = FallingEdgeDetect;
    }
    else
    {
      edgeDirection = RisingEdgeDetect;
    }

    previousState = currentRawSample;

    switch (edgeDectionType)
    {
    case FallingEdgeDetect:

      if (edgeDirection == FallingEdgeDetect) invokeCallBack = true;
      break;

    case RisingEdgeDetect:

      if (edgeDirection == RisingEdgeDetect) invokeCallBack = true;
      break;

    case ToggleDetect:

      invokeCallBack = true;
      break;

    default:

      // nothing
      invokeCallBack = false;
    }

    // if (     !isFirstSample && invokeCallBack && onEdgeDetect != NULL )
    if (invokeCallBack && (onEdgeDetect != NULL))
    {
      onEdgeDetect(currentRawSample, pin);
    }

    // isFirstSample = false;
  }
}

bool DA_DiscreteInput::debouncedRead()
{
  currentRawSample = digitalRead(pin);
  bool retVal = false;

  if (currentRawSample != previousDebounceRead)
  {
    // reset the debouncing timer
    //Serial << "Reseting...";
    debounceTimestamp = millis();
  }

  if ((millis() - debounceTimestamp) > debounceTime)
  {
    retVal = true;
  }
  previousDebounceRead = currentRawSample;
  return retVal;
}

void DA_DiscreteInput::onRefresh()
{
  if (debouncedRead())
  {
    sampleDebounced = getRawSample();
    //Serial << "Debounced...";
    if (onPoll != NULL)
    {
      onPoll(getSample());
    }

    if (edgeDectionType != None) doEdgeDetection();
  }
      //Serial << "Waiting...";
}

void DA_DiscreteInput::setDebounceTime(unsigned int aDebounceTime)
{
  if (aDebounceTime > 0) debounceTime = aDebounceTime;
}

void DA_DiscreteInput::setOnPollCallBack(void (*callBack)(int aValue))
{
  onPoll = callBack;
}

void DA_DiscreteInput::setOnEdgeEvent(void (*callBack)(bool aValue, int aPin))
{
  onEdgeDetect = callBack;
}

void DA_DiscreteInput::setEdgeDetectType(
  DA_DiscreteInput::edgeDetectType aEdgeDectType)
{
  edgeDectionType = aEdgeDectType;
}

void DA_DiscreteInput::enableInternalPullup() // internal pull up resitor
                                              // enabled
{
  pinMode(pin, INPUT_PULLUP);
}

#if defined(ESP8266)
void DA_DiscreteInput::enableInternalPulldown() // internal pull up resitor
                                              // enabled
{

    pinMode(pin, INPUT_PULLDOWN_16);

  //pinMode(pin, INPUT_PULLDOWN;
}
#endif 
void DA_DiscreteInput::disableInternalPullup() // default-input pin floats
{
  pinMode(pin, INPUT);
}

void DA_DiscreteInput::serialize(Stream *aOutputStream, bool includeCR)
{
  DA_Input::serialize(aOutputStream, false);

  *aOutputStream << "{currentState:" << getSample()  << " debounceTime:" <<
    debounceTime << " ms edgeDetectType:" << edgeDectionType << "}";

  if (includeCR) *aOutputStream << endl;
}
