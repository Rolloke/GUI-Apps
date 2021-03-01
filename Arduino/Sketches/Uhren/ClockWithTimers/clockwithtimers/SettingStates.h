#ifndef SettingStates_h
#define SettingStates_h

#include <inttypes.h>

#include <Time.h>
#include <TimeAlarms.h>


#define NO_OF_TONES 4

#define AlarmButtonModeForUSB 0


// TODO: test to disable functionalities

#define SELECT 0    // select language EN or DE
#define EN     1    // only english
#define DE     2    // only german

#define LanguageSelection EN
#define MeasureElectricPower 1
#define MeasureElectricWork 0

// TODO reduce floating point operations to minimum

class SettingStates
{
public:
    enum state  { Time, Date, Timer, Timers=NO_OF_TONES, MeasureTemperature=Timer+Timers, MeasureVoltage, MeasureCurrent,
#if MeasureElectricPower
                  MeasurePower,
#endif
                  SetTime, SetDate, SetYear, SetContrast, SetLightLow, SetLightHigh, SetOnLightTime,
#if LanguageSelection == SELECT
                  SetLanguage,
#endif
                  SetMeasurementsActive, StoreTime,
                  SetAlarm, SetAlarmMode, SetAlarmDay, SetAlarmMelody,
                  FirstMeasurementState = MeasureTemperature,
#if MeasureElectricPower == 1
                  LastState             = MeasurePower,
                  LastMeasurementState  = MeasurePower,
#else
                  LastState             = MeasureCurrent,
                  LastMeasurementState  = MeasureCurrent,
#endif
                  LastSettingsState     = SetMeasurementsActive,
                  LastSetAlarmState     = SetAlarmDay
                };
    // Mo, Mi, Ho, Al -> mode, minute, hour, alarm
    enum button { Mode=0, Hour=1, Minute=2, AlarmBtn=3, buttons=4, Plus=Hour, Minus=Minute, Start=AlarmBtn, Stop=AlarmBtn, DateTime = AlarmBtn, ClearAll=10};
    enum alarm_mode  { Once, Daily, Weekly, FirstAlarmMode=Once, LastAlarmMode=Weekly };
    enum consts      { Active = 0x80, LED_Bit = 0x40, COUNTER_BITS=0x3f, Disabled=0xff, MinTemperature=-40, MaxTemperature=125, MaxAnalogValue = 1023,
                       MeasureVoltageActive=1, MeasureCurrentActive=2, MeasureTemperatureActive=4};
    enum calibrate   { Off, CalibrateCurrent, CalibrateTemperature, SetLowerTemperature, SetUpperTemperature, CalibrateVoltage, ResetCalibrationValue, ResetWorkValue};
    
    static const char degreeC = 0xDF;  // ° character for LCD


    SettingStates();

    void    setAlarmFunction(OnTick_t aF);
    void    setTimerFunction(OnTick_t aF);
    void    setTonePin(uint8_t aPin);
    void    setBlinkMode(uint8_t aMode);
    void    setMeasureCurrentPins(uint8_t aVddPin, uint8_t aAnalogInPin);
    void    setMeasureTemperaturePin(uint8_t aAnalogInPin);
    void    setMeasureVoltagePin(uint8_t aAnalogInPin);

    bool    isButtonPressed(button aBtn) const;
    uint8_t getButtonState(button aBtn) const;
    String  getStateName() ;
    uint8_t getState() const;
#if LanguageSelection == SELECT
    uint8_t getLanguage() const;
#endif
    String  getAlarmModeName() const;
    String  getAlarmDayName() const;
    String  getActiveMeasurements() const;

    uint8_t isLightOn() const;
#if AlarmButtonModeForUSB == 1
    bool    isLED_DisplayOn() const;
    bool    isAnalogDisplayOn() const;
#endif

    bool    isTimerActive(int aIndex = -1) const;
    bool    isTimerState() const;
    bool    isAlarmActive() const;
    bool    hasDisplayChanged() ;
    uint8_t isCalibrating() const;
    bool    isStateAvailable() const;
    bool    isMeasurementActive() const;

