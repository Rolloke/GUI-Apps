#include "ClockMux.h"
#include <Arduino.h>

uint8_t ClockMux::mSegmentBits[characters] = 
{//   0,    1,    2,    3,    4,    5,    6,    7,    8,    9
   0xBF, 0x06, 0xDB, 0xCF, 0x66, 0xED, 0xFD, 0x07, 0xFF, 0xEF  
};

// \brief constructor with parameters
ClockMux::ClockMux(uint8_t dataPin, uint8_t clockPin, uint8_t d1Pin, uint8_t d2Pin, uint8_t d3Pin, uint8_t d4Pin ):
  mDataPin(dataPin)
, mClockPin(clockPin)
, mCurrentDigit(0)
, mCommonLevel(LOW)
, mPulse_ms(5)
, mPause_ms(1)
, mState(pause)
{
  
  mDigitPins[0]  = d1Pin;
  mDigitPins[1]  = d2Pin;
  mDigitPins[2]  = d3Pin;
  mDigitPins[3]  = d4Pin;

  mDigitValue[0]  = 0;
  mDigitValue[1]  = 0;
  mDigitValue[2]  = 0;
  mDigitValue[3]  = 0;

  setupPins();
}

// \brief initializes pinMode and state, should be called in method setup()
void ClockMux::setupPins()
{  
  pinMode(mDataPin, OUTPUT);
  digitalWrite(mDataPin, mCommonLevel);
  pinMode(mClockPin, OUTPUT);
  digitalWrite(mClockPin, mCommonLevel);

  for (uint8_t i=0; i<digits; ++i)
  {
    pinMode(mDigitPins[i], OUTPUT);
    digitalWrite(mDigitPins[i], mCommonLevel);
    mDigitValue[i] = 0;
  }
}

void ClockMux::tick(unsigned long fNow)
{
  if (fNow >= mTime)
  {
    switch (mState)
    {
      case pause:
        mTime = fNow + mPulse_ms;
        modifyDigit();
        mState = pulse;
        break;
      case pulse:
        mTime = fNow + mPause_ms;
        incrementDigit();
        mState = pause;
        break;
    }
  }
}

void ClockMux::incrementDigit()
{
   digitalWrite(mDigitPins[mCurrentDigit], mCommonLevel);
   if (++mCurrentDigit >= digits)
   {
      mCurrentDigit = 0;
   }
}

void ClockMux::modifyDigit()
{
  int fValue = mDigitValue[mCurrentDigit];
  if (fValue >= characters)
  {
    fValue = 0;
  }
  shiftOutData(mSegmentBits[fValue]);
  digitalWrite(mDigitPins[mCurrentDigit], !mCommonLevel);
}

void ClockMux::shiftOutData(uint8_t data)
{
  int fMask = 1;
  for (int i=0; i<segments; ++i, fMask <<= 1)
  {
    digitalWrite(mClockPin, mCommonLevel);
    digitalWrite(mDataPin, ((data & fMask) != 0) ? !mCommonLevel : mCommonLevel);
    digitalWrite(mClockPin, !mCommonLevel);
  }
  digitalWrite(mClockPin, mCommonLevel);
  digitalWrite(mDataPin, mCommonLevel);
}

void ClockMux::set_Time(uint8_t aHours, uint8_t aMinutes)
{
  mDigitValue[0] = aHours   / 10;
  mDigitValue[1] = aHours   % 10;
  mDigitValue[2] = aMinutes / 10;
  mDigitValue[3] = aMinutes % 10;
}

void ClockMux::set_Pause(uint8_t a_ms)
{
  mPause_ms = a_ms;
}

void ClockMux::set_Pulse(uint8_t a_ms)
{
  mPulse_ms = a_ms;
}


