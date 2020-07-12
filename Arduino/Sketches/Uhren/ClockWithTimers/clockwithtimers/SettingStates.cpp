#include "SettingStates.h"
#include <Arduino.h>
#include <DS1307RTC.h>
#include <button.h>


#define TICK_INTERVAL_MS     500
#define LIGHT_ON_TIME        10   // 10 * TICK_INTERVAL_MS is 5 Seconds

// \brief constructor
SettingStates::SettingStates()
    : mTonePin(0)
    , mModeBlink(Active)
    , mModeLight(0)
    , mContrast(128)
    , mLightLow(64)
    , mLightHigh(200)
    , mLastTickTime(0)
    , mAlarmMelody(0)
    , mAlarmActive(false)
    , mTimeChanged(false)
    , mDisplayChanged(true)
    , mRTC(false)
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
        mTimerStartTime[i] = dtINVALID_TIME;
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
        else
        {
            handleSettingsState();
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
    case Time:          return F("Time");
    case Date:          return F("Date");
    case SetTime:       return F("Set Time");
    case SetDate:       return F("Set Date");
    case SetYear:       return F("Set Year");
    case SetContrast:   return F("Set Contrast");
    case SetLightLow:   return F("Set Light dimmed");
    case SetLightHigh:  return F("Set Light on");
    case StoreTime:
    {
        String fTxt = F("Store Time");
        if (mRTC) fTxt += F(" to RTC");
        return fTxt;
    }
    case SetAlarm:      return F("Set Alarm");
    case SetAlarmMode:  return F("Set AlarmMode");
    case SetAlarmDay:   return F("Set AlarmDay");
    case SetAlarmMelody:return F("Set AlarmMelody");
    default:
        if (isTimerState())
        {
            mTimerName = String(F("Timer ")) + String(getTimerIndex()+1);
            return mTimerName;
        }
        else return F("invalid");
    }
#else
    case Time:          return "Uhrzeit";
    case Date:          return "Datum";
    case SetTime:       return "Uhrzeit stellen";
    case SetDate:       return "Datum stellen";
    case SetYear:       return "Jahr stellen";
    case SetContrast:   return "Kontrast";
    case SetLightLow:   return "Licht dunkel";
    case SetLightHigh:  return "Light hell";
    case StoreTime:
    {
        String fTxt = F("Speichern");
        if (mRTC) fTxt += F(" nach RTC");
        return fTxt;
    }
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
    case SetAlarmDay:       return mTime.Wday;
    case SetAlarmMelody:    return mAlarmMelody;
    case SetContrast:       return 255 - mContrast;
    case SetLightLow:       return mLightLow;
    case SetLightHigh:      return mLightHigh;
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

int SettingStates::getContrast()
{
    return mContrast;
}

int SettingStates::getLightLow()
{
    return mLightLow;
}

