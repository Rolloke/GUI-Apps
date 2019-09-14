#include <SPI.h>
#include <button.h>

// Arduino Pins
// Pin    Nano    Mega  Farbe
// SCK    13      52    gelb
// MISO   12      50    nc 
// MOSI   11      51    orange
// SS     10      53    braun
#define PIN_SWITCH_PATTERN 2  // D2 IO-Pin
#define PIN_BUZZER         3  // D3 IO-Pin PWM


#define IOEXP_NOOP        0
#define IOEXP_BASE_ADDR   0x24


struct sLeds
{
  uint8_t pin;
  uint8_t device;
};

const int gLEDsize = 22;

const sLeds gLEDS[gLEDsize] = 
{
  // red LEDs
  { 8, 1},  {14, 1}, {10, 1},           //  0,  1,  2
  {20, 1},  {18, 1}, {22, 1},           //  3,  4,  5
  { 8, 2},  {12, 2}, {10, 2},  {16, 1}, //  6,  7,  8,  9
  // green LEDs
  { 9, 1},  {15, 1}, {11, 1},           // 10, 11, 12
  {21, 1},  {19, 1}, {23, 1},           // 13, 14, 15
  { 9, 2},  {13, 2}, {11, 2},  {17, 1}, // 16, 17, 18, 19
  // Big LED red, green
  {14, 2}, {15, 2}                      // 20, 21
};


class Blinker
{
  public:
  Blinker(): mIndices(0), mSize(0), mNextSwitchOff(0), mNextWaitEnd(0) {};
  void blinkLEDs(const int* aIndex, int aSize, int aOnTime, int aOffTime);
  void tick(unsigned long aTick);
  bool isReady();
  
  private:
  
  void switchLEDs(bool bOn);
  
  int *mIndices;
  int  mSize;
  unsigned long mNextSwitchOff;
  unsigned long mNextWaitEnd;
};


void triggerButton(uint8_t aState, uint8_t aID);
Button gSwitchPattern(PIN_SWITCH_PATTERN, triggerButton);
Blinker gBlinker;


enum ePattern
{
  AllLEDSingle,
  AllLEDRandom,
  ElacPattern,
  RotationPattern,
  PingPattern,
  LastPattern,
  FirstPattern = AllLEDSingle
};


void setup() 
{
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  pinMode(PIN_BUZZER, OUTPUT);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
  initMAX7301();
  randomSeed(micros());
}

int gPattern = AllLEDSingle;
int gIndex = 0;
bool gCountUp = true;
const uint32_t gRandomMax = 1 << gLEDsize;

void loop()
{
  unsigned long fTicks = millis();
  gSwitchPattern.tick(fTicks);
  gBlinker.tick(fTicks);

  if (gBlinker.isReady())
  {
    switch (gPattern)
    {
      case AllLEDSingle:    blinkAllLEDsSingle();  break;
      case AllLEDRandom:    blinkAllLEDsRandom();  break;
      case ElacPattern:     showElacPattern();     break;
      case RotationPattern: showRotationPattern(); break;
      case PingPattern:     showPingPattern();     break;
    }
  }
}

void triggerButton(uint8_t aState, uint8_t aID)
{
  if (aState == Button::pressed && aID == PIN_SWITCH_PATTERN)
  {
    changePattern();
  }
}

void changePattern()
{
  gIndex   = 0;
  gCountUp = true;
  
  gPattern++;
  if (gPattern == LastPattern)
  {
    gPattern = 0;
  }
}

void blinkAllLEDsSingle()
{
  static int fIndex;
  fIndex = gIndex;
  gBlinker.blinkLEDs(&fIndex, 1, 400, 100);
  if (gCountUp) gIndex++;
  else          gIndex--;
  
  if (gIndex>=gLEDsize) gCountUp = false;
  else if (gIndex <= 0) gCountUp = true;
}

void blinkAllLEDsRandom()
{
  static int fRandomIndices[gLEDsize];
  uint32_t fRandomNo = random(4194303);
  int fMask = 1;
  int fCount = 0;
  for (int i=0; i<gLEDsize; ++i, fMask <<= 1)
  {
    if (fMask & fRandomNo)
    {
      fRandomIndices[fCount++] = i;
    }
  }
  gBlinker.blinkLEDs(fRandomIndices, fCount, 100, 10);
}

