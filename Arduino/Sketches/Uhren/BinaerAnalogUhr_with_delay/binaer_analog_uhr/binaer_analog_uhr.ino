
/*
  Binary Clock and analog Clock
 
 Shows clock time in binary format and drives two outputs for analog gauge
 
 The circuit:
 * LEDs connected to pins via 2.2k resistors (aprox. 1 mA per LED)
 * the cathode connector of the LEDs for seconds, minutes and hours
 * 2 pwm outputs for analog gauge
 * 1 light for analog gauge
 * 4 buttons for settings
 * 1 alarm tone output
 created 2015
 by Rolf Kary-Ehlers
 TODO! at alarm switch off all LED and analog output
 TODO! after some minutes dim all LED
 TODO! after some minutes switch off analog pins 
 FIXME! Setting the time is not working properly
 
 */

#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <DS1307RTC.h>

#include "BinaryClockMux.h"
#include "SettingStates.h"
#include "Melody.h"


//#include "ioMega2560.h"  // pins for test with at mega 2560

#include "ioAt328.h"   // pins for at mega 328



//#define TEST_PERIPHERY
//#define CALIBRATE_VU


#ifdef SOFTWARE_SERIAL
#include <SoftwareSerial.h>
#endif


#define countof(X) sizeof(X) / sizeof(X[0])

int mCounter = 0;
int mOldState = -1;

Tone gTones1[] =
{ 
  #include "piep.h"  
};
Tone gTones2[] =
{ 
  #include "big_ben.h"
};
Tone gTones3[] =
{ 
  #include "die_maus.h"
};

Tone gTones4[] =
{ 
  #include "SevenTone.h"  
};
/*
Tone gTones5[] =
{ 
  #include "piep.h"  
};

Tone gTones6[] =
{ 
  #include "piep.h"  
};
*/
Melody gAlarmMelody1(toneOutput, gTones1, 20, 1000);
Melody gAlarmMelody2(toneOutput, gTones2, 20, 1000);
Melody gAlarmMelody3(toneOutput, gTones3, 20, 1000);
Melody gAlarmMelody4(toneOutput, gTones4, 20, 1000);
/*
Melody gAlarmMelody5(toneOutput, gTones5, 20, 1000);
Melody gAlarmMelody6(toneOutput, gTones6, 20, 1000);
*/
Melody* gAlarmMelody = &gAlarmMelody1;

uint8_t  mapHours(uint8_t aHour);
uint8_t  mapMinutes(uint8_t aMinutes);
void playTimerAlarm();
void playAlarm();

void setup() 
{
  pinMode(analogMinute, OUTPUT);
  analogWrite(analogMinute, 0);
  pinMode(analogHour, OUTPUT);
  analogWrite(analogHour, 0);
  
  pinMode(dimLED, OUTPUT);
  digitalWrite(dimLED, 0);
  
#ifdef SOFTWARE_SERIAL
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif
   
#ifdef LCD_PRINT
  gLCD.begin(16, 4);
#endif

  gSettings.setAlarmFunction(playAlarm);
  gSettings.setTimerFunction(playTimerAlarm);

#ifdef TEST_PERIPHERY
  testPeriphery();
#endif
  gTimeDisplay.setCommonDisplayTimes(2,2,3);

  tmElements_t fTime;
  if (!RTC.read(fTime) && RTC.chipPresent())
  {
    int Hour, Min, Sec;
  
    sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec);
    fTime.Hour = Hour;
    fTime.Minute = Min;
    fTime.Second = Sec;

    fTime.Day   = 1;
    fTime.Month = 1;
    fTime.Year  = CalendarYrToTm(2016);
    fTime.Wday  = 1;
    
    RTC.write(fTime);
  }
  // initialize IOs
  setSyncProvider(RTC.get);
//  gAlarmMelody = &gAlarmMelody3;
//  gAlarmMelody->startMelody();
}

#ifdef CALIBRATE_VU
int calibrateValue=0;
#endif

void loop()
{

#ifdef CALIBRATE_VU
calibrateVU();
#else
  bool fAlarmOn = false;
  if (gAlarmMelody)
  {
    gAlarmMelody->tick();
    fAlarmOn = gAlarmMelody->isPlaying();
  }

  // display
  if (!fAlarmOn && gSettings.isLED_DisplayOn())
  {
    gTimeDisplay.displayLEDs(gSettings.getHours(), gSettings.getMinutes(), gSettings.getSeconds());
  }
  else
  {
    delay(10);
  }
  
  if (!fAlarmOn && gSettings.isAnalogDisplayOn())
  {
    analogWrite(analogMinute, mapMinutes(gSettings.getMinutes()));
    analogWrite(analogHour  , mapHours(gSettings.getHours()%12));
  }
  else
  {
    analogWrite(analogMinute, 0);
    analogWrite(analogHour  , 0);
  }
 
  ++mCounter;

  if (mCounter % 5 == 0)
  {
    gSettings.tick();   // setting of the clock 
  }
  
  if ( mCounter % 50 == 0)
  {
    digitalWrite(dimLED, gSettings.isLightOn());
    
    if (gAlarmMelody != 0 && gSettings.isButtonPressed(SettingStates::Stop))
    {
      gAlarmMelody->stopMelody();
      gAlarmMelody = 0;
#ifdef LCD_PRINT
      gLCD.setCursor(0, 2);
      gLCD.print("                ");
#endif
    }
#ifdef LCD_PRINT
    PrintLCD_Time();
#endif
    Alarm.delay(1);
  } 
  if (mCounter == 100)
  {
    mCounter = 0;
#ifdef SOFTWARE_SERIAL
    Serial.print("Time: ");
    Serial.print(day());
    Serial.print(":");
    Serial.print(month());
    Serial.print(":");
    Serial.print(year());
    Serial.print(", ");
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.println(second());
#endif
  }
#endif
}

