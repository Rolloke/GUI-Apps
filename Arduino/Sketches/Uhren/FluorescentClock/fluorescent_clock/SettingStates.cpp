#include "SettingStates.h"
#include "button.h"

#include <Arduino.h>
#include <DS1307RTC.h>


#include "serial_debug.h"
#include "LCD_print.h"


#define BUTTON_COUNT_MAX     255
#define BUTTON_COUNT_PRESSED 4

// \brief constructor with parameters
// \param led1 - led6 define 6 LED pins for common anode pins via resistor to display bit 0 to 5 of seconsd, minutes and hours
// \param commonPin1 - commonPin3 define 3 pins for common cathode of the leds for seconds, minutes and hours
// \param commonLevel HIGH defines common cathode, LOW defines common anode
SettingStates::SettingStates( uint8_t aTonePin)
: mTonePin(aTonePin)
, mModeBlink(0)
, mSetTimeSelect(Time)
, mAlarmMelody(0)
, mLastSeconds(60)
, mAlarmActive(false)
, mTimeChanged(false)
, mChanged(false)
, mState(Time)
, mAlarmMode(Daily)
, mAlarmID(dtINVALID_ALARM_ID)
, mTimerID(dtINVALID_ALARM_ID)
, mTimerStartTime(0)
, mEventTime(0)
, mTimerFunction(0)
, mAlarmFunction(0)
{
  mButtonPressed[Mode]     = false;
  mButtonPressed[Hour]     = false;
  mButtonPressed[Minute]   = false;
  mButtonPressed[AlarmBtn] = false;

  mTime.Second = 1;
  mTime.Minute = 1;
  mTime.Hour   = 0;
  mTime.Day    = 1;
  mTime.Month  = 1;
  mTime.Year   = 0;
  mTime.Wday   = dowSunday;
}

void SettingStates::setAlarmFunction(OnTick_t aF)
{
  mAlarmFunction = aF;
}

void SettingStates::setTimerFunction(OnTick_t aF)
{
  mTimerFunction = aF;
}

void SettingStates::tick(unsigned long fNow)
{
  state fOldState(mState);
  
  if (readButtons())
  {
    
  }
  if (fNow > mEventTime)
  {
    mEventTime = fNow + 1000;
    checkMode();
  }
  
  
  switch (mState)
  {
    case Time:          handleTimeState();         break;
    case Date:          handleDateState();         break;
    case SetTime:       handleSetTimeState();      break;
    case SetAlarm:      handleSetAlarmState();     break;
    case SetAlarmMode:  handleSetAlarmModeState(); break;
    case SetAlarmDay:   handleSetAlarmDayState();  break;
    case SetAlarmMelody:handleSetAlarmMelody();    break;
    case Timer:         handleTimerState();        break;
  }
  
  if (fOldState != mState)
  {
    handleStateChanged(fOldState);
  }
}

bool SettingStates::isChanged()
{
  bool bChanged = mChanged;
  mChanged = false;
  return bChanged;
}

SettingStates::state SettingStates::getState()
{
  return mState;
}

const char* SettingStates::getStateName()
{
  switch (mState)
  {
    case Time:          return "Time";
    case Date:          return "Date";
    case SetTime:
    switch (mSetTimeSelect)
    {
       case SetHourMinute: return "Set Hour / Minute";
       case SetMonthDay:   return "Set Month / Day";
       case SetYear:       return "Set Year";
       default:            return "Set Time";
    } break;
    case SetAlarm:      return "SetAlarm";
    case SetAlarmMode:  return "SetAlarmMode";
    case SetAlarmDay:   return "SetAlarmDay";
    case SetAlarmMelody:return "SetAlarmMelody";
    case Timer:         return "Timer";
    default:            return "invalid";
  }
}

int SettingStates::getHours()
{
  switch(mState)
  {
    case Time:            return hour();
    case Date:            return month();
    case SetAlarm:        return mTime.Hour;
    case Timer:           return mTime.Hour;
    case SetTime:  
    switch (mSetTimeSelect)
    {
  		case SetHourMinute: return mTime.Hour;
      case SetMonthDay:   return mTime.Month;
  		case SetYear:       return 0;
  		default:            return 0;
  	} break;
    default:              return 0;
  }
}

