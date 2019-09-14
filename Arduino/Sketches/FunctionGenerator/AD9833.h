
#ifndef _AD9833_HEADER_INCLUDED_
#define _AD9833_HEADER_INCLUDED_

#include <inttypes.h>

// Generator implementation
#define SINE       0x2000       // index 0
#define TRIANGLE   0x2002       // index 1
#define RECTANGLE  0x2020       // index 2

#define FLAG_REG0   0x0001
#define FLAG_REG1   0x0000
#define FLAG_RESET  0x0002
#define FLAG_XRESET 0x0004


#define CONTROL_RESET             0x2100
#define CONTROL_EXIT_RESET        0x2000

#define LSB_INIT                  0x10C7
#define MSB_INIT                  0x0000

#define FREQUENCY_REGISTER0       0x4000
#define FREQUENCY_REGISTER1       0x8000
#define PHASE_REGISTER0           0xC000
#define PHASE_REGISTER1           0xE000
#define QUARZ_FREQUENCY 25000000  // 25MHz Quarz

#ifndef M_PI
  #define M_PI 3.141592654;
#endif

class AD9833
{
  public:
  AD9833(uint8_t aFSYNC, uint8_t aSDATA, uint8_t aSCLK);

  void setup();
  void updateFreqency(long aFrequency, int aWaveForm, double aPhase=0, int aFlag=FLAG_REG0|FLAG_RESET|FLAG_XRESET);
  void updateRegister(unsigned int aData);

  static uint16_t toWaveForm(int aWaveFormIndex);
  
  private:
  uint16_t toPhase(double aPhase);

  uint8_t mFSYNC;
  uint8_t mSDATA;
  uint8_t mSCLK;
};

#endif //_AD9833_HEADER_INCLUDED_
