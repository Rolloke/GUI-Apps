
/*
  Fluorecent Display Clock
 
 Shows clock time on a fluorescent display
 
 The circuit:
 ClockMux device with 6 pins
 * 4 buttons for settings
 * 1 alarm tone output
 created 2015
 by Rolf Kary-Ehlers
 
 */

#include "ClockMux.h"
#include "SettingStates.h"

#include "ioMega2560.h"  // pins for test with at mega 2560
//#include "ioAt328.h"   // pins for at mega 328


#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <DS1307RTC.h>

#include "serial_debug.h"
#include "LCD_print.h"

#ifdef LCD_PRINTER
  LiquidCrystal LCD(8, 9, 10, 11, 12, 13);
#endif

void triggerModeButton(uint8_t, uint8_t);
void triggerHourMinutButton(uint8_t, uint8_t);
void triggerAlarmButton(uint8_t, uint8_t);

//#define TEST_PERIPHERY
Button gModeButton(gModeBtnPin, triggerModeButton);
Button gAlarmButton(gAlarmBtnPin, triggerAlarmButton);
uint8_t gHMPins[2] = {gHourBtnPin , gMinuteBtnPin };
Button gHourMinuteButton(gHMPins, 2, triggerHourMinutButton);


#define countof(X) sizeof(X) / sizeof(X[0])

void setup() 
{
  gTimeDisplay.set_Pause(1);
  gTimeDisplay.set_Pulse(2);

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
  

  gModeButton.setDelay(4000);
  //gModeButton.setRepeat(2000);
  gHourMinuteButton.setDelay(1000);
  gHourMinuteButton.setRepeat(500);

#ifdef SOFTWARE_SERIAL 
  Serial.begin(115200);
#endif
  LCD_BEGIN
}


void loop()
{
#ifdef TEST_PERIPHERY
  testPeriphery();
#else
  unsigned long fNow = millis();

  gModeButton.tick(fNow);
  gHourMinuteButton.tick(fNow);
  gAlarmButton.tick(fNow);
  
  gSettings.tick(fNow);
  
  // todo
  gTimeDisplay.set_Time(gSettings.getHours(), gSettings.getMinutes());
  gTimeDisplay.tick(fNow);
  
#ifdef SOFTWARE_SERIAL 
  if (gSettings.isChanged())
  {
    DEBUG_PRINT(gSettings.getHours());
    DEBUG_PRINT(" / ");
    DEBUG_PRINT(gSettings.getMinutes());
    DEBUG_PRINT(" / ");
    DEBUG_PRINTLN(gSettings.getSeconds());
  }
#else
#ifdef LCD_PRINTER
  if (gSettings.isChanged())
  {
    SettingStates::state fState =  gSettings.getState();
    if (fState == SettingStates::Time || fState == SettingStates::Date)
    {
      const char *monthName[12] = 
      {
        "Januar", "Februar", "März", "April", "Mai", "Juni",
        "Juli", "August", "September", "Oktober", "November", "Dezember"
      };
      const char *dayName[7] = 
      {
        "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Sonnabend"
      };
      time_t fNow = now();
      tmElements_t fTime;
      breakTime(fNow, fTime);
      String fString = dayName[fTime.Wday - 1];
      LCD_PRINT_LINE(CENTER, 0, fString, fString.length());
      
      fString = "";
      fString.concat(fTime.Day);
      fString.concat(". ");
      fString.concat(monthName[fTime.Month - 1]);
      fString.concat(' ');
      fString.concat(year());
      LCD_PRINT_LINE(CENTER, 1, fString, fString.length());
      
      fString = "";
      if (fTime.Hour < 10) fString.concat('0');
      fString.concat(fTime.Hour);
      fString.concat(':');
      if (fTime.Minute < 10) fString.concat('0');
      fString.concat(fTime.Minute);
      fString.concat(':');
      if (fTime.Second < 10) fString.concat('0');
      fString.concat(fTime.Second);
      LCD_PRINT_LINE(CENTER, 2, fString, fString.length());

      String fPending = gSettings.getPendingAlarmOrTimer();
      LCD_PRINT_LINE(CENTER, 3, fPending, fPending.length());
    }
    else
    {
      String fTime = "";
      if (gSettings.getHours() < 10) fTime.concat('0');
      fTime.concat(gSettings.getHours());
      fTime.concat(':');
      if (gSettings.getMinutes() < 10) fTime.concat('0');
      fTime.concat(gSettings.getMinutes());
      fTime.concat(':');
      if (gSettings.getSeconds() < 10) fTime.concat('0');
      fTime.concat(gSettings.getSeconds());
      LCD_PRINT_LINE(0, 2, fTime, fTime.length());
    }
  }
#endif
#endif
#endif
 
//  gSettings.tick();   // setting of the clock 
}

void triggerModeButton(uint8_t aState, uint8_t aPin)
{
  gSettings.setButtonPressed(SettingStates::Mode, aState);
}

void triggerHourMinutButton(uint8_t aState, uint8_t aPin)
{
  uint8_t aButton = 0;
  if (aPin & 1) gSettings.setButtonPressed(SettingStates::Hour, aState);
  if (aPin & 2) gSettings.setButtonPressed(SettingStates::Minute, aState);
}

void triggerAlarmButton(uint8_t aState, uint8_t aPin)
{
  gSettings.setButtonPressed(SettingStates::AlarmBtn, aState);
}

#ifdef TEST_PERIPHERY
void testPeriphery()
{
}
#endif