int SettingStates::getMinutes()
{
  switch(mState)
  {
    case Time:              return minute();
    case Date:              return day();
    case SetTime:
    switch (mSetTimeSelect)
    {
  		case SetHourMinute:   return mTime.Minute;
      case SetMonthDay:     return mTime.Day;
  		case SetYear:         return tmYearToY2k(mTime.Year);
  		default:              return 0;
	  } break;
    case SetAlarm:          return mTime.Minute;
    case SetAlarmMode:      return mAlarmMode;
    case SetAlarmDay:       return mTime.Wday+1;
    case SetAlarmMelody:    return mAlarmMelody+1;
    case Timer:             return mTime.Minute;
    default:                return minute();
  }
}

int SettingStates::getSeconds()
{
  switch(mState)
  {
    case Time: return second();
    // Todo! set state bits with LED from shift register and extra output
    case Timer: 
    if (mTimerID != dtINVALID_ALARM_ID)
    {
      return mTime.Second;
    }
    else
    {
      return bit(mState - FirstSetState);
    }
    break;
    case SetTime:
    if (mModeBlink & 1)    return 0;
    switch (mSetTimeSelect)
	  {
       case SetHourMinute: return 1;
       case SetMonthDay:   return 3;
       case SetYear:       return 7;
       default:            return 0;
    } break;
    default:
    if (mModeBlink & 1) return 0;
    else                return bit(mState - FirstSetState);
  }
  return 0;
}

int SettingStates::getAlarmMelody()
{
   return mAlarmMelody;
}

void SettingStates::setButtonPressed(uint8_t aButton, uint8_t aState)
{
  DEBUG_PRINT("button ");
  DEBUG_PRINT(aButton);
  DEBUG_PRINT(" was ");
  DEBUG_PRINTLN(Button::nameOf((Button::eState)aState));

  if (aButton >= 0 && aButton < buttons)
  {
    mButtonPressed[aButton] = aState;
  }
}

bool SettingStates::isButtonPressed(button aBtn, uint8_t aState)
{
  bool fPressed = (mButtonPressed[aBtn] & aState) != 0;
  if (fPressed)
  {
    mChanged = true;
    mButtonPressed[aBtn] = Button::none;
  }
  return fPressed; 
}


bool SettingStates::isDisplayOn()
{
  return true;
}

bool SettingStates::readButtons()
{
  for (int i=0; i<buttons; ++i)
  {
    if (mButtonPressed[i] > Button::released)
    {
      return true;
    }
  }
  return false;
}

void SettingStates::checkMode()
{
  ++mModeBlink;
}

void SettingStates::beep()
{
  int ftime_ms = 50;
  tone(mTonePin, 1000, ftime_ms);
  delay(ftime_ms);
  noTone(mTonePin);
}

void SettingStates::printTime(const char* aText)
{
  DEBUG_PRINT(aText);
  DEBUG_PRINT(mTime.Day);
  DEBUG_PRINT(":");
  DEBUG_PRINT(mTime.Month);
  DEBUG_PRINT(":");
  DEBUG_PRINT(tmYearToCalendar(mTime.Year));
  DEBUG_PRINT(", ");
  DEBUG_PRINT(mTime.Hour);
  DEBUG_PRINT(":");
  DEBUG_PRINT(mTime.Minute);
  DEBUG_PRINT(":");
  DEBUG_PRINTLN(mTime.Second);
#ifdef LCD_PRINTER
  String fString;
  
  if (aText) LCD_PRINT_LINE(0, 2, aText, strlen(aText));
  fString += String(mTime.Day);
  fString += ".";
  fString += String(mTime.Month);
  fString += ".";
  fString += String(tmYearToCalendar(mTime.Year));
  fString += " ";
  fString += String(mTime.Hour);
  fString += ":";
  fString += String(mTime.Minute);
  fString += ":";
  fString += String(mTime.Second);
  LCD_PRINT_LINE(0, 3, fString, fString.length());
#endif
}

void SettingStates::playAlarm()
{
  DEBUG_PRINT("Play Melody");
  DEBUG_PRINT((unsigned int)mAlarmFunction);
  DEBUG_PRINT(",");
  DEBUG_PRINTLN(mAlarmMelody);

  if (mAlarmFunction)
  {
    mAlarmFunction();
  }
}