uint8_t  mapMinutes(uint8_t aMinutes)
{
  static const uint8_t Minutes[61] =  { 0 ,   2,   4,   6,   8,  10, //  5
                                             12,  15,  17,  20,  23, // 10
                                             24,  26,  28,  30,  32, // 15
                                             33,  34,  36,  38,  40, // 20
                                             41,  43,  45,  47,  49, // 25
                                             52,  54,  56,  59,  61, // 30
                                             64,  66,  68,  70,  73, // 35
                                             76,  79,  82,  85,  88, // 40
                                             91,  95,  98, 101, 104, // 45
                                            107, 111, 116, 121, 127, // 50
                                            131, 135, 140, 147, 155, // 55
                                            162, 170, 180, 188, 196};// 60

  if (aMinutes > 60) aMinutes = 60;
  return Minutes[aMinutes];

//  return map(aMinutes, 1, 60, 10, 255);
}

uint8_t  mapHours(uint8_t aHour)
{
  // Hours                                    1   2   3   4   5   6   7   8   9    10   11   12
  static const uint8_t Hours[13]     =  {209, 12, 32, 40, 52, 62, 73, 83, 98, 116, 139, 166, 209 };
  

  if (aHour > 12) aHour = 12;
  return Hours[aHour];
//  return map(mHours, 0, 24, 10, 255);
}

void playAlarm()
{
#ifdef LCD_PRINT
  gLCD.setCursor(0, 2);
  gLCD.print(" Alarm ");
#endif
  if (gAlarmMelody)
  {
    gAlarmMelody->stopMelody();
  }
  switch (gSettings.getAlarmMelody())
  {
    case 0:  gAlarmMelody = &gAlarmMelody1; break;
    case 1:  gAlarmMelody = &gAlarmMelody2; break;
    case 2:  gAlarmMelody = &gAlarmMelody3; break;
    case 3:  gAlarmMelody = &gAlarmMelody4; break;
//    case 4:  gAlarmMelody = &gAlarmMelody5; break;
//    case 5:  gAlarmMelody = &gAlarmMelody6; break;
    default: gAlarmMelody = &gAlarmMelody1; break;
  }
  
  if (gAlarmMelody)
  {
#ifdef SOFTWARE_SERIAL 
  Serial.print("Play Alarm");
  Serial.println((unsigned int)gSettings.getAlarmMelody());
#endif  
    gAlarmMelody->startMelody();
  }
  
}

void playTimerAlarm()
{
#ifdef LCD_PRINT
  gLCD.setCursor(0, 2);
  gLCD.print(" Timer Alarm ");
#endif
  gSettings.stopTimer();
  if (gAlarmMelody)
  {
    gAlarmMelody->stopMelody();
  }
  gAlarmMelody = &gAlarmMelody1;
  gAlarmMelody->startMelody();
}

#ifdef CALIBRATE_VU
void calibrateVU()
{
  ++mCounter;
  analogWrite(analogMinute, calibrateValue);
  analogWrite(analogHour  , calibrateValue);
  gTimeDisplay.displayLEDs(0, calibrateValue>>6, calibrateValue&0x7f);
  if (mCounter == 5)
  {
    gSettings.tick();
    if (gSettings.isButtonPressed(SettingStates::Plus))
    {
      calibrateValue++;
    }
    if (gSettings.isButtonPressed(SettingStates::Minus))
    {
      calibrateValue--;
    }
    mCounter = 0;
  }
}
#endif

#ifdef LCD_PRINT
void PrintLCD_Time()
{
    gLCD.setCursor(0, 0);
    if (gSettings.getHours()  <10) gLCD.print("0");
    gLCD.print(gSettings.getHours());
    gLCD.print(":");
    if (gSettings.getMinutes()<10) gLCD.print("0");
    gLCD.print(gSettings.getMinutes());
    gLCD.print(":");
    if (gSettings.getSeconds()<10) gLCD.print("0");
    gLCD.print(gSettings.getSeconds());
    if (mOldState != gSettings.getState())
    {
      gLCD.setCursor(0, 1);
      gLCD.print("                ");
      gLCD.setCursor(0, 1);
      gLCD.print(gSettings.getStateName());
      mOldState = gSettings.getState();
    }
}
#endif

#ifdef TEST_PERIPHERY
void testPeriphery()
{
  gTimeDisplay.setCommonDisplayTimes(200,200,200);
  int i;
  for (i=0; i<6; ++i)
  {
    gTimeDisplay.displayLEDs(1<<i, 0, 0);
  }
  for (i=0; i<6; ++i)
  {
    gTimeDisplay.displayLEDs(0, 1<<i, 0);
  }
  for (i=0; i<6; ++i)
  {
    gTimeDisplay.displayLEDs(0, 0, 1<<i);
  }
  for (i=0; i<=12; ++i)
  {
    analogWrite(analogHour  , mapHours(i));
    delay(500);
  }
  for (i=0; i<=60; ++i)
  {
    analogWrite(analogMinute, mapMinutes(i));
    delay(500);
  }
  digitalWrite(dimLED, true);
  delay(500);
  digitalWrite(dimLED, false);
  tone(toneOutput, NOTE_C3, 500);
  delay(500);
  noTone(toneOutput);
}
#endif