void showElacPattern()
{
  static const int fEred[] =  { 0,  1,  2,  3,  4,  6,  7,  8}; // E red
  static const int fEgreen[]= {10, 11, 12, 13, 14, 16, 17, 18}; // E green
  static const int fLred[] =  { 0,  3,  6,  7,  8};             // L red
  static const int fLgreen[]= {10, 13, 16, 17, 18};             // L green
  static const int fAred[] =  { 1,  3,  4,  5,  6,  8};         // A red
  static const int fAgreen[]= {11, 13, 14, 15, 16, 18};         // A green
  static const int fCred[] =  { 0,  1,  2,  3,  6,  7,  8};     // C red
  static const int fCgreen[]= {10, 11, 12, 13, 16, 17, 18};     // C green
 
  int fSize = 0;
  int *fPattern = 0;
  switch (gIndex)
  {
  case 0: fPattern = fEred;   fSize = sizeof(fEred)   / sizeof(int); break;
  case 1: fPattern = fLred;   fSize = sizeof(fLred)   / sizeof(int); break;
  case 2: fPattern = fAred;   fSize = sizeof(fAred)   / sizeof(int); break;
  case 3: fPattern = fCred;   fSize = sizeof(fCred)   / sizeof(int); break;
  case 4: fPattern = fEgreen; fSize = sizeof(fEgreen) / sizeof(int); break;
  case 5: fPattern = fLgreen; fSize = sizeof(fLgreen) / sizeof(int); break;
  case 6: fPattern = fAgreen; fSize = sizeof(fAgreen) / sizeof(int); break;
  case 7: fPattern = fCgreen; fSize = sizeof(fCgreen) / sizeof(int); break;
  }

  int fSwitchOffDelay = 100;
  gIndex++;
  if (gIndex == 8)
  {
    gIndex = 0;
    fSwitchOffDelay += 1000;
  }

  gBlinker.blinkLEDs(fPattern, fSize, 1000, fSwitchOffDelay);
  
}

void showRotationPattern()
{
  static const int fAngle1[] =  {1, 17}; // 
  static const int fAngle2[] =  {2, 16}; // 
  static const int fAngle3[] =  {5, 13}; // 
  static const int fAngle4[] =  {8, 10}; // 
  static const int fAngle5[] =  {7, 11}; // 
  static const int fAngle6[] =  {6, 12}; // 
  static const int fAngle7[] =  {3, 15}; // 
  static const int fAngle8[] =  {0, 18}; // 
  int fSize = 0;
  int *fPattern = 0;
  switch (gIndex)
  {
  case 0: fPattern = fAngle1; fSize = sizeof(fAngle1) / sizeof(int); break;
  case 1: fPattern = fAngle2; fSize = sizeof(fAngle2) / sizeof(int); break;
  case 2: fPattern = fAngle3; fSize = sizeof(fAngle3) / sizeof(int); break;
  case 3: fPattern = fAngle4; fSize = sizeof(fAngle4) / sizeof(int); break;
  case 4: fPattern = fAngle5; fSize = sizeof(fAngle5) / sizeof(int); break;
  case 5: fPattern = fAngle6; fSize = sizeof(fAngle6) / sizeof(int); break;
  case 6: fPattern = fAngle7; fSize = sizeof(fAngle7) / sizeof(int); break;
  case 7: fPattern = fAngle8; fSize = sizeof(fAngle8) / sizeof(int); break;
  }
  gBlinker.blinkLEDs(fPattern, fSize, 200, 100);
  gIndex++;
  if (gIndex == 8)  gIndex = 0;
}

void showPingPattern()
{
  static const int fMainR[] =   {21};          // 
  static const int fLEDs1R[] =  {10, 13, 16}; // 
  static const int fLEDs2R[] =  {11, 14, 17}; // 
  static const int fLEDs3R[] =  {12, 15, 18}; // 
  static const int fLEDs4R[] =  {19};         // 

  static const int fMainG[] =   {20};          // 
  static const int fLEDs1G[] =  { 0,  3,  6}; // 
  static const int fLEDs2G[] =  { 1,  4,  7}; // 
  static const int fLEDs3G[] =  { 2,  5,  8}; // 
  static const int fLEDs4G[] =  { 9};         //
  //                               0,   1,   2,   3,    4,   5,   6,   7,   8,    9
  static const int fDelay[]  =  {400, 200, 200, 200, 2800, 200, 200, 200, 400, 5000 };
  int fSize = 0;
  int *fPattern = 0;
  switch (gIndex)
  {
  case 0: fPattern = fMainR;  fSize = sizeof(fMainR)  / sizeof(int); break;
  case 1: fPattern = fLEDs1R; fSize = sizeof(fLEDs1R) / sizeof(int); break;
  case 2: fPattern = fLEDs2R; fSize = sizeof(fLEDs2R) / sizeof(int); break;
  case 3: fPattern = fLEDs3R; fSize = sizeof(fLEDs3R) / sizeof(int); break;
  case 4: fPattern = fLEDs4R; fSize = sizeof(fLEDs4R) / sizeof(int); break;
  case 5: fPattern = fLEDs4G; fSize = sizeof(fLEDs4G) / sizeof(int); break;
  case 6: fPattern = fLEDs3G; fSize = sizeof(fLEDs3G) / sizeof(int); break;
  case 7: fPattern = fLEDs2G; fSize = sizeof(fLEDs2G) / sizeof(int); break;
  case 8: fPattern = fLEDs1G; fSize = sizeof(fLEDs1G) / sizeof(int); break;
  case 9: fPattern = fMainG;  fSize = sizeof(fMainG)  / sizeof(int); break;
  }
  
  gBlinker.blinkLEDs(fPattern, fSize, 100, fDelay[gIndex]);
  if (gIndex == 0)
  {
    playBuzzer(255);
    delay(100);
    playBuzzer(0);
  }
  if (gIndex == 9)
  {
    playBuzzer(120);
    delay(100);
    playBuzzer(0);
  }
  gIndex++;
  if (gIndex == 10)  gIndex = 0;
}

