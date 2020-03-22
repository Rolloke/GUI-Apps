#include "SettingStates.h"
#include <Arduino.h>
#include <DS1307RTC.h>
#include <button.h>


#define TICK_INTERVAL_MS     500
#define LIGHT_ON_TIME        10   // 10 * TICK_INTERVAL_MS is 5 Seconds

// TODO: start and stop timer with alarm button
// BUG: time is resetted, when alarm is suspended
// BUG: timer is changed during time settings
// TODO: switch to alarm timer, when alarm is reached and show, that this alarm is now activated


// \brief constructor
SettingStates::SettingStates()
: mTonePin(0)
, mModeBlink(Active)
, mModeLight(0)
, mLastTickTime(0)
, mAlarmMelody(0)
, mAlarmActive(false)
, mTimeChanged(false)
, mDisplayChanged(true)
, mState(Time)
, mAlarmMode(Daily)
, mAlarmID(dtINVALID_ALARM_ID)
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
  for (int i=0; i<Timers; ++i)
  {
      mTimerID[i]        = dtINVALID_ALARM_ID;
      mTimerStartTime[i] = 0;
  }
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
      mDisplayChanged = true;
      mButtonState[aButton] = aState;
      break;
  }
}

void SettingStates::tick(unsigned long fNow)
{
  if (fNow > mLastTickTime)
  {
    mLastTickTime = fNow + TICK_INTERVAL_MS;
    onTrigger();
    mDisplayChanged = true;

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
  Alarm.delay(0);
  state fOldState(mState);
  if (isButtonPressed(Mode) || isButtonPressed(Hour) || isButtonPressed(Minute) || isButtonPressed(AlarmBtn))
  {
	  mModeLight = LIGHT_ON_TIME;
  }
  
  switch (mState)
  {
    case Time:          handleTimeState();         break;
    case Date:          handleDateState();         break;
    case SetTime: case SetDate: case SetYear:
                        handleSetTimeState();
      break;
    case StoreTime:
      if (getButtonState(Mode) == Button::released) mState = Time;
      break;
    case SetAlarm:      handleSetAlarmState();     break;
    case SetAlarmMode:  handleSetAlarmModeState(); break;
    case SetAlarmDay:   handleSetAlarmDayState();  break;
    case SetAlarmMelody:handleSetAlarmMelody();    break;
    default:
        if (isTimerState())
        {
            handleTimerState();
        }
        break;
  }
  if (fOldState != mState)
  {
    handleStateChanged();
  }
}

String SettingStates::getStateName()
{
  switch (mState)
  {
#if LANGUAGE == EN
    case Time:          return "Time";
    case Date:          return "Date";
    case SetTime:       return "Set Time";
    case SetDate:       return "Set Date";
    case SetYear:       return "Set Year";
    case StoreTime:     return "Store Time";
    case SetAlarm:      return "Set Alarm";
    case SetAlarmMode:  return "Set AlarmMode";
    case SetAlarmDay:   return "Set AlarmDay";
    case SetAlarmMelody:return "Set AlarmMelody";
    default:
      if (isTimerState())
      {
          mTimerName = "Timer " + String(getTimerIndex()+1);
          return mTimerName;
      }
      else return "invalid";
  }
#else
  case Time:          return "Uhrzeit";
  case Date:          return "Datum";
  case SetTime:       return "Uhrzeit stellen";
  case SetDate:       return "Datum stellen";
  case SetYear:       return "Jahr stellen";
  case StoreTime:     return "Speichern";
  case SetAlarm:      return "Alarm stellen";
  case SetAlarmMode:  return "Alarm Modus";
  case SetAlarmDay:   return "Alarm Tag";
  case SetAlarmMelody:return "Alarm Melodie";
  default:
    if (isTimerState())
    {
        mTimerName = "Timer " + String(getTimerIndex()+1);
        return mTimerName;
    }
    else return "invalid";
}
#endif
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
    case SetTime:         return mTime.Hour;
    case SetDate:         return mTime.Month;
    case SetYear:         return 0;
    default:
      if(isTimerState())  return mTime.Hour;
      else                return 0;
  }
}

int SettingStates::getMinutes()
{
  switch(mState)
  {
    case Time:              return minute();
    case Date:              return day();
    case SetTime:           return mTime.Minute;
    case SetDate:           return mTime.Day;
    case SetYear:           return tmYearToY2k(mTime.Year);
    case SetAlarm:          return mTime.Minute;
    case SetAlarmMode:      return mAlarmMode;
    case SetAlarmDay:       return mTime.Wday-1;
    case SetAlarmMelody:    return mAlarmMelody;
    default:
      if(isTimerState())    return mTime.Minute;
      else return minute();
  }
}

