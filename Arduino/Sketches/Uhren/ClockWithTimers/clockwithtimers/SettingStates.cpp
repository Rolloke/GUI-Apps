#include "SettingStates.h"
#include <Arduino.h>
#include <DS1307RTC.h>
#include <button.h>
#include <TimeAlarms.h>
#include <EEPROM.h>

const char  gEEPROMid[]        = "ClockVar1.6";
const float gAnalogCalibration = SettingStates::MaxAnalogValue * 10.0f;

#if LanguageSelection == SELECT
 #define SELECT_ENGLISH if (getLanguage()== EN)
 #define SELECT_GERMAN  else

#else
 #define SELECT_ENGLISH
 #define SELECT_GERMAN
#endif


#define TICK_INTERVAL_MS     500

// \brief constructor
SettingStates::SettingStates()
    : mTonePin(0)
    , mVddPulsePin(0)
    , mMeasureCurrentInPin(0)
    , mMeasureTemperatureInPin(0)
    , mMeasureVoltageInPin(0)
    , mModeBlink(Active)
    , mModeLight(0)
    , mAlarmMelody(0)
    , mReadAnalogCurrent(0)
    , mReadAnalogVoltage(0)
    #if MeasureElectricWork ==1
    , mUSB_Work(0)
    #endif
    , mLastTickTime(0)
    , mAlarmActive(false)
    , mTemperatureAlarmActive(0)
    , mTimeChanged(false)
    , mSettingsChanged(false)
    , mDisplayChanged(true)
    , mRTC(false)
    , mCalibration(Off)
    , mState(Time)
    , mAlarmMode(Daily)
    , mAlarmID(dtINVALID_ALARM_ID)
    , mTimerFunction(0)
    , mAlarmFunction(0)
    , mVDDswitchedOn(false)
    , mCalibrateCurrentValue(MaxAnalogValue*10)
    , mCalibrateTemperatureValue(MaxAnalogValue*10)
    , mCalibrateVoltageValue(MaxAnalogValue*10)
    , mLowerTemperatureTreshold(0)
    , mUpperTemperatureTreshold(100)
    , mContrast(128)
    , mLightLow(64)
    , mLightHigh(200)
    , mLanguage(EN)
    , mLightOnTime(50)
    , mActiveFlag(MeasureCurrentActive|MeasureVoltageActive|MeasureTemperatureActive)
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

    int i, len = strlen(gEEPROMid);

    for (i=0; i<len; ++i)
    {
        char fByte = EEPROM.read(i);
        if (fByte != gEEPROMid[i]) break;
    }

    if (i==len)
    {
        len = getEEPROMsize();
        char *fBlock = (char*) &mCalibrateCurrentValue;
        for (int j=0; j<len; ++j, ++i)
        {
            fBlock[j] = EEPROM.read(i);
        }
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

void SettingStates::setMeasureCurrentPins(uint8_t aVddPin, uint8_t aAnalogInPin)
{
    mVddPulsePin = aVddPin;
    pinMode(mVddPulsePin, OUTPUT);
    mMeasureCurrentInPin = aAnalogInPin;
    pinMode(mMeasureCurrentInPin, INPUT);
}

void SettingStates::setMeasureTemperaturePin(uint8_t aAnalogInPin)
{
    mMeasureTemperatureInPin = aAnalogInPin;
    pinMode(mMeasureTemperatureInPin, INPUT);
}

void SettingStates::setMeasureVoltagePin(uint8_t aAnalogInPin)
{
    mMeasureVoltageInPin = aAnalogInPin;
    pinMode(mMeasureVoltageInPin, INPUT);
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

        if (mMeasureTemperatureInPin && mCalibration == Off)
        {
            const float fTemperature = getTemperatureValue();
            if (fTemperature != -100)
            {
                if (fTemperature > mUpperTemperatureTreshold)
                {
                    if (!mTemperatureAlarmActive)
                    {
                        mState = MeasureTemperature;
                        mTemperatureAlarmActive = +1;
                        playAlarm();
                    }
                }
                else if (fTemperature < mLowerTemperatureTreshold)
                {
                    if (!mTemperatureAlarmActive)
                    {
                        mState = MeasureTemperature;
                        mTemperatureAlarmActive = -1;
                        playAlarm();
                    }
                }
                else if (mTemperatureAlarmActive != 0)
                {
                    mState = Time;
                    mTemperatureAlarmActive = 0;
                    playAlarm();
                }
            }
        }

        if (mModeBlink & Active)
        {
            mModeBlink ^= LED_Bit;
        }
        if (mModeLight > 0)
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
        mModeLight = mLightOnTime;
    }


    switch (mState)
    {
    case Time:               handleTimeState();               break;
    case Date:               handleDateState();               break;
    case MeasureTemperature: handleMeasureTemperatureState(); break;
    case MeasureCurrent:     handleMeasureCurrrentState();    break;
    case MeasureVoltage:
#if MeasureElectricPower
    case MeasurePower:
        handleMeasureVoltageState();
        handleMeasureCurrrentState();
        break;
#endif
    case StoreTime:
        if (getButtonState(Mode) == Button::released) mState = Time;
        break;
    case SetAlarm:           handleSetAlarmState();           break;
    case SetAlarmMode:       handleSetAlarmModeState();       break;
    case SetAlarmDay:        handleSetAlarmDayState();        break;
    case SetAlarmMelody:     handleSetAlarmMelody();          break;
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
        handleExitState(fOldState);
        handleEnterState(mState);
        handleStateChanged();
    }
}