void playBuzzer(int aLevel)
{
  // RC-Glied am Ausgang ermitteln
  // Pegel Bereich 0-255 ermitteln
  analogWrite(PIN_BUZZER, aLevel);
}

void Blinker::blinkLEDs(const int* aIndex, int aSize, int aOnTime, int aOffTime)
{
  mNextSwitchOff = millis() + aOnTime;
  mNextWaitEnd   = mNextSwitchOff + aOffTime;
  mIndices       = aIndex;
  mSize          = aSize;
  switchLEDs(true);
}

void Blinker::tick(unsigned long aTick)
{
  if (mNextSwitchOff)
  {
    if (aTick >= mNextSwitchOff)
    {
      switchLEDs(false);
      mNextSwitchOff = 0;
    }
  }
  else if (mNextWaitEnd)
  {
    if (aTick >= mNextWaitEnd)
    {
      mNextWaitEnd = 0;
    }
  }
}

bool Blinker::isReady()
{
  return mNextSwitchOff == 0 && mNextWaitEnd == 0;
}

void Blinker::switchLEDs(bool bOn)
{
  for (int i=0; i<mSize; ++i)
  {
    writeCommand(0x24+gLEDS[mIndices[i]].pin, bOn ? 0 : 1, gLEDS[mIndices[i]].device);
  }
}

void write32(uint16_t aD1_data, uint16_t aD2_data)
{
  digitalWrite(SCK, LOW);
  digitalWrite(SS, LOW);
  delay(1);
  SPI.transfer16(aD1_data);
  SPI.transfer16(aD2_data);
  delay(1);
  digitalWrite(SS, HIGH);
  digitalWrite(SCK, LOW);
}


void writeCommand(uint8_t aCmd, uint8_t aData, uint8_t aDevice)
{
  uint16_t fProg = (aCmd << 8) | aData;
  if (aDevice == 2)
  {
    write32(fProg, IOEXP_NOOP);
  }
  else
  {
    write32(IOEXP_NOOP, fProg);
  }

}

void initMAX7301()
{
  const uint16_t IOExp_Initdata[] =
  {  
    0x0000,   // D1 NOOP (zu Synchronisationszwecken nach Reset)
    0x0000,   // D2 NOOP (zu Synchronisationszwecken nach Reset)
    0x0401,   // D1 Disable Shutdown Mode
    0x0401,   // D2 Disable Shutdown Mode
    0x0955,   // D1 Config P[ 7.. 4] = 0b0101.0101
    0x0955,   // D2 Config P[ 7.. 4] = 0b1111.1111
    0x0A55,   // D1 Config P[11.. 8] = 0b1001.0101
    0x0A55,   // D2 Config P[11.. 8] = 0b1111.1111
    0x0B55,   // D1 Config P[15..12] = 0b1010.1010
    0x0B55,   // D2 Config P[15..12] = 0b1111.1111
    0x0C55,   // D1 Config P[19..16] = 0b0110.1110
    0x0C55,   // D2 Config P[19..16] = 0b0111.1101
    0x0D55,   // D1 Config P[23..20] = 0b1001.0101
    0x0D55,   // D2 Config P[23..20] = 0b0101.0101
    0x0E55,   // D1 Config P[27..24] = 0b1010.1010
    0x0E55,   // D2 Config P[27..24] = 0b0110.0111
    0x0F55,   // D1 Config P[31..28] = 0b0110.1010
    0x0F55,   // D2 Config P[31..28] = 0b0101.1101
    0x44FF,   // D1 Write  P[11.. 4] = 0b0110.0110
    0x44FF,   // D2 Write  P[11.. 4] = 0b0000.0000
    0x4CFF,   // D1 Write  P[19..12] = 0b0000.0000
    0x4CFF,   // D2 Write  P[19..12] = 0b0001.0000
    0x54FF,   // D1 Write  P[27..20] = 0b0000.0000
    0x54FF,   // D2 Write  P[27..20] = 0b0000.0000
    0x5CFF,   // D1 Write  P[31..28] = 0b0000.0000
    0x5CFF    // D2 Write  P[31..28] = 0b0000.0000
  };
  int Size = sizeof(IOExp_Initdata) / sizeof(uint16_t);
  for (int i=0; i<Size; i+=2)
  {
    write32(IOExp_Initdata[i], IOExp_Initdata[i+1]);
  }
}




