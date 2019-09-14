#ifndef BinaryClockMux_h
#define BinaryClockMux_h

#include <inttypes.h>


class BinaryClockMux
{
public:
  enum timeindex { seconds, minutes, hours }; // index for LEDs common cathode
  enum consts    { ledPins=6, commonPins=3 };
  BinaryClockMux( uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, uint8_t bit4, uint8_t bit5, uint8_t commonSeconds, uint8_t commonMinutes, uint8_t commonHour, uint8_t commonLevel = 1);

  void setCommonDisplayTimes(uint8_t aSeconds, uint8_t aMinutes, uint8_t aHours);
  void displayLEDs(uint8_t aSeconds, uint8_t aMinutes, uint8_t aHours, unsigned long aNow);
  
private:
  BinaryClockMux();
  void setupPins();
  void showLEDs(uint8_t aType, bool aOn, unsigned long aNow);
  void setBinaryPinStates(uint8_t aBits);
  
  uint8_t mBinaryLEDPins[ledPins];
  uint8_t mCommonLEDPins[commonPins];
  uint8_t mCommonPinsTime[commonPins];
  unsigned long mNextChange;
  uint8_t mMuxIndex;
  uint8_t mCommonLevel;
};
#endif