int SettingStates::getLightHigh()
{
    return mLightHigh;
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
    return mModeLight > Inactive;
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
                if (RTC.read(mTime))
                {
                    mRTC = true;
                }
                else
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
            switch (Alarm.readType(mAlarmID))
            {
            case dtExplicitAlarm: mAlarmMode = Once;   break;
            case dtDailyAlarm:    mAlarmMode = Daily;  break;
            case dtWeeklyAlarm:   mAlarmMode = Weekly; break;
//          case dtTimer: break;
            default: break;
            }
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
        int fIndex = getTimerIndex();
        if (mTimerID[fIndex] == dtINVALID_ALARM_ID)
        {
            time_t fTime = mTimerStartTime[fIndex];
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
        if (mState > LastSettingsState)
        {
            mState = SetTime;
        }
    }
    else if (getButtonState(Mode) == Button::delayed)
    {
        if (mTimeChanged)
        {
            mState = StoreTime;
            mRTC = RTC.write(mTime);
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
        if (mAlarmMode != Weekly && mState == SetAlarmDay)
        {
            mState = (state) (mState + 1);
        }
        if (mState > LastSetAlarmState)
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
        mTimeChanged = true;
    }
    else if ( isButtonPressed(Minute))
    {
        if (++mTime.Minute == 60)
        {
            mTime.Minute = 0;
        }
        mTimeChanged = true;
    }
    else if ( isButtonPressed(Hour) )
    {
        if (++mTime.Hour == 24)
        {
            mTime.Hour = 0;
        }
        mTimeChanged = true;
    }
} 


void SettingStates::handleMonthDay()
{
    if ( isButtonPressed(Hour) && isButtonPressed(Minute))
    {
        mTime.Month = mTime.Day = 1;
        mTimeChanged = true;
    }
    else if ( isButtonPressed(Minute))
    {
        if (++mTime.Day == 32)
        {
            mTime.Day = 1;
        }
        mTimeChanged = true;
    }
    else if ( isButtonPressed(Hour) )
    {
        if (++mTime.Month == 13)
        {
            mTime.Month = 1;
        }
        mTimeChanged = true;
    }
} 

void SettingStates::handleYear()
{
    if ( isButtonPressed(Plus) && mTime.Year < 255)
    {
        ++mTime.Year;
        mTimeChanged = true;
    }
    else if ( isButtonPressed(Minus) && mTime.Year > 0 )
    {
        --mTime.Year;
        mTimeChanged = true;
    }
} 

void SettingStates::handleContrast()
{
    if ( isButtonPressed(Plus) && mContrast < 255)
    {
        ++mContrast;
    }
    else if ( isButtonPressed(Minus) && mContrast > 0 )
    {
        --mContrast;
    }
}

void SettingStates::handleLightLow()
{
    if ( isButtonPressed(Plus) && mLightLow < 255)
    {
        ++mLightLow;
    }
    else if ( isButtonPressed(Minus) && mLightLow > 0 )
    {
        --mLightLow;
    }
}

void SettingStates::handleLightHigh()
{
    if ( isButtonPressed(Plus) && mLightHigh < 255)
    {
        ++mLightHigh;
    }
    else if ( isButtonPressed(Minus) && mLightHigh > 0 )
    {
        --mLightHigh;
    }
}

void SettingStates::handleActivateTimer()
{
    int fIndex = getTimerIndex();
    if (isTimerState() && isTimerActive(fIndex))
    {
        time_t fTime = Alarm.read(mTimerID[fIndex]);
        time_t fDiff = fTime - now() + mTimerStartTime[fIndex];
        mTime.Hour   = hour(fDiff);
        mTime.Minute = minute(fDiff);
        mTime.Second = second(fDiff);
        if (isButtonPressed(Stop))
        {
            stopTimer(fIndex);
        }
        // Hint could be nice but does not work like it is implemented
//        else if (isButtonPressed(Plus))
//        {
//            Alarm.enable(mTimerID[fIndex]);
//        }
//        else if (isButtonPressed(Minus))
//        {
//            Alarm.disable(mTimerID[fIndex]);
//        }
    }
    else
    {
        handleHourMinute();
        if (isButtonPressed(Start))
        {
            Alarm.free(mTimerID[fIndex]);
            mTimerID[fIndex] = Alarm.timerOnce(mTime.Hour, mTime.Minute, 0, mTimerFunction);
            Alarm.enable(mTimerID[fIndex]);
            mTimerStartTime[fIndex] = now();
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
            time_t fTime = now() - mTimerStartTime[i];
            stopTimer(i, fTime);
            mState = (state)(Timer + i);
            return i;
        }
    }
    if (mAlarmID == fID)
    {
        return Timers;
    }
    return -1;
}

void SettingStates::stopTimer(int aIndex, time_t aTimerStartTime)
{
    // Hint: time is resetted, when alarm is suspended.
    //
    time_t fTime = dtINVALID_TIME;
    if (Alarm.isAllocated(mTimerID[aIndex]))
    {
        fTime = Alarm.read(mTimerID[aIndex]);
        Alarm.free(mTimerID[aIndex]);
        mTimerStartTime[aIndex] = fTime;
    }
    if (aTimerStartTime != dtINVALID_TIME)
    {
        mTimerStartTime[aIndex] = aTimerStartTime;
        fTime = aTimerStartTime;
    }

    if (fTime != dtINVALID_TIME)
    {
        mTime.Hour       = hour(fTime);
        mTime.Minute     = minute(fTime);
        mTime.Second     = second(fTime);
    }

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

bool SettingStates::isTimerActive(int aIndex)
{
    if (aIndex == -1) aIndex = getTimerIndex();
    if (aIndex >= 0 && aIndex < Timers)
    {
        return mTimerID[aIndex] != dtINVALID_ALARM_ID;
    }
    return false;
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
            mAlarmActive = Alarm.isAlarm(mAlarmID);
        }
        else
        {
            Alarm.disable(mAlarmID);
        }
    }
}

void SettingStates::handleSettingsState()
{
    handleModeInSetTimeStates();

    switch (mState)
    {
    case SetTime:      handleHourMinute();break;
    case SetDate:      handleMonthDay();  break;
    case SetYear:      handleYear();      break;
    case SetContrast:  handleContrast();  break;
    case SetLightLow:  handleLightLow();  break;
    case SetLightHigh: handleLightHigh(); break;
    default: break;
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
    if (isButtonPressed(Plus)  && mTime.Wday < dowSaturday)
    {
        mTime.Wday++;
    }
    if (isButtonPressed(Minus) && mTime.Wday > dowSunday)
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