int SettingStates::getSeconds()
{
  switch(mState)
  {
    case Time: return second();
    case Date: return year();
    case SetYear: return tmYearToCalendar(mTime.Year);
    case SetTime: case SetDate: return 0;
    default:
      if (isTimerState())
      {
          if (mTimerID[getTimerIndex()] != dtINVALID_ALARM_ID)
          {
            return mTime.Second;
          }
          else
          {
            return 0;
          }
      }
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
  tone(mTonePin, 1000, ftime_ms);
  delay(ftime_ms);
  noTone(mTonePin);
}

void SettingStates::playAlarm()
{
  if (mAlarmFunction)
  {
    mAlarmFunction();
  }
}

void SettingStates::handleStateChanged()
{
  if (mState == SetTime)
  {
      if (! mTimeChanged)
      {
          if (RTC.chipPresent())
          {
             if (!RTC.read(mTime))
             {
               mTime.Hour    = 0;
               mTime.Minute  = 0;
               mTime.Month   = 1;
               mTime.Day     = 1;
             }
          }
          else
          {
              breakTime(now(), mTime);
          }
      }
  }

  if (mState == SetAlarm)
  {
    time_t fTime = Alarm.read(mAlarmID);
    if (fTime != dtINVALID_TIME)
    {
      mTime.Hour   = hour(fTime);
      mTime.Minute = minute(fTime);
      mTime.Second = second(fTime);
    }
    else
    {
      RTC.read(mTime);
    }
  }

  if (isTimerState())
  {
    time_t fTime = Alarm.read(mTimerID[getTimerIndex()]);
    if (fTime != dtINVALID_TIME)
    {
      mTime.Hour   = hour(fTime);
      mTime.Minute = minute(fTime);
      mTime.Second = second(fTime);
    }
    else
    {
        mTime.Hour   = 0;
        mTime.Minute = 0;
        mTime.Second = 0;
    }
  }

}

void SettingStates::handleTimeState()
{
  handleModeInTimeStates();
}

void SettingStates::handleDateState()
{
  handleModeInTimeStates();
}

void SettingStates::handleTimerState()
{
    handleActivateTimer();
    handleModeInTimeStates();
}

void SettingStates::handleModeInSetTimeStates()
{
  if (getButtonState(Mode) == Button::released)
  {
    mState = (state) (mState + 1);
    if (mState > SetYear)
    {
        mState = SetTime;
    }
  }
  else if (getButtonState(Mode) == Button::delayed)
  {
      if (mTimeChanged)
      {
          mState = StoreTime;
          if (RTC.chipPresent())
          {
            RTC.set(makeTime(mTime));
          }
          setTime(mTime.Hour, mTime.Minute, mTime.Second, mTime.Day, mTime.Month, tmYearToY2k(mTime.Year));
          mTimeChanged = false;
      }
      else
      {
          mState = Time;
      }
  }
}

void SettingStates::handleModeInSetAlarmStates()
{
  if (getButtonState(Mode) == Button::released)
  {
    mState = (state) (mState + 1);
    if (mAlarmMode == Weekly && mState == SetAlarmDay)
    {
      mState = (state) (mState + 1);
    }
    if (mState > LastSetState)
    {
        mState = SetAlarm;
    }
  }
  else if (getButtonState(Mode) == Button::delayed)
  {
    mState = Time;
  }
}

void SettingStates::handleModeInTimeStates()
{
    if (getButtonState(Mode) == Button::released)
    {
        mState = static_cast<state>(mState + 1);
        if (mState == (Timer+Timers))
        {
            mState = Time;
        }
    }
    else if (getButtonState(Mode) == Button::delayed)
    {
      mState = SetTime;
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
      if (++mTime.Minute == 60)
      {
          mTime.Minute = 0;
      }
    }
    else if ( isButtonPressed(Hour) )
    {
      if (++mTime.Hour == 24)
      {
          mTime.Hour = 0;
      }
    }
} 


void SettingStates::handleMonthDay()
{
    if ( isButtonPressed(Hour) && isButtonPressed(Minute))
    {
      mTime.Month = mTime.Day = 1;
    }
    else if ( isButtonPressed(Minute))
    {
      if (++mTime.Day == 32)
      {
          mTime.Day = 1;
      }

    }
    else if ( isButtonPressed(Hour) )
    {
      if (++mTime.Month == 13)
      {
          mTime.Month = 1;
      }
    }
} 

void SettingStates::handleYear()
{
    if ( isButtonPressed(Plus) && mTime.Year < 255)
    {
      ++mTime.Year;
    }
    else if ( isButtonPressed(Minus) && mTime.Year > 0 )
    {
      --mTime.Year;
    }
} 

void SettingStates::handleActivateTimer()
{
  if (isTimerActive())
  {
    time_t fTime = Alarm.read(mTimerID[getTimerIndex()]);
    time_t fDiff = fTime - now() + mTimerStartTime[getTimerIndex()];
    mTime.Hour   = hour(fDiff);
    mTime.Minute = minute(fDiff);
    mTime.Second = second(fDiff);
    if (isButtonPressed(Stop))
    {
      stopTimer(getTimerIndex());
    }
  }
  else
  {
    handleHourMinute();
    if (isButtonPressed(Start))
    {
      Alarm.free(mTimerID[getTimerIndex()]);
      mTimerID[getTimerIndex()] = Alarm.timerOnce(mTime.Hour, mTime.Minute, 0, mTimerFunction);
      Alarm.enable(mTimerID[getTimerIndex()]);
      mTimerStartTime[getTimerIndex()] = now();
    }
  } 
}

int SettingStates::onTimerAlarm()
{
    AlarmID_t fID = Alarm.getTriggeredAlarmId();
    for (int i=0; i<Timers; ++i)
    {
        if (fID == mTimerID[i])
        {
            stopTimer(i);
            return i;
        }
    }
    if (mAlarmID == fID)
    {
        return Timers;
    }
    return -1;
}

void SettingStates::stopTimer(int aIndex)
{
  time_t fTime = Alarm.read(mTimerID[aIndex]);
  mTimerStartTime[aIndex] = fTime;
  mTime.Hour       = hour(fTime);
  mTime.Minute     = minute(fTime);
  mTime.Second     = second(fTime);
  Alarm.free(mTimerID[aIndex]);
  mTimerID[aIndex] = dtINVALID_ALARM_ID;
}

int SettingStates::getTimerIndex()
{
    return mState - Timer;
}

bool SettingStates::isTimerState()
{
    return mState >= Timer && mState < (Timer+Timers);
}

bool SettingStates::isTimerActive()
{
    return isTimerState() && mTimerID[getTimerIndex()] != dtINVALID_ALARM_ID;
}

bool SettingStates::isAlarmActive()
{
    return mAlarmActive;
}

bool SettingStates::hasDisplayChanged()
{
    bool fChanged = mDisplayChanged;
    mDisplayChanged = false;
    return fChanged;
}

void SettingStates::handleActivateAlarm()
{
  if (isButtonPressed(AlarmBtn))
  {
    mAlarmActive = !mAlarmActive;
    if (mAlarmActive)
    {
      Alarm.free(mAlarmID);
      switch (mAlarmMode)
      {
       case Weekly: 
         mAlarmID = Alarm.alarmRepeat((timeDayOfWeek_t)(dowSunday+mTime.Wday), mTime.Hour, mTime.Minute, 0, mAlarmFunction);
         break;
       case Once:
         mAlarmID = Alarm.alarmOnce(mTime.Hour, mTime.Minute, 0, mAlarmFunction);
         break;
       case Daily:
         mAlarmID = Alarm.alarmRepeat(mTime.Hour, mTime.Minute, 0, mAlarmFunction); 
         break;
      }
      // bug: fix alarm functionality
      mAlarmActive = Alarm.isAlarm(mAlarmID);
    }
    else
    {
       Alarm.disable(mAlarmID);
    }
  }
}

void SettingStates::handleSetTimeState()
{
  handleModeInSetTimeStates();

  switch (mState)
  {
      case SetTime: handleHourMinute(); break;
      case SetDate: handleMonthDay();   break;
      case SetYear: handleYear();       break;
      default: break;
  }
  if (isButtonPressed(Plus) || isButtonPressed(Minus))
  {
    mTimeChanged = true;
  }
  if (isButtonPressed(DateTime))
  {
      mState = SetAlarm;
  }
}

void SettingStates::handleSetAlarmState()
{
  handleModeInSetAlarmStates();
  handleHourMinute();
  handleActivateAlarm();
}

void SettingStates::handleSetAlarmModeState()
{
  handleModeInSetAlarmStates();
  if (isButtonPressed(Plus) && mAlarmMode != LastAlarmMode)
  {
    mAlarmMode = (alarm_mode) (mAlarmMode + 1);
  }
  if (isButtonPressed(Minus)&& mAlarmMode != FirstAlarmMode)
  {
    mAlarmMode = (alarm_mode) (mAlarmMode - 1);
  }
  handleActivateAlarm();
}

void SettingStates::handleSetAlarmDayState()
{
  handleModeInSetAlarmStates();
  if (isButtonPressed(Plus) && mTime.Wday < dowSaturday)
  {
    mTime.Wday++;
  }
  if (isButtonPressed(Minus)&& mTime.Wday > dowSunday)
  {
    mTime.Wday--;
  }
  handleActivateAlarm();
}


void SettingStates::handleSetAlarmMelody()
{
  handleModeInSetAlarmStates();
  if (isButtonPressed(Plus) && mAlarmMelody < 3)
  {
    ++mAlarmMelody;
    playAlarm();
  }
  else if (isButtonPressed(Minus) && mAlarmMelody > 0)
  {
    --mAlarmMelody;
    playAlarm();
  }
}