void SettingStates::readTime()
{
  if (RTC.chipPresent())
  {
     if (!RTC.read(mTime))
     {
       breakTime(now(), mTime);
       printTime("Could not read Time: ");
     }
  }
  else
  {
    breakTime(now(), mTime);
  }
}

void SettingStates::writeTime()
{
  if (RTC.chipPresent())
  {
    RTC.set(makeTime(mTime));
  }
  else
  {
    setTime(makeTime(mTime));
  }
}

void SettingStates::handleStateChanged(state aOldState)
{
  char *fState = getStateName(); 
  DEBUG_PRINT("new state is:");
  DEBUG_PRINTLN(fState);
  
  LCD_PRINT_LINE(CENTER, 3, fState, strlen(fState));

  if (mState == SetTime)
  {
     readTime();
     mSetTimeSelect  = SetHourMinute;
  }
  if (aOldState == SetTime && mState != SetTime && mTimeChanged)
  {
    writeTime();
    mTimeChanged = false;
  }
  if (mState == SetAlarm)
  {
    time_t fTime = Alarm.read(mAlarmID);
    if (fTime != dtINVALID_TIME)
    {
      mTime.Hour   = hour(fTime);
      mTime.Minute = minute(fTime);
      printTime("Got alarm time: ");
    }
    else
    {
      readTime();
    }
  }
  if (mState == Time)
  {
    readTime();
  }
  if (mState == Date)
  {
    readTime();
  }
  if (mState == Timer)
  {
    time_t fTime = Alarm.read(mTimerID);
    if (fTime != dtINVALID_TIME)
    {
      mTime.Hour   = hour(fTime);
      mTime.Minute = minute(fTime);
      printTime("Got timer time: ");
    }
  }

}

void SettingStates::handleTimeState()
{
  int fSecond = second();
  if (fSecond != mLastSeconds)
  {
    mChanged = true;
    mLastSeconds = fSecond;
  }

  if (isButtonPressed(Mode, Button::pressed))
  {
    mState = Date;
  }
}

void SettingStates::handleDateState()
{
  if (isButtonPressed(Mode, Button::released))
  {
    mState = Time;
  }
  
  if (isButtonPressed(Mode, Button::delayed))
  {
    mState = FirstSetState;
  }
}

void SettingStates::handleModeInSetStates()
{
  if (isButtonPressed(Mode, Button::pressed))
  {
    mState =  (mState == Last) ? SetAlarm  : (state) (mState + 1);
    if (mAlarmMode == Weekly && mState == SetAlarmDay)
    {
      mState = (state) (mState + 1);
    }
  }
  if (isButtonPressed(Mode, Button::delayed))
  {
    mState = Date;
  }
}

bool SettingStates::handleHourMinute()
{
    bool fMinute = isButtonPressed(Minute, Button::pressed|Button::delayed|Button::repeated);
    bool fHour   = isButtonPressed(Hour , Button::pressed|Button::delayed|Button::repeated);
    if ( fMinute )
    {
      ++mTime.Minute;
      if (mTime.Minute == 60)
      {
        mTime.Minute = 0;
      }
    }
    else if ( fHour )
    {
      ++mTime.Hour;
      if (mTime.Hour == 24)
      {
        mTime.Hour = 0;
      }
    }

    return fMinute || fHour;
} 


bool SettingStates::handleMonthDay()
{
    bool fMinute = isButtonPressed(Minute, Button::pressed|Button::delayed|Button::repeated);
    bool fHour   = isButtonPressed(Hour , Button::pressed|Button::delayed|Button::repeated);
    if ( fMinute )
    {
      ++mTime.Day;
      if (mTime.Day == 31)
      {
        mTime.Day = 1;
      }
    }
    else if ( fHour )
    {
      ++mTime.Month;
      if (mTime.Month == 13)
      {
        mTime.Month = 1;
      }
    }

    return fMinute || fHour;
} 

bool SettingStates::handleYear()
{
    if ( isButtonPressed(Plus, Button::pressed|Button::delayed|Button::repeated) && tmYearToY2k(mTime.Year) < 63)
    {
      ++mTime.Year;
      return true;
    }
    else if ( isButtonPressed(Minus, Button::pressed|Button::delayed|Button::repeated) && tmYearToY2k(mTime.Year) > 0 )
    {
      --mTime.Year;
      return true;
    }
    return false;
} 