    int     onTimerAlarm();
    int     getSeconds() const;
    int     getMinutes() const;
    int     getHours() const;
    int     getAlarmMelody() const;
    int     getContrast() const;
    float   getUSBCurrentValue_mA() const;
    float   getUSBVoltageValue_V() const;
    float   getUSBPowerValue_W() const;
#if MeasureElectricWork ==1
    float   getUSBWorkValue_Ws() const;
#endif
    float   getTemperatureValue() const;
    int8_t  getTemperatureAlarmActive() const;

    void stopTimer(int aIndex, time_t aTimerStartTime=dtINVALID_TIME);
    void triggerButton(button aButton, uint8_t aState);
    void onTrigger();
    void tick(unsigned long fNow);
    void printTime(const char* aText);

private:
    bool readButtons();
    void playAlarm();

#if AlarmButtonModeForUSB == 1
    bool isMeasurementState() const;
    void beep();
#endif

    void handleStateChanged();
    bool handleSetPlusMinus(uint8_t& aVar, uint8_t aMin, uint8_t aMax);
    bool handleSetPlusMinus(int16_t& aVar, int16_t aMin, int16_t aMax);
    bool handleCalibration(long& aVar);
    void handleEnterState(state aState);
    void handleExitState(state aState);

    void handleTimeState();
    void handleDateState();
    void handleTimerState();
    void handleMeasureCurrrentState();
    void handleMeasureTemperatureState();
    void handleMeasureVoltageState();

    void handleActivateTimer();
    void handleActivateAlarm();
    int  getTimerIndex() const;
    int  getEEPROMsize() const;
    void storeToEEPROM() const;

    void handleSettingsState();
    void handleHourMinute();
    void handleMonthDay();
    void handleYear();
    void handleContrast();
    void handleLightLow();
    void handleLightHigh();
#if LanguageSelection == SELECT
    void handleLanguage();
#endif
    void handleSetMeasurementActive();
    void handleLightOnTime();

    void handleSetAlarmState();
    void handleSetAlarmModeState();
    void handleSetAlarmDayState();
    void handleSetAlarmMelody();

    void handleModeInSetTimeStates();
    void handleModeInSetAlarmStates();
    void handleModeInTimeStates();

    uint8_t mButtonState[buttons];
    uint8_t mTonePin;
    uint8_t mVddPulsePin;
    uint8_t mMeasureCurrentInPin;
    uint8_t mMeasureTemperatureInPin;
    uint8_t mMeasureVoltageInPin;
    int8_t  mModeBlink;
    int8_t  mModeLight;
    uint8_t mAlarmMelody;
    uint16_t mReadAnalogCurrent;
    uint16_t mReadAnalogVoltage;
#if MeasureElectricWork ==1
    float    mUSB_Work;
#endif
    unsigned long mLastTickTime;
    bool    mAlarmActive;
    int8_t  mTemperatureAlarmActive;
    bool    mTimeChanged;
    bool    mSettingsChanged;
    bool    mDisplayChanged;
    bool    mRTC;
    uint8_t mCalibration;
    tmElements_t mTime;
    state        mState;
    alarm_mode   mAlarmMode;
    AlarmId  mAlarmID;
    AlarmId  mTimerID[Timers];
    time_t   mTimerStartTime[Timers];
    OnTick_t mTimerFunction;
    OnTick_t mAlarmFunction;
    String   mTimerName;
    // stored in EEPROM
    long     mCalibrateCurrentValue;
    long     mCalibrateTemperatureValue;
    long     mCalibrateVoltageValue;
    int16_t  mLowerTemperatureTreshold;
    int16_t  mUpperTemperatureTreshold;
    uint8_t  mContrast;
    uint8_t  mLightLow;
    uint8_t  mLightHigh;
    uint8_t  mLanguage;
    uint8_t  mLightOnTime;
    uint8_t  mActiveFlag;
    // end
};

#endif