String SettingStates::getStateName()
{
#if LanguageSelection == EN || LanguageSelection == SELECT
    SELECT_ENGLISH
    {
        switch (mState)
        {
        case Time:          return F("Time");
        case Date:          return F("Date");
        case MeasureCurrent:return F("USB-Current");
        case MeasureTemperature:return F("Temperature");
        case MeasureVoltage:return F("USB-Voltage");
#if MeasureElectricPower == 1
        case MeasurePower:  return F("USB-Power");
#endif
        case SetTime:       return F("Set Time");
        case SetDate:       return F("Set Date");
        case SetYear:       return F("Set Year");
        case SetContrast:   return F("Set Contrast");
        case SetLightLow:   return F("Lightness dimmed");
        case SetLightHigh:  return F("Lightness on");
        case SetOnLightTime:  return F("Light on Time");
        case StoreTime:
        {
            String fTxt = F("Store");
            if (mRTC) fTxt += F(" to RTC");
            return fTxt;
        }
        case SetAlarm:      return F("Set Alarm");
        case SetAlarmMode:  return F("Set AlarmMode");
        case SetAlarmDay:   return F("Set AlarmDay");
        case SetAlarmMelody:return F("Set AlarmMelody");
#if LanguageSelection == SELECT
        case SetLanguage:   return F("Language English");
#endif
        case SetMeasurementsActive:return F("Measurements");
        default:
            if (isTimerState())
            {
                mTimerName = String(F("Timer ")) + String(getTimerIndex()+1);
                return mTimerName;
            }
            else return F("invalid");
        }
    }
#endif

#if LanguageSelection == DE || LanguageSelection == SELECT
    SELECT_GERMAN
    {
        switch (mState)
        {
        case Time:          return F("Uhrzeit");
        case Date:          return F("Datum");
        case MeasureCurrent:return F("USB-Strom");
        case MeasureTemperature:return F("Temperatur");
        case MeasureVoltage:return F("USB-Spannung");
#if MeasureElectricPower == 1
        case MeasurePower:  return F("USB-Leistung");
#endif
        case SetTime:       return F("Uhrzeit stellen");
        case SetDate:       return F("Datum stellen");
        case SetYear:       return F("Jahr stellen");
        case SetContrast:   return F("Kontrast");
        case SetLightLow:   return F("Licht dunkel");
        case SetLightHigh:  return F("Licht hell");
        case SetOnLightTime:  return F("Licht hell Zeit");
        case StoreTime:
        {
            String fTxt = F("Speichern");
            if (mRTC) fTxt += F(" in RTC");
            return fTxt;
        }
        case SetAlarm:      return F("Alarm stellen");
        case SetAlarmMode:  return F("Alarm Modus");
        case SetAlarmDay:   return F("Alarm Tag");
        case SetAlarmMelody:return F("Alarm Melodie");
#if LanguageSelection == SELECT
        case SetLanguage:   return F("Sprache deutsch");
#endif
        case SetMeasurementsActive:return F("Messungen");
        default:
            if (isTimerState())
            {
                mTimerName = String(F("Timer ")) + String(getTimerIndex()+1);
                return mTimerName;
            }
            else return F("Invalide");
        }
    }
#endif
}