void SettingStates::handleSwitchDateTime()
{
    if (isButtonPressed(DateTime, Button::released))
    {
      ++mSetTimeSelect;
      if (mSetTimeSelect > SetYear)
      {
		    mSetTimeSelect = SetHourMinute;
	    }
      DEBUG_PRINT("set time select: ");
      DEBUG_PRINTLN(mSetTimeSelect);
    }
}

void SettingStates::handleActivateTimer()
{
  if (mTimerID != dtINVALID_ALARM_ID)
  {
    time_t fTime = Alarm.read(mTimerID);
    time_t fDiff = fTime - now() + mTimerStartTime;
    mTime.Hour   = hour(fDiff);
    mTime.Minute = minute(fDiff);
    int fSecond  = second(fDiff);
    if (fSecond  != mTime.Second) mChanged = true;
    mTime.Second = fSecond;
    if (isButtonPressed(Stop, Button::released))
    {
      char sz[] = "Timer Stopped";
      LCD_PRINT_LINE(0, 3, sz, sizeof(sz));
      DEBUG_PRINTLN(sz);

      Alarm.free(mTimerID);
      mTimerID = dtINVALID_ALARM_ID;
    }
  }
  else
  {
    handleHourMinute();
    if (isButtonPressed(Start, Button::released))
    {
      char sz[] = "Timer Started";
      LCD_PRINT_LINE(0, 3, sz, sizeof(sz));
      DEBUG_PRINTLN(sz);

      printTime("Timer:");
      mTimerID        = Alarm.timerOnce(mTime.Hour, mTime.Minute, 0, mTimerFunction);
      mTimerStartTime = now();
    }
  } 
}

void SettingStates::stopTimer()
{
  Alarm.free(mTimerID);
  time_t fDiff = now() - mTimerStartTime;
  mTime.Hour   = hour(fDiff);
  mTime.Minute = minute(fDiff);
  mTimerID     = dtINVALID_ALARM_ID;
}

void SettingStates::handleActivateAlarm()
{
  if (isButtonPressed(AlarmBtn, Button::released))
  {
    mAlarmActive = !mAlarmActive;
    if (mAlarmActive)
    {
      Alarm.free(mAlarmID);
      
      DEBUG_PRINT("Function: ");
      DEBUG_PRINTLN((long)mAlarmFunction);
      
      switch (mAlarmMode)
      {
       case Weekly: 
         {
           mPendingAlarm = "Weekly Alarm: ";
           mPendingAlarm.concat(dowSunday+mTime.Wday);
           mPendingAlarm.concat(", ");
           if (mTime.Hour < 10) mPendingAlarm.concat('0');
           mPendingAlarm.concat(mTime.Hour);
           mPendingAlarm.concat(':');
           if (mTime.Minute < 10) mPendingAlarm.concat('0');
           mPendingAlarm.concat(mTime.Minute);
           
           LCD_PRINT_LINE(0, 3, mPendingAlarm, mPendingAlarm.length());
           DEBUG_PRINTLN(mPendingAlarm);
         }
         mAlarmID = Alarm.alarmRepeat((timeDayOfWeek_t)(dowSunday+mTime.Wday), mTime.Hour, mTime.Minute, 0, mAlarmFunction);
         break;
       case Once:
         {
           mPendingAlarm = "Single Alarm: ";
           if (mTime.Hour < 10) mPendingAlarm.concat('0');
           mPendingAlarm.concat(mTime.Hour);
           mPendingAlarm.concat(':');
           if (mTime.Minute < 10) mPendingAlarm.concat('0');
           mPendingAlarm.concat(mTime.Minute);
           
           LCD_PRINT_LINE(0, 3, mPendingAlarm, mPendingAlarm.length());
           DEBUG_PRINTLN(mPendingAlarm);
         }
         mAlarmID = Alarm.alarmOnce(mTime.Hour, mTime.Minute, 0, mAlarmFunction);
         break;
       case Daily:
         {
           mPendingAlarm = "Repeated Alarm: ";
           if (mTime.Hour < 10) mPendingAlarm.concat('0');
           mPendingAlarm.concat(mTime.Hour);
           mPendingAlarm.concat(':');
           if (mTime.Minute < 10) mPendingAlarm.concat('0');
           mPendingAlarm.concat(mTime.Minute);
           
           LCD_PRINT_LINE(0, 3, mPendingAlarm, mPendingAlarm.length());
           DEBUG_PRINTLN(mPendingAlarm);
         }
         mAlarmID = Alarm.alarmRepeat(mTime.Hour, mTime.Minute, 0, mAlarmFunction); 
         break;
      }
    }
    else
    {
       char sz[] = "Alarm inactive";
       LCD_PRINT_LINE(0, 3, sz, sizeof(sz));
       DEBUG_PRINTLN(sz);

       mPendingAlarm = "";
       Alarm.disable(mAlarmID);
    }
  }
}

