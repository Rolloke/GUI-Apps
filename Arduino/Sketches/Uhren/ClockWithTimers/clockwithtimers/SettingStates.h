#ifndef SettingStates_h
#define SettingStates_h

#include <inttypes.h>

#include <Time.h>
#include <TimeAlarms.h>

#define EN 1
#define DE 2
#define LANGUAGE EN


class SettingStates
{
public:
  enum state  { Time, Date, Timer, Timers=5, MeasureCurrent=Timer+Timers, SetTime, SetDate, SetYear, SetContrast, SetLightLow, SetLightHigh, SetLanguage, StoreTime,
                SetAlarm, SetAlarmMode, SetAlarmDay, SetAlarmMelody,
                LastState          = MeasureCurrent,
                LastSettingsState  = SetLanguage,
                LastSetAlarmState  = SetAlarmDay };
  // Mo, Mi, Ho, Al -> mode, minute, hour, alarm
  enum button { Mode=0, Hour=1, Minute=2, AlarmBtn=3, buttons=4, Plus=Hour, Minus=Minute, Start=AlarmBtn, Stop=AlarmBtn, DateTime = AlarmBtn, ClearAll=10};
  enum alarm_mode  { Once, Daily, Weekly, FirstAlarmMode=Once, LastAlarmMode=Weekly }; 
  enum consts      { Inactive = 1, Active = 0x80, LED_Bit = 1, Disabled=0xff};
  SettingStates();

  void setAlarmFunction(OnTick_t aF);
  void setTimerFunction(OnTick_t aF);
  void setTonePin(uint8_t aPin);
  void setBlinkMode(uint8_t aMode);
  void setMeasureCurrentPins(uint8_t aVddPin, uint8_t aAnalogInPin);

  bool    isButtonPressed(button aBtn);
  uint8_t getButtonState(button aBtn);
  String  getStateName();
  uint8_t getState();
  uint8_t getLanguage();
  String  getAlarmModeName();
  String  getAlarmDayName();

  bool isLightOn();
  bool isLED_DisplayOn();
  bool isAnalogDisplayOn();
  bool isTimerActive(int aIndex = -1);
  bool isTimerState();
  bool isAlarmActive();
  bool hasDisplayChanged();
  bool isCalibrating();

  int  onTimerAlarm();

  int getSeconds();
  int getMinutes();
  int getHours();
  int getAlarmMelody();
  int getContrast();
  int getLightLow();
  int getLightHigh();
  float getUSBCurrentValue() const;

  void stopTimer(int aIndex, time_t aTimerStartTime=dtINVALID_TIME);
  void triggerButton(button aButton, uint8_t aState);
  void onTrigger();
  void tick(unsigned long fNow);
  void printTime(const char* aText);
  
private:
  bool readButtons();
  void playAlarm();
  void beep();
  void handleStateChanged();
  bool handleSetPlusMinus(uint8_t& aVar, uint8_t aMin, uint8_t aMax);
  void handleEnterState(state aState);
  void handleExitState(state aState);

  void handleTimeState();
  void handleDateState();
  void handleTimerState();
  void handleMeasureCurrrentState();

  void handleActivateTimer();
  void handleActivateAlarm();
  int  getTimerIndex();

  void handleSettingsState();
  void handleHourMinute();
  void handleMonthDay();
  void handleYear();
  void handleContrast();
  void handleLightLow();
  void handleLightHigh();
  void handleLanguage();

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
  int8_t  mModeBlink;
  int8_t  mModeLight;
  uint8_t mContrast;
  uint8_t mLightLow;
  uint8_t mLightHigh;
  uint8_t mAlarmMelody;
  uint8_t mLanguage;
  uint16_t mReadCurrentValue;
  unsigned long mLastTickTime;
  bool    mAlarmActive;
  bool    mTimeChanged;
  bool    mDisplayChanged;
  bool    mRTC;
  bool    mCalibrateCurrent;
  tmElements_t mTime;
  state        mState;
  alarm_mode   mAlarmMode;
  AlarmId  mAlarmID;
  AlarmId  mTimerID[Timers];
  time_t   mTimerStartTime[Timers];
  long     mCalibrateCurrentValue;
  OnTick_t mTimerFunction;
  OnTick_t mAlarmFunction;
  String   mTimerName;
};

#endif
