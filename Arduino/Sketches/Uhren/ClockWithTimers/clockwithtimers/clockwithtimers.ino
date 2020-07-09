
/*
  LCD Clock
 
 Shows clock time on an LCD Display
 * 4 buttons for settings
 * 1 alarm tone output
 created 2015
 by Rolf Kary-Ehlers
 
 */

#include "SettingStates.h"


#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <button.h>
#include <Melody.h>

#define LCD_PRINTER 1

#include "LCD_print.h"

//#include "ioMega2560.h"  // pins for test with at mega 2560
#include "ioAt328.h"   // pins for at mega 328


#define TICK_INTERVAL_MS 500


void PrintLCD_Time();

void triggerModeButton(uint8_t, uint8_t);
void triggerHourMinutButton(uint8_t, uint8_t);
void triggerAlarmButton(uint8_t, uint8_t);
void onTimerAlarm();


//#define TEST_PERIPHERY
Button gModeButton(gModeBtnPin, triggerModeButton);
Button gAlarmButton(gAlarmBtnPin, triggerAlarmButton);
uint8_t gHMPins[2] = {static_cast<uint8_t>(gHourBtnPin), static_cast<uint8_t>(gMinuteBtnPin) };
Button gHourMinuteButton(gHMPins, 2, triggerHourMinutButton);
OnTick_t gTimerFunctions[SettingStates::Timers];

Tone gTones1[] =
{
    { NOTE_C7, 1, 8},
    { 0, 2, 1},
    { 0, 0, 0}
};

Tone gTones2[] =
{
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};


Tone gTones3[] =
{
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};

Tone gTones4[] =
{
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};

Tone gTones5[] =
{
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 1, 8},
    { NOTE_C7, 1, 8},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};

Tone gTones6[] =
{
  #include "big_ben.h"
};

Melody gMelody(toneOutput, gTones6, 20, 1000);

#define countof(X) sizeof(X) / sizeof(X[0])

void setup() 
{
  if (RTC.chipPresent())
  {
    tmElements_t fTime;
    if (!RTC.read(fTime))
    {
      int Hour, Min, Sec;
  
      sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec);
      fTime.Hour = Hour;
      fTime.Minute = Min;
      fTime.Second = Sec;
  
      sscanf(__DATE__, "%d.%d.%d", &Hour, &Min, &Sec);
      fTime.Day   = 1;
      fTime.Month = 1;
      fTime.Year  = CalendarYrToTm(2016);
      fTime.Wday  = 1;
      
      RTC.write(fTime);
    }
    setSyncProvider(RTC.get);
  }
  
  gModeButton.setDeBounce(10);
  gAlarmButton.setDeBounce(10);
  gHourMinuteButton.setDeBounce(10);
  gModeButton.setDelay(3000);
  //gModeButton.setRepeat(2000);
  gHourMinuteButton.setDelay(1000);
  gHourMinuteButton.setRepeat(250);


  gSettings.setTimerFunction(&onTimerAlarm);
  gSettings.setAlarmFunction(&onTimerAlarm);
  LCD_BEGIN
}


void loop()
{
  unsigned long fNow = millis();

  gModeButton.tick(fNow);
  gHourMinuteButton.tick(fNow);
  gAlarmButton.tick(fNow);
  gSettings.tick(fNow);
  gMelody.tick(fNow);
  
  if (gSettings.hasDisplayChanged())
  {
    analogWrite(dimLED, gSettings.isLightOn() ? gSettings.getLightHigh() : gSettings.getLightLow());
    analogWrite(contrastPin, gSettings.getContrast());

    // display
    PrintLCD_Time();
  }
}

void triggerModeButton(uint8_t aState, uint8_t )
{
  gSettings.triggerButton(SettingStates::Mode, aState);
  gSettings.onTrigger();
  gSettings.triggerButton(SettingStates::ClearAll, Button::none);
}

void triggerHourMinutButton(uint8_t aState, uint8_t aPin)
{
  if (aPin & 1) gSettings.triggerButton(SettingStates::Hour, aState);
  if (aPin & 2) gSettings.triggerButton(SettingStates::Minute, aState);
  if (aPin & 3)
  {
    gSettings.onTrigger();
    gSettings.triggerButton(SettingStates::ClearAll, Button::none);
  }
}

void triggerAlarmButton(uint8_t aState, uint8_t )
{
    if (gMelody.isPlaying())
    {
        gMelody.stopMelody();
    }
    else
    {
      gSettings.triggerButton(SettingStates::AlarmBtn, aState);
      gSettings.onTrigger();
      gSettings.triggerButton(SettingStates::ClearAll, Button::none);
    }
}

void print2Decimals(int aNumber)
{
    if (aNumber >= 0)
    {
        if (aNumber < 10) LCD_PRINT("0");
        LCD_PRINT(aNumber);
    }
}

