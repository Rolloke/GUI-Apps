
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


String gLine1;
String gLine2;

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

  tmElements_t fTime;
  if (RTC.read(fTime))
  {
      LCD_PRINT_AT(0, 0, F("got time from RTC"));
      delay(1000);
  }

  if (RTC.chipPresent())
  {
    setSyncProvider(RTC.get);
  }

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
void fillString(String& aStr, int aLen)
{
    while (aStr.length() < aLen)
    {
        aStr += " ";
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

void print2Decimals(String& aStr, int aNumber)
{
    if (aNumber >= 0)
    {
        if (aNumber < 10) aStr += "0";
        aStr += String(aNumber);
    }
}

void PrintLCD_Time()
{
    String fLine1;
    fLine1 = gSettings.getStateName();

    if (   (gSettings.isTimerState()                        && gSettings.isTimerActive())
        || (gSettings.getState() == SettingStates::SetAlarm && gSettings.isAlarmActive()))
    {
        fLine1 += ": *";
    }

    if (gSettings.getState() == SettingStates::Time)
    {
        LCD_PRINT(": ");
        for (int i=0; i<SettingStates::Timers; ++i)
        {
            if (gSettings.isTimerActive(i))
            {
                fLine1 += "*";
                fLine1 += String(i+1);
            }
            else
            {
                fLine1 += "  ";
            }
        }
    }
    fillString(fLine1, LCD_XDIM);
    if (fLine1 != gLine1)
    {
        LCD_PRINT_AT(0, 0, fLine1);
        gLine1 = fLine1;
    }

    String fLine2;
    bool fPrintTime = false;
    switch (gSettings.getState())
    {
    case SettingStates::Time:
    case SettingStates::SetAlarm:
      fPrintTime = true;
      break;
    case SettingStates::Date:
#if LANGUAGE == EN
            fLine2 += monthShortStr(month());
            fLine2 += ",";
            fLine2 += String(day());
            fLine2 += ",";
            print2Decimals(fLine2, gSettings.getSeconds());
#else
            print2Decimals(fLine2, gSettings.getMinutes());
            fLine2 += ":";
            print2Decimals(fLine2, gSettings.getHours());
            fLine2 += ":";
            print2Decimals(fLine2, gSettings.getSeconds());
#endif
        break;
    case SettingStates::SetTime:
        print2Decimals(fLine2, gSettings.getHours());
        fLine2 += ":";
        print2Decimals(fLine2, gSettings.getMinutes());
        break;
    case SettingStates::SetDate:
#if LANGUAGE == EN
        print2Decimals(fLine2, gSettings.getHours());
        fLine2 += ":";
        print2Decimals(fLine2, gSettings.getMinutes());
#else
        print2Decimals(fLine2, gSettings.getMinutes());
        fLine2 += ":";
        print2Decimals(fLine2, gSettings.getHours());
#endif
        break;
    case SettingStates::SetYear:
        print2Decimals(fLine2, gSettings.getSeconds());
        break;
    case SettingStates::SetAlarmMode:
#if LANGUAGE == EN
      fLine2 += "Mode: ";
      switch (gSettings.getMinutes())
      {
      case SettingStates::Once:   fLine2 += "Once";   break;
      case SettingStates::Daily:  fLine2 += "Daily";  break;
      case SettingStates::Weekly: fLine2 += "Weekly"; break;
      default:break;
      }
#else
      fLine2 += "Modus: ";
      switch (gSettings.getMinutes())
      {
      case SettingStates::Once:   fLine2 += "einmal";     break;
      case SettingStates::Daily:  fLine2 += "täglich";    break;
      case SettingStates::Weekly: fLine2 += "wöchentlich";break;
      default:break;
      }
#endif
      break;
    case SettingStates::SetAlarmDay:
#if LANGUAGE == EN
      fLine2 += (dayStr(gSettings.getMinutes()));
#else
      switch (gSettings.getMinutes())
      {
      case dowSunday:    fLine2 += "Sonntag"; break;
      case dowMonday:    fLine2 += "Montag"; break;
      case dowTuesday:   fLine2 += "Dienstag"; break;
      case dowWednesday: fLine2 += "Mittwoch"; break;
      case dowThursday:  fLine2 += "Donnerstag"; break;
      case dowFriday:    fLine2 += "Freitag"; break;
      case dowSaturday:  fLine2 += "Sonnabend"; break;
      default: break;
      }


#endif
      break;
    case SettingStates::SetAlarmMelody:
    case SettingStates::SetContrast:
    case SettingStates::SetLightLow:
    case SettingStates::SetLightHigh:
      fLine2 += String(gSettings.getMinutes());
      break;
    default:
      fPrintTime = gSettings.isTimerState();
    }
    if (fPrintTime)
    {
        print2Decimals(fLine2, gSettings.getHours());
        fLine2 += (":");
        print2Decimals(fLine2, gSettings.getMinutes());
        fLine2 += (":");
        print2Decimals(fLine2, gSettings.getSeconds());
    }

    fillString(fLine2, LCD_XDIM);
    if (fLine2 != gLine2)
    {
        LCD_PRINT_AT(0, 1, fLine2);
        gLine2 = fLine2;
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
