#include "AD9833.h"
#include <Arduino.h>

AD9833::AD9833(uint8_t aFSYNC, uint8_t aSDATA, uint8_t aSCLK)
: mFSYNC(aFSYNC)
, mSDATA(aSDATA)
, mSCLK(aSCLK)
{
  pinMode(mFSYNC, OUTPUT);
  pinMode(mSDATA, OUTPUT);
  pinMode(mSCLK , OUTPUT);
  digitalWrite(mFSYNC, HIGH);
  digitalWrite(mSDATA, LOW);
  digitalWrite(mSCLK , HIGH);
}

void AD9833::setup()
{
  // Nach Application Note AN-1070 von Analog Devices
  updateRegister(CONTROL_RESET);  
  updateRegister(FREQUENCY_REGISTER0|LSB_INIT);
  updateRegister(FREQUENCY_REGISTER0|MSB_INIT);
  updateRegister(PHASE_REGISTER0);
  updateRegister(CONTROL_EXIT_RESET);
}

void AD9833::updateFreqency(long aFrequency, int aWaveForm, double aPhase, int aFlag)
{
  // Die Frequenz Register schreiben.
  long fFrequencyRegister;
  unsigned int fMSB, fLSB;
  uint16_t fPhase;
   
  fFrequencyRegister = (aFrequency * pow(2, 28)) / QUARZ_FREQUENCY; 
  if (aWaveForm == RECTANGLE)
  {
    fFrequencyRegister = fFrequencyRegister << 1;  // Rechteck 1/2 Frequenz
  }
  fMSB = (int)((fFrequencyRegister & 0xFFFC000) >> 14);
  fLSB = (int) (fFrequencyRegister & 0x3FFF);
  fPhase = toPhase(aPhase);
  if (aFlag & FLAG_REG0)
  {
    fLSB   |= FREQUENCY_REGISTER0;
    fMSB   |= FREQUENCY_REGISTER0;
    fPhase |= PHASE_REGISTER0;
  }
  else
  {
    fLSB   |= FREQUENCY_REGISTER1;
    fMSB   |= FREQUENCY_REGISTER1;
    fPhase |= PHASE_REGISTER1;
  }
  
  if (aFlag & FLAG_RESET)
  {
    updateRegister(CONTROL_RESET);  // Control Register, Reset Bit DB8 gesetzt
  }
  updateRegister(fLSB);             // Frequency Register 0 fLSB
  updateRegister(fMSB);             // Frequency Register 0 fMSB
  updateRegister(fPhase);           // Phase Register
  if (aFlag & FLAG_XRESET)
  {
    updateRegister(aWaveForm);      // Exit Reset : Wellenform nach dem Reset
  }
}

uint16_t AD9833::toWaveForm(int aWaveFormIndex)
{
    switch (aWaveFormIndex)
    {
      case 0: return SINE;
      case 1: return TRIANGLE;
      case 2: return RECTANGLE;
      default:return SINE; 
    }
}

uint16_t AD9833::toPhase(double aPhase)
{
  return static_cast<uint16_t>(2 * M_PI * aPhase / 4096 + 0.5);
}

void AD9833::updateRegister(unsigned int aData)
{
  // Ein DDS Register schreiben
  unsigned int fPointer = 0x8000;

  digitalWrite(mFSYNC, LOW);
  for (int i=0; i<16; i++)
  {
    if ((aData & fPointer) > 0) 
    { 
      digitalWrite(mSDATA, HIGH); 
    }
    else
    { 
      digitalWrite(mSDATA, LOW); 
    }
    
    digitalWrite(mSCLK, LOW);
    digitalWrite(mSCLK, HIGH);
    fPointer = fPointer >> 1;
  }
  
  digitalWrite(mFSYNC, HIGH);
}