String SettingStates::getAlarmModeName()  const
{
    String fName;
#if LanguageSelection == EN || LanguageSelection == SELECT
    SELECT_ENGLISH
    {
        fName = F("Mode: ");
        switch (getMinutes())
        {
        case SettingStates::Once:   fName += F("Once");   break;
        case SettingStates::Daily:  fName += F("Daily");  break;
        case SettingStates::Weekly: fName += F("Weekly"); break;
        default:break;
        }
    }
#endif

#if LanguageSelection == DE || LanguageSelection == SELECT
    SELECT_GERMAN
    {
        fName += "Modus: ";
        switch (getMinutes())
        {
        case SettingStates::Once:   fName += F("einmal");     break;
        case SettingStates::Daily:  fName += F("taeglich");    break;
        case SettingStates::Weekly: fName += F("woechentlich");break;
        default:break;
        }
    }
#endif
    return fName;
}

String SettingStates::getAlarmDayName() const
{
    String fName;
#if LanguageSelection == EN || LanguageSelection == SELECT
    SELECT_ENGLISH
    {
        fName = (dayStr(getMinutes()));
    }
#endif

#if LanguageSelection == DE || LanguageSelection == SELECT
    SELECT_GERMAN
    {
        switch (getMinutes())
        {
        case dowSunday:    fName = F("Sonntag"); break;
        case dowMonday:    fName = F("Montag"); break;
        case dowTuesday:   fName = F("Dienstag"); break;
        case dowWednesday: fName = F("Mittwoch"); break;
        case dowThursday:  fName = F("Donnerstag"); break;
        case dowFriday:    fName = F("Freitag"); break;
        case dowSaturday:  fName = F("Sonnabend"); break;
        default: break;
        }
    }
#endif
    return fName;
}

String SettingStates::getActiveMeasurements() const
{
    String measurement ="|";
    if (mActiveFlag&MeasureTemperatureActive)
    {
        measurement += degreeC;
        measurement += F("C|");
    }
    if (mActiveFlag&MeasureVoltageActive) measurement += F("V|");
    if (mActiveFlag&MeasureCurrentActive) measurement += F("C|");
    return measurement;
}

uint8_t SettingStates::getState() const
{
    return mState;
}

#if LanguageSelection == SELECT
uint8_t SettingStates::getLanguage() const
{
    return mLanguage;
}
#endif

int SettingStates::getHours() const
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

int SettingStates::getMinutes() const
{
    switch(mState)
    {
    case Time:              return minute();
    case Date:              return day();
    case SetTime:           return mTime.Minute;
    case SetDate:           return mTime.Day;
    case SetYear:           return tmYearToCalendar(mTime.Year);
    case SetAlarm:          return mTime.Minute;
    case SetAlarmMode:      return mAlarmMode;
    case SetAlarmDay:       return mTime.Wday;
    case SetAlarmMelody:    return mAlarmMelody;
    case SetContrast:       return 255 - mContrast;
    case SetLightLow:       return mLightLow;
    case SetLightHigh:      return mLightHigh;
    case SetOnLightTime:    return mLightOnTime;
    default:
        if(isTimerState())    return mTime.Minute;
        else return minute();
    }
}

