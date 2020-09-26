#include "BinaryClockMux.h"
#include <Arduino.h>

// \brief constructor with parameters
// \param led1 - led6 define 6 LED pins for common anode pins via resistor to display bit 0 to 5 of seconsd, minutes and hours
// \param commonPin1 - commonPin3 define 3 pins for common cathode of the leds for seconds, minutes and hours
// \param commonLevel HIGH defines common cathode, LOW defines common anode
BinaryClockMux::BinaryClockMux(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, uint8_t bit4, uint8_t bit5, uint8_t commonSeconds, uint8_t commonMinutes, uint8_t commonHours, uint8_t commonLevel )
{
  mBinaryLEDPins[0] = bit0;
  mBinaryLEDPins[1] = bit1;
  mBinaryLEDPins[2] = bit2;
  mBinaryLEDPins[3] = bit3;
  mBinaryLEDPins[4] = bit4;
  mBinaryLEDPins[5] = bit5;
  
  mCommonLEDPins[seconds]  = commonSeconds;
  mCommonLEDPins[minutes]  = commonMinutes;
  mCommonLEDPins[hours]    = commonHours;
  
  mCommonPinsTime[seconds] = 3;
  mCommonPinsTime[minutes] = 3;
  mCommonPinsTime[hours]   = 3;
  
  mCommonLevel = commonLevel;
  mNextChange  = 0;
  mMuxIndex    = 0;
  setupPins();
}

// \brief initializes pinMode and state, should be called in method setup()
void BinaryClockMux::setupPins()
{  
  for (uint8_t i=0; i<ledPins; ++i)
  {
    pinMode(mBinaryLEDPins[i], OUTPUT);
    digitalWrite(mBinaryLEDPins[i], !mCommonLevel);
  }

  for (uint8_t i=0; i<commonPins; ++i)
  {
    pinMode(mCommonLEDPins[i], OUTPUT);
    digitalWrite(mCommonLEDPins[i], mCommonLevel);
  }
}

// \brief sets the common display time for the common pins
// \param aHoursTime time for hours pins to be displayed in ms
// \param aMinutesTime time for minutes pins to be displayed in ms
// \param aSecondsTime time for seconds pins to be displayed in ms
// \hint the sum of this times is the delay the displayLEDs(..) function causes
void BinaryClockMux::setCommonDisplayTimes(uint8_t aHoursTime, uint8_t aMinutesTime, uint8_t aSecondsTime)
{
  mCommonPinsTime[seconds] = aSecondsTime;
  mCommonPinsTime[minutes] = aMinutesTime;
  mCommonPinsTime[hours]   = aHoursTime;
}

// \brief displays the time
// \param aHours  hours to be displayed
// \param aMinutes minutes to be displayed
// \param aSeconds seconds to be displayed
void BinaryClockMux::displayLEDs(uint8_t aHours, uint8_t aMinutes, uint8_t aSeconds, unsigned long aNow)
{
  if (aNow >= mNextChange)
  {
    switch (mMuxIndex++)
    {
      case 0:
        setBinaryPinStates(aSeconds);
        showLEDs(seconds, true, aNow);
        break;
      case 1:
        showLEDs(seconds, false, aNow);
        break;
      case 2:
        setBinaryPinStates(aMinutes);
        showLEDs(minutes, true, aNow);
        break;
      case 3:
        showLEDs(minutes, false, aNow);
        break;
      case 4:
        setBinaryPinStates(aHours);
        showLEDs(hours, true, aNow);
        break;
      case 5:
        showLEDs(hours, false, aNow);
        break;
      default:
        mMuxIndex = 0;
        break;        
    }
  }
}

void BinaryClockMux::setBinaryPinStates(uint8_t aBits)
{
  for (uint8_t i=0; i<ledPins; ++i)
  {
    if (mCommonLevel)
    {
      digitalWrite(mBinaryLEDPins[i], bitRead(aBits, i));
    }
    else
    {
      digitalWrite(mBinaryLEDPins[i], !bitRead(aBits, i));
    }
    
  }
}

void BinaryClockMux::showLEDs(uint8_t aType, bool aOn, unsigned long aNow)
{
  digitalWrite(mCommonLEDPins[aType], aOn ? !mCommonLevel : mCommonLevel);
  mNextChange = aNow + (aOn ? mCommonPinsTime[aType] : 0);
}