void SettingStates::handleSetTimeState()
{
  handleModeInSetStates();
  bool bH_M_pressed = false;
  switch (mSetTimeSelect)
  {
	  case SetHourMinute: bH_M_pressed = handleHourMinute(); break;
	  case SetMonthDay:   bH_M_pressed = handleMonthDay();   break;
	  case SetYear:       bH_M_pressed = handleYear();       break;
  }
  if (bH_M_pressed)
  {
    mTimeChanged = true;
  }
  handleSwitchDateTime();
}

void SettingStates::handleSetAlarmState()
{
  handleModeInSetStates();
  handleHourMinute();
  handleActivateAlarm();
}

void SettingStates::handleSetAlarmModeState()
{
  handleModeInSetStates();
  bool fChanged = false;
  if (isButtonPressed(Plus, Button::pressed) && mAlarmMode != LastAlarmMode)
  {
    mAlarmMode = (alarm_mode) (mAlarmMode + 1);
    fChanged = true;
  }
  else if (isButtonPressed(Minus, Button::pressed) && mAlarmMode != FirstAlarmMode)
  {
    mAlarmMode = (alarm_mode) (mAlarmMode - 1);
    fChanged = true;
  }
  if (fChanged)
  {  
    String fMsg = "Alarm Mode:";
    fMsg.concat(mAlarmMode);
    LCD_PRINT_LINE(0, 3, fMsg, fMsg.length());
    DEBUG_PRINTLN(fMsg);
  }
  handleActivateAlarm();
}

void SettingStates::handleSetAlarmDayState()
{
  handleModeInSetStates();
  bool fChanged = false;
  if (isButtonPressed(Plus, Button::pressed) && mTime.Wday < dowSaturday)
  {
    mTime.Wday++;
    fChanged = true;
  }
  if (isButtonPressed(Minus, Button::pressed)&& mTime.Wday > dowSunday)
  {
    mTime.Wday--;
    fChanged = true;
  }

  if (fChanged)
  {  
    String fMsg = "Alarm Day:";
    fMsg.concat(dayStr(mTime.Wday));
    LCD_PRINT_LINE(0, 3, fMsg, fMsg.length());
    DEBUG_PRINTLN(fMsg);
  }
  handleActivateAlarm();
}

String SettingStates::getPendingAlarmOrTimer()
{
  if (mTimerID != dtINVALID_ALARM_ID)
  {
    time_t fTime  = now() - mTimerStartTime;
    uint8_t fHour   = hour(fTime);
    uint8_t fMinute = minute(fTime);
    uint8_t fSecond = second(fTime);
    String sTime = "Timer:";
    if (fHour < 10) sTime.concat('0');
    sTime.concat(fHour);
    sTime.concat(':');
    if (fMinute < 10) sTime.concat('0');
    sTime.concat(fMinute);
    sTime.concat(':');
    if (fSecond < 10) sTime.concat('0');
    sTime.concat(fSecond);
    return sTime;
  }
  return mPendingAlarm;
}

void SettingStates::handleTimerState()
{
  handleModeInSetStates();
  handleActivateTimer();
}

void SettingStates::handleSetAlarmMelody()
{
  handleModeInSetStates();
  bool fChanged = false;
  if (isButtonPressed(Plus, Button::pressed) && mAlarmMelody < 3)
  {
    ++mAlarmMelody;
    fChanged = true;
    playAlarm();
  }
  else if (isButtonPressed(Minus, Button::pressed) && mAlarmMelody > 0)
  {
    --mAlarmMelody;
    fChanged = true;
    playAlarm();
  }
  if (fChanged)
  {  
    String fMsg = "Alarm Melody:";
    fMsg.concat(mAlarmMelody);
    LCD_PRINT_LINE(0, 3, fMsg, fMsg.length());
    DEBUG_PRINTLN(fMsg);
  } 
}