int SettingStates::getSeconds() const
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

int SettingStates::getAlarmMelody() const
{
    return mAlarmMelody;
}

int SettingStates::getContrast() const
{
    return mContrast;
}

float SettingStates::getUSBCurrentValue_mA() const
{
    const float fFactor = gAnalogCalibration / mCalibrateCurrentValue;
    return (((float)mReadAnalogCurrent) * fFactor);
}

float SettingStates::getUSBVoltageValue_V() const
{
    const float fFactor = gAnalogCalibration * 0.01 / mCalibrateVoltageValue;
    return (((float)mReadAnalogVoltage) * fFactor);
}

float SettingStates::getUSBPowerValue_W() const
{
    return getUSBVoltageValue_V() * getUSBCurrentValue_mA() * 0.001;
}

#if MeasureElectricWork ==1
float SettingStates::getUSBWorkValue_Ws() const
{
    return mUSB_Work;
}
#endif

float SettingStates::getTemperatureValue() const
{
    const int16_t fValueTable[] =
    {  // values for temperatures in °C in 5 °C steps from -40
       1022,
       1022,
       1022,
       1022,
       1022,
       1006,
       998,
       953,
       902,
       845,
       784,
       720,
       654,
       589,
       525,
       465,
       409,
       357,
       311,
       270,
       234,
       202,
       175,
       151,
       130,
       113,
       98,
       85,
       74,
       64,
       56,
       49,
       43,
       38
    };

    switch (mCalibration)
    {
    case SetLowerTemperature: return mLowerTemperatureTreshold;
    case SetUpperTemperature: return mUpperTemperatureTreshold;
    case ResetCalibrationValue:
    case CalibrateTemperature:
    default:
        {
            int16_t fValue = analogRead(mMeasureTemperatureInPin);
            const int fEnd = sizeof(fValueTable) / sizeof(int16_t) - 1;
            if (fValue <= fValueTable[0])
            {
                for (int i=fEnd-1; i>0; --i)
                {
                    if (fValueTable[i] >= fValue)
                    {
                        const int16_t aFromLow = fValueTable[i];
                        const int16_t aFromHigh= fValueTable[i+1];
                        const int16_t aToLow = (i-1) * 5 - 40;
                        float fTemperature = (fValue - aFromLow) * 5.0f / (aFromHigh - aFromLow) + aToLow;
                        //return fValue;
                        return fTemperature * gAnalogCalibration / mCalibrateTemperatureValue;
                    }
                }
            }
        }
        break;
    }
    return -100;
}


int8_t SettingStates::getTemperatureAlarmActive() const
{
    return mTemperatureAlarmActive;
}

bool SettingStates::isButtonPressed(button aBtn) const
{
    return (mButtonState[aBtn] & (Button::pressed | Button::delayed | Button::repeated)) != 0;
}

uint8_t SettingStates::getButtonState(button aBtn) const
{
    return mButtonState[aBtn];
}


uint8_t SettingStates::isLightOn() const
{
    return map(mModeLight, 0, mLightOnTime, mLightLow, mLightHigh);
}

#if AlarmButtonModeForUSB == 1
bool SettingStates::isLED_DisplayOn() const
{
    return true;
}

bool SettingStates::isAnalogDisplayOn() const
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
#endif

void SettingStates::playAlarm()
{
    if (mAlarmFunction)
    {
        mAlarmFunction();
    }
}
void SettingStates::handleEnterState(state new_state)
{
    if (isMeasurementState(new_state)
        && ((mActiveFlag & MeasureCurrentActive) != 0
        && analogRead(mMeasureCurrentInPin) != 0) )
    {
        mVDDswitchedOn = enableVDD(true);
    }
}

