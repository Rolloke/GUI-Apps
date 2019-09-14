#ifndef BinaryClockMux_h
#define BinaryClockMux_h

#include <inttypes.h>

/*
  Fluorecent Display ClockMux
  Shows 4 letters on a fluorescent display
  4 pins for the letters
  a data and a clock pin for the 7 Segments of a letter
*/
class ClockMux
{
public:
  enum consts    { segments = 8, digits = 4, characters = 10 , pulse=0, pause};
  ClockMux(uint8_t dataPin, uint8_t clockPin, uint8_t d1Pin, uint8_t d2Pin, uint8_t d3Pin, uint8_t d4Pin);

  void tick(unsigned long fNow);
  
  void set_Time(uint8_t aHours, uint8_t aMinutes);
  void set_Pause(uint8_t a_ms);
  void set_Pulse(uint8_t a_ms);
  
private:
  ClockMux();
  void setupPins();
  void modifyDigit();
  void incrementDigit();
  void shiftOutData(uint8_t data);

  uint8_t mDataPin;
  uint8_t mClockPin;
  uint8_t mCurrentDigit;
  uint8_t mCommonLevel;
  uint8_t mPulse_ms;
  uint8_t mPause_ms;
  uint8_t mState;
  uint8_t mDigitPins[digits];
  uint8_t mDigitValue[digits];
  unsigned long mTime;

  static uint8_t mSegmentBits[characters];
  
};
#endif
