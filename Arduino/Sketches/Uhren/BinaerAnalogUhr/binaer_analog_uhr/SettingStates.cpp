#include "SettingStates.h"
#include "Melody.h"
#include <Arduino.h>
#include <DS1307RTC.h>
#include <button.h>


//#define SOFTWARE_SERIAL

#ifdef SOFTWARE_SERIAL
#include <serialinterface.h>
#endif

#define TICK_INTERVAL_MS     500
#define LIGHT_ON_TIME        10   // 10 * TICK_INTERVAL_MS is 5 Seconds

// \brief constructor
SettingStates::SettingStates()
: mTonePin(0)
, mModeBlink(Active)
, mModeLight(0)
, mLastTickTime(0)
, mSetTimeSelect(Time)
, mAlarmMelody(0)
, mAlarmActive(false)
, mTimeChanged(false)
, mState(Time)
, mAlarmMode(Daily)
, mAlarmID(dtINVALID_ALARM_ID)
, mTimerID(dtINVALID_ALARM_ID)
, mTimerStartTime(0)
, mTimerFunction(0)
, mAlarmFunction(0)
{
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

void SettingStates::setTonePin(uint8_t aPin)
{
    mTonePin = aPin;
}
void SettingStates::setBlinkMode(uint8_t aMode)
{
    mModeBlink = aMode;
}

void SettingStates::triggerButton(button aButton, uint8_t aState)
{
  switch (aButton)
  {
    case ClearAll:
      mButtonState[Mode] = mButtonState[Hour] = mButtonState[Minute] = mButtonState[AlarmBtn] = aState;
      break;
    default:
      mButtonState[aButton] = aState;
      break;
  }
}

void SettingStates::tick(unsigned long fNow)
{
  if (fNow > mLastTickTime)
  {
    mLastTickTime = fNow + TICK_INTERVAL_MS;
    
    if (mModeBlink & Active)
    {
        mModeBlink ^= LED_Bit;
    }
    if (mModeLight > Inactive)
    {
     --mModeLight;
    }
  }
}

void SettingStates::onTrigger()
{
  state fOldState(mState);
  if (isButtonPressed(Mode) || isButtonPressed(Hour) || isButtonPressed(Minute) || isButtonPressed(AlarmBtn))
  {
	  mModeLight = LIGHT_ON_TIME;
  }
#ifdef SOFTWARE_SERIAL
   Serial.print("Mode:");
   Serial.print(isButtonPressed(Mode));
   Serial.print(":Hour:");
   Serial.print(isButtonPressed(Hour));
   Serial.print("Min:");
   Serial.print(isButtonPressed(Minute));
   Serial.print("Alarm:");
   Serial.println(isButtonPressed(AlarmBtn));
#endif
  
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

const char* SettingStates::getStateName()
{
  switch (mState)
  {
    case Time:          return "Time";
    case Date:          return "Date";
    case SetTime:       return "SetTime";
    case SetAlarm:      return "SetAlarm";
    case SetAlarmMode:  return "SetAlarmMode";
    case SetAlarmDay:   return "SetAlarmDay";
    case SetAlarmMelody:return "SetAlarmMelody";
    case Timer:         return "Timer";
    default:            return "invalid";
  }
}

uint8_t SettingStates::getState()
{
    return mState;
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
    case SetAlarmMode:      return bit(mAlarmMode);
    case SetAlarmDay:       return bit(mTime.Wday-1);
    case SetAlarmMelody:    return bit(mAlarmMelody);
    case Timer:             return mTime.Minute;
    default:                return minute();
  }
}

int SettingStates::getSeconds()
{
  switch(mState)
  {
    case Time: return second();
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
    if (mModeBlink & LED_Bit) return 0;
    switch (mSetTimeSelect)
	  {
       case SetHourMinute: return 1;
       case SetMonthDay:   return 3;
       case SetYear:       return 7;
       default:            return 0;
    } break;
    case Date:             return year();
    default:
    if (mModeBlink & LED_Bit) return 0;
    else                return bit(mState - FirstSetState);
  }
  return 0;
}

int SettingStates::getAlarmMelody()
{
   return mAlarmMelody;
}

bool SettingStates::isButtonPressed(button aBtn)
{
  return (mButtonState[aBtn] & (Button::pressed | Button::delayed | Button::repeated)) != 0;
}

uint8_t SettingStates::getButtonState(button aBtn)
{
  return mButtonState[aBtn];
}


bool SettingStates::isLightOn()
{
	return mModeLight > 0;
}

bool SettingStates::isLED_DisplayOn()
{
  return true;
}

bool SettingStates::isAnalogDisplayOn()
{
  return true;
}

void SettingStates::beep()
{
  int ftime_ms = 50;
  tone(mTonePin, NOTE_C6, ftime_ms);
  delay(ftime_ms);
  noTone(mTonePin);
}

void SettingStates::printTime(const char* aText)
{
#ifdef SOFTWARE_SERIAL 
  Serial.print(aText);
  Serial.print(mTime.Day);
  Serial.print(":");
  Serial.print(mTime.Month);
  Serial.print(":");
  Serial.print(tmYearToCalendar(mTime.Year));
  Serial.print(", ");
  Serial.print(mTime.Hour);
  Serial.print(":");
  Serial.print(mTime.Minute);
  Serial.print(":");
  Serial.println(mTime.Second);
#else

#endif
}
void SettingStates::playAlarm()
{
#ifdef SOFTWARE_SERIAL 
  Serial.print("Play Melody");
  Serial.print((unsigned int)mAlarmFunction);
  Serial.print(",");
  Serial.println(mAlarmMelody);
#endif  
  if (mAlarmFunction)
  {
    mAlarmFunction();
  }
}

void SettingStates::handleStateChanged(state aOldState)
{
#ifdef SOFTWARE_SERIAL    
  Serial.print("new state is:");
  Serial.println(getStateName());
#endif

  if (mState == SetTime)
  {
     if (!RTC.read(mTime))
     {
       mTime.Hour    = 0;
       mTime.Minute  = 0;
       mTime.Month   = 1;
       mTime.Day     = 1;
     }
     printTime("Got Time: ");
     mSetTimeSelect  = SetHourMinute;
  }
  if (aOldState == SetTime && mState != SetTime && mTimeChanged)
  {
    printTime("Setting new time: ");
    RTC.set(makeTime(mTime));
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
      RTC.read(mTime);
    }
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
  if (isButtonPressed(Mode))
  {
    mState = Date;
  }
}

void SettingStates::handleDateState()
{
  if (getButtonState(Mode) == Button::pressed)
  {
    mState = Time;
  }

  if (getButtonState(Mode) == Button::delayed)
  {
    mState = FirstSetState;
  }
}

void SettingStates::handleModeInSetStates()
{
  if (getButtonState(Mode) == Button::pressed)
  {
    mState =  (mState == Last) ? SetAlarm  : (state) (mState + 1);
    if (mAlarmMode == Weekly && mState == SetAlarmDay)
    {
      mState = (state) (mState + 1);
    }
  }
  if (getButtonState(Mode) == Button::delayed)
  {
    mState = Time;
  }
}

void SettingStates::handleHourMinute()
{
    if ( isButtonPressed(Hour) && isButtonPressed(Minute))
    {
      mTime.Minute = mTime.Hour = mTime.Second = 0;
    }
    else if ( isButtonPressed(Minute))
    {
      ++mTime.Minute;
    }
    else if ( isButtonPressed(Hour) )
    {
      ++mTime.Hour;
    }
} 


void SettingStates::handleMonthDay()
{
    if ( isButtonPressed(Hour) && isButtonPressed(Minute))
    {
      mTime.Month = mTime.Day = 0;
    }
    else if ( isButtonPressed(Minute))
    {
      ++mTime.Day;
    }
    else if ( isButtonPressed(Hour) )
    {
      ++mTime.Month;
    }
} 

void SettingStates::handleYear()
{
    if ( isButtonPressed(Plus) && tmYearToY2k(mTime.Year) < 63)
    {
      ++mTime.Year;
    }
    else if ( isButtonPressed(Minus) && tmYearToY2k(mTime.Year) > 0 )
    {
      --mTime.Year;
    }
} 

void SettingStates::handleSwitchDateTime()
{
    if (isButtonPressed(DateTime))
    {
      ++mSetTimeSelect;
      if (mSetTimeSelect > SetYear)
      {
		    mSetTimeSelect = SetHourMinute;
	    }
#ifdef SOFTWARE_SERIAL 
      Serial.print("set time select: ");
      Serial.println(mSetTimeSelect);
#endif
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
    mTime.Second = second(fDiff);
    if (isButtonPressed(Stop))
    {
      //gLCD.setCursor(0, 3);
      //gLCD.print("Timer Stopped   ");
#ifdef SOFTWARE_SERIAL    
      Serial.println("Timer Stopped");
#endif
      Alarm.free(mTimerID);
      mTimerID = dtINVALID_ALARM_ID;
    }
  }
  else
  {
    handleHourMinute();
    if (isButtonPressed(Start))
    {
      //gLCD.setCursor(0, 3);
      //gLCD.print("Timer Started   ");
#ifdef SOFTWARE_SERIAL    
      Serial.println("Timer Started");
#endif
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
  if (isButtonPressed(AlarmBtn))
  {
    mAlarmActive = !mAlarmActive;
    if (mAlarmActive)
    {
      Alarm.free(mAlarmID);
#ifdef SOFTWARE_SERIAL    
      Serial.print("Function: ");
      Serial.println((long)mAlarmFunction);
#endif
      switch (mAlarmMode)
      {
       case Weekly: 
         //gLCD.setCursor(0, 3);
         //gLCD.print("Weekly Alarm    ");
#ifdef SOFTWARE_SERIAL    
         Serial.println("Weekly Alarm Activated");
#endif
         mAlarmID = Alarm.alarmRepeat((timeDayOfWeek_t)(dowSunday+mTime.Wday), mTime.Hour, mTime.Minute, 0, mAlarmFunction);
         break;
       case Once:
         //gLCD.setCursor(0, 3);
         //gLCD.print("One shot Alarm  ");
#ifdef SOFTWARE_SERIAL    
         Serial.println("Single Alarm Activated");
#endif
         mAlarmID = Alarm.alarmOnce(mTime.Hour, mTime.Minute, 0, mAlarmFunction);
         break;
       case Daily:
         //gLCD.setCursor(0, 3);
         //gLCD.print("Daily Alarm     ");
#ifdef SOFTWARE_SERIAL    
         Serial.println("Repeated Alarm Activated");
#endif
         mAlarmID = Alarm.alarmRepeat(mTime.Hour, mTime.Minute, 0, mAlarmFunction); 
         break;
      }
    }
    else
    {
       //gLCD.setCursor(0, 3);
       //gLCD.print("Alarm inactive  ");
#ifdef SOFTWARE_SERIAL    
       Serial.println("Alarm inactive");
#endif
       Alarm.disable(mAlarmID);
    }
  }
}

void SettingStates::handleSetTimeState()
{
  handleModeInSetStates();
  switch (mSetTimeSelect)
  {
	  case SetHourMinute: handleHourMinute(); break;
	  case SetMonthDay:   handleMonthDay();   break;
	  case SetYear:       handleYear();       break;
  }
  if (isButtonPressed(Plus) || isButtonPressed(Minus))
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
  if (isButtonPressed(Plus) && mAlarmMode != LastAlarmMode)
  {
    mAlarmMode = (alarm_mode) (mAlarmMode + 1);
#ifdef SOFTWARE_SERIAL
    Serial.print("Alarm Mode:");    
    Serial.println(mAlarmMode);
#endif
  }
  if (isButtonPressed(Minus)&& mAlarmMode != FirstAlarmMode)
  {
    mAlarmMode = (alarm_mode) (mAlarmMode - 1);
#ifdef SOFTWARE_SERIAL
    Serial.print("Alarm Mode:");    
    Serial.println(mAlarmMode);
#endif
  }
  handleActivateAlarm();
}

void SettingStates::handleSetAlarmDayState()
{
  handleModeInSetStates();
  if (isButtonPressed(Plus) && mTime.Wday < dowSaturday)
  {
    mTime.Wday++;
#ifdef SOFTWARE_SERIAL
    Serial.print("Alarm Day:");    
    Serial.println(mTime.Wday);
#endif
  }
  if (isButtonPressed(Minus)&& mTime.Wday > dowSunday)
  {
    mTime.Wday--;
#ifdef SOFTWARE_SERIAL
    Serial.print("Alarm Day:");    
    Serial.println(mTime.Wday);
#endif
  }
  handleActivateAlarm();
}

void SettingStates::handleTimerState()
{
  handleModeInSetStates();
  handleActivateTimer();
}

void SettingStates::handleSetAlarmMelody()
{
  handleModeInSetStates();
  if (isButtonPressed(Plus) && mAlarmMelody < 3)
  {
    ++mAlarmMelody;
#ifdef SOFTWARE_SERIAL
    Serial.print("Alarm Melody:");    
    Serial.println(mAlarmMelody);
#endif
    playAlarm();
  }
  else if (isButtonPressed(Minus) && mAlarmMelody > 0)
  {
    --mAlarmMelody;
#ifdef SOFTWARE_SERIAL
    Serial.print("Alarm Melody:");    
    Serial.println(mAlarmMelody);
#endif
    playAlarm();
  }
}