void PrintLCD_Time()
{
    LCD_PRINT_AT(0, 0, "                ");
    LCD_PRINT_AT(0, 0, gSettings.getStateName());

    if (   (gSettings.isTimerState()                        && gSettings.isTimerActive())
        || (gSettings.getState() == SettingStates::SetAlarm && gSettings.isAlarmActive()))
    {
        LCD_PRINT(": *")
    }

    if (gSettings.getState() == SettingStates::Time)
    {
        LCD_PRINT(": ");
        for (int i=0; i<SettingStates::Timers; ++i)
        {
            if (gSettings.isTimerActive(i))
            {
                LCD_PRINT("*");
                LCD_PRINT((i+1));
            }
            else
            {
                LCD_PRINT("  ");
            }
        }
    }

    LCD_PRINT_AT(0, 1, "                ")
    LCD_SETCURSOR(0, 1);
    bool fPrintTime = false;
    switch (gSettings.getState())
    {
    case SettingStates::Time:
    case SettingStates::SetAlarm:
      fPrintTime = true;
      break;
    case SettingStates::Date:
#if LANGUAGE == EN
            LCD_PRINT(monthShortStr(month()));
            LCD_PRINT(",");
            LCD_PRINT(day());
            LCD_PRINT(" ");
            print2Decimals(gSettings.getSeconds());
#else
        print2Decimals(gSettings.getMinutes());
        LCD_PRINT(":");
        print2Decimals(gSettings.getHours());
        LCD_PRINT(":");
        print2Decimals(gSettings.getSeconds());
#endif
        break;
    case SettingStates::SetTime:
        print2Decimals(gSettings.getHours());
        LCD_PRINT(":");
        print2Decimals(gSettings.getMinutes());
        break;
    case SettingStates::SetDate:
#if LANGUAGE == EN
        print2Decimals(gSettings.getHours());
        LCD_PRINT(":");
        print2Decimals(gSettings.getMinutes());
#else
        print2Decimals(gSettings.getMinutes());
        LCD_PRINT(":");
        print2Decimals(gSettings.getHours());
#endif
        break;
    case SettingStates::SetYear:
        print2Decimals(gSettings.getSeconds());
        break;
    case SettingStates::SetAlarmMode:
#if LANGUAGE == EN
      LCD_PRINT("Mode: ");
      switch (gSettings.getMinutes())
      {
      case SettingStates::Once:   LCD_PRINT("Once");   break;
      case SettingStates::Daily:  LCD_PRINT("Daily");  break;
      case SettingStates::Weekly: LCD_PRINT("Weekly"); break;
      default:break;
      }
#else
      LCD_PRINT("Modus: ");
      switch (gSettings.getMinutes())
      {
      case SettingStates::Once:   LCD_PRINT("einmal");     break;
      case SettingStates::Daily:  LCD_PRINT("täglich");    break;
      case SettingStates::Weekly: LCD_PRINT("wöchentlich");break;
      default:break;
      }
#endif
      break;
    case SettingStates::SetAlarmDay:
#if LANGUAGE == EN
      LCD_PRINT(dayStr(gSettings.getMinutes()));
#else
      switch (gSettings.getMinutes())
      {
      case dowSunday:    LCD_PRINT("Sonntag"); break;
      case dowMonday:    LCD_PRINT("Montag"); break;
      case dowTuesday:   LCD_PRINT("Dienstag"); break;
      case dowWednesday: LCD_PRINT("Mittwoch"); break;
      case dowThursday:  LCD_PRINT("Donnerstag"); break;
      case dowFriday:    LCD_PRINT("Freitag"); break;
      case dowSaturday:  LCD_PRINT("Sonnabend"); break;
      default: break;
      }


#endif
      break;
    case SettingStates::SetAlarmMelody:
    case SettingStates::SetContrast:
    case SettingStates::SetLightLow:
    case SettingStates::SetLightHigh:
      LCD_PRINT(gSettings.getMinutes());
      break;
    default:
      fPrintTime = gSettings.isTimerState();
    }
    if (fPrintTime)
    {
        print2Decimals(gSettings.getHours());
        LCD_PRINT(":");
        print2Decimals(gSettings.getMinutes());
        LCD_PRINT(":");
        print2Decimals(gSettings.getSeconds());
    }

}

void onTimerAlarm()
{
    gMelody.stopMelody();
    switch (gSettings.onTimerAlarm())
    {
    case 0: gMelody.setTones(gTones1); break;
    case 1: gMelody.setTones(gTones2); break;
    case 2: gMelody.setTones(gTones3); break;
    case 3: gMelody.setTones(gTones4); break;
    case 4: gMelody.setTones(gTones5); break;
    case SettingStates::Timers: gMelody.setTones(gTones6); break;
    }
    gMelody.startMelody();
}