void SettingStates::handleExitState(state old_state)
{
    if ( isMeasurementState(old_state) && !isMeasurementState(mState))
    {
        mVDDswitchedOn = enableVDD(false);

        if (mCalibration != Off)
        {
            storeToEEPROM();
            mCalibration = Off;
        }
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
                    breakTime(now(), mTime);
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


void SettingStates::handleMeasureTemperatureState()
{
    handleModeInTimeStates();
    switch(mCalibration)
    {
    case CalibrateTemperature:handleCalibration(mCalibrateTemperatureValue); break;
    case SetLowerTemperature: handleSetPlusMinus(mLowerTemperatureTreshold, MinTemperature, mUpperTemperatureTreshold); break;
    case SetUpperTemperature: handleSetPlusMinus(mUpperTemperatureTreshold, mLowerTemperatureTreshold, MaxTemperature ); break;
    case ResetCalibrationValue:
        if (getButtonState(AlarmBtn) == Button::released)
        {
            mCalibrateTemperatureValue = MaxAnalogValue*10;
        }
        break;
    }
}

void SettingStates::handleMeasureCurrrentState()
{
    handleModeInTimeStates();
    switch (mCalibration)
    {
    case CalibrateCurrent: handleCalibration(mCalibrateCurrentValue); break;
    case ResetCalibrationValue:
        if (getButtonState(AlarmBtn) == Button::released)
        {
            mCalibrateCurrentValue = MaxAnalogValue*10;
        }
        break;
#if MeasureElectricWork ==1
    case ResetWorkValue:
        if (getButtonState(AlarmBtn) == Button::released)
        {
            mUSB_Work = 0;
            mCalibration = Off;
        }
        break;
#endif
    }
    if (mActiveFlag & MeasureCurrentActive)
    {
        mReadAnalogCurrent = analogRead(mMeasureCurrentInPin);
#if MeasureElectricWork ==1
        float work = getUSBPowerValue_W() * TICK_INTERVAL_MS * 0.001;
        mUSB_Work += work;
#endif
    }
    else
    {
        mReadAnalogCurrent = 0;
    }
}

void SettingStates::handleMeasureVoltageState()
{
    switch (mCalibration)
    {
    case CalibrateVoltage: handleCalibration(mCalibrateVoltageValue); break;
    case ResetCalibrationValue:
        if (getButtonState(AlarmBtn) == Button::released)
        {
            mCalibrateVoltageValue = MaxAnalogValue*10;
        }
        break;
    }
    if (mActiveFlag & MeasureVoltageActive)
    {
        mReadAnalogVoltage = analogRead(mMeasureVoltageInPin);
    }
    else
    {
        mReadAnalogVoltage = 0;
    }
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
            setTime(mTime.Hour, mTime.Minute, mTime.Second, mTime.Day, mTime.Month, tmYearToCalendar(mTime.Year));
            mTimeChanged = false;
        }
        else
        {
            mState = Time;
        }
        if (mSettingsChanged)
        {
            storeToEEPROM();
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
        do
        {
            mState = static_cast<state>(mState + 1);

            if (mState > LastState)
            {
                mState = Time;
            }
        }
        while (!isStateAvailable());
    }
    else if (getButtonState(Mode) == Button::delayed)
    {
        if (mState == MeasureTemperature)
        {
            switch (mCalibration)
            {
            case Off:                   mCalibration = SetLowerTemperature;   break;
            case SetLowerTemperature:   mCalibration = SetUpperTemperature;   break;
            case SetUpperTemperature:   mCalibration = CalibrateTemperature;  break;
            case CalibrateTemperature:  mCalibration = ResetCalibrationValue; break;
            case ResetCalibrationValue: mCalibration = SetLowerTemperature;   break;
            default:
                break;
            }
        }
        else if (mState == MeasureCurrent || mState == MeasureVoltage)
        {
            switch (mCalibration)
            {
            case Off:                   mCalibration = mState == MeasureCurrent ? CalibrateCurrent : CalibrateVoltage; break;
            case CalibrateCurrent:      // fall through
            case CalibrateVoltage:      mCalibration = ResetCalibrationValue; break;
            }
        }
#if MeasureElectricPower == 1
        else if (mState == MeasurePower)
        {
            mCalibration = ResetWorkValue;
        }
#endif
        else
        {
            mState = SetTime;
            if (!RTC.read(mTime))
            {
                breakTime(now(), mTime);
            }
        }
    }
#if AlarmButtonModeForUSB == 1
    else if (isMeasurementState(mState)
             && getButtonState(AlarmBtn) == Button::released
             && (mCalibration & (ResetWorkValue|ResetCalibrationValue)) == 0)
    {
        do
        {
            mState = (state) (mState + 1);
            if (mState > LastMeasurementState)
            {
                mState = FirstMeasurementState;
                mButtonState[AlarmBtn] = Button::none;
            }
        }
        while (!isStateAvailable());
    }
#endif
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
    if (handleSetPlusMinus(mTime.Year, 0, 255))
    {
        mTimeChanged = true;
    }
} 

void SettingStates::handleContrast()
{
    if (handleSetPlusMinus(mContrast, 0, 255))
    {
        mSettingsChanged = true;
    }
}

void SettingStates::handleLightLow()
{
    if (handleSetPlusMinus(mLightLow, 0, 255))
    {
        mSettingsChanged = true;
    }
}

void SettingStates::handleLightHigh()
{
    if (handleSetPlusMinus(mLightHigh, 0, 255))
    {
        mSettingsChanged = true;
    }
}

#if LanguageSelection == SELECT
void SettingStates::handleLanguage()
{
    if (handleSetPlusMinus(mLanguage, EN, DE))
    {
        mSettingsChanged = true;
    }
}
#endif

void SettingStates::handleSetMeasurementActive()
{
    if (handleSetPlusMinus(mActiveFlag, 0, (MeasureCurrentActive|MeasureTemperatureActive|MeasureVoltageActive)))
    {
        mSettingsChanged = true;
    }
}

void SettingStates::handleLightOnTime()
{
    if (handleSetPlusMinus(mLightOnTime, 5, 100))
    {
        mSettingsChanged = true;
    }
}

bool SettingStates::handleSetPlusMinus(uint8_t& aVar, uint8_t aMin, uint8_t aMax)
{
    if ( isButtonPressed(Plus) && aVar < aMax)
    {
        ++aVar;
        return true;
    }
    else if ( isButtonPressed(Minus) && aVar > aMin )
    {
        --aVar;
        return true;
    }
    return false;
}

bool SettingStates::handleSetPlusMinus(int16_t& aVar, int16_t aMin, int16_t aMax)
{
    if ( isButtonPressed(Plus) && aVar < aMax)
    {
        ++aVar;
        return true;
    }
    else if ( isButtonPressed(Minus) && aVar > aMin )
    {
        --aVar;
        return true;
    }
    return false;
}

bool SettingStates::handleCalibration(long& aVar)
{
    if (isButtonPressed(Plus))
    {
        aVar++;
        if (getButtonState(Plus)&Button::repeated)
        {
            aVar += 50;
        }
        return true;
    }
    else if (isButtonPressed(Minus))
    {
        aVar--;
        if (getButtonState(Minus)&Button::repeated)
        {
            aVar -= 50;
        }
        return true;
    }
    return false;
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
    if (fID != dtINVALID_ALARM_ID)
    {
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

int SettingStates::getTimerIndex()  const
{
    return mState - Timer;
}

int SettingStates::getEEPROMsize() const
{
    //int size = (char*)&mLastEEPROMMemoryMarker - (char*)&mCalibrateCurrentValue;
    int size = sizeof(long) * 3 + sizeof(int16_t) * 2 + sizeof(uint8_t) * 6;
    return size;
}

void SettingStates::storeToEEPROM() const
{
    int i, len = strlen(gEEPROMid);
    for (i=0; i<len; ++i)
    {
        EEPROM.write(i, gEEPROMid[i]);
    }
    if (i==len)
    {
        len = getEEPROMsize();
        char *fBlock = (char*) &mCalibrateCurrentValue;
        for (int j=0; j<len; ++j, ++i)
        {
            EEPROM.write(i, fBlock[j]);
        }
    }
}

bool SettingStates::isTimerState() const
{
    return mState >= Timer && mState < (Timer+Timers);
}

bool SettingStates::isTimerActive(int aIndex) const
{
    if (aIndex == -1) aIndex = getTimerIndex();
    if (aIndex >= 0 && aIndex < Timers)
    {
        return mTimerID[aIndex] != dtINVALID_ALARM_ID;
    }
    return false;
}

bool SettingStates::isAlarmActive() const
{
    return mAlarmActive;
}

uint8_t SettingStates::isCalibrating() const
{
    return mCalibration;
}

bool SettingStates::isStateAvailable() const
{
    switch (mState)
    {
    case MeasureCurrent:     return isMeasurementActive() && analogRead(mMeasureCurrentInPin) != 0;
    case MeasureTemperature: return isMeasurementActive() && analogRead(mMeasureTemperatureInPin) != 1023;
    case MeasureVoltage:     return isMeasurementActive();
#if MeasureElectricPower == 1
    case MeasurePower:       return isMeasurementActive() && analogRead(mMeasureCurrentInPin) != 0;
#endif
    default: return true;
    }
}

bool SettingStates::isMeasurementActive() const
{
    switch (mState)
    {
    case MeasureCurrent:     return (mActiveFlag & MeasureCurrentActive) != 0;
    case MeasureTemperature: return (mActiveFlag & MeasureTemperatureActive) != 0;
    case MeasureVoltage:     return (mActiveFlag & MeasureVoltageActive) != 0;
#if MeasureElectricPower == 1
    case MeasurePower:       return (mActiveFlag & MeasureCurrentActive) != 0 && (mActiveFlag & MeasureVoltageActive) != 0;
#endif
    default: return true;
    }
}

bool SettingStates::isVDDenabled() const
{
    return mVDDswitchedOn;
}

bool SettingStates::enableVDD(bool on)
{
    if (mVddPulsePin)
    {
        if (on)
        {
            tone(mVddPulsePin, 5000);
        }
        else
        {
            noTone(mVddPulsePin);
        }
        return on;
    }
    return false;
}

#if AlarmButtonModeForUSB == 1
bool SettingStates::isMeasurementState(state test_state) const
{
    return test_state >= FirstMeasurementState && test_state <= LastMeasurementState;
}
#endif

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
    case SetTime:               handleHourMinute();  break;
    case SetDate:               handleMonthDay();    break;
    case SetYear:               handleYear();        break;
    case SetContrast:           handleContrast();    break;
    case SetLightLow:           handleLightLow();    break;
    case SetLightHigh:          handleLightHigh();   break;
    case SetOnLightTime:        handleLightOnTime(); break;
#if LanguageSelection == SELECT
    case SetLanguage:           handleLanguage();    break;
#endif
    case SetMeasurementsActive: handleSetMeasurementActive();  break;

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
    uint8_t fAlarmMode = mAlarmMode;
    if (handleSetPlusMinus(fAlarmMode, FirstAlarmMode, LastAlarmMode))
    {
        mAlarmMode = (alarm_mode)fAlarmMode;
    }
    handleActivateAlarm();
}

void SettingStates::handleSetAlarmDayState()
{
    handleModeInSetAlarmStates();
    handleSetPlusMinus(mTime.Wday, dowSunday, dowSaturday);
    handleActivateAlarm();
}


void SettingStates::handleSetAlarmMelody()
{
    handleModeInSetAlarmStates();
    if (handleSetPlusMinus(mAlarmMelody, 0, 3))
    {
        playAlarm();
    }
}
