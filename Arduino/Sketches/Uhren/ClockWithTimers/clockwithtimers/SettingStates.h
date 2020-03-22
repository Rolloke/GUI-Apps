#ifndef SettingStates_h
#define SettingStates_h

#include <inttypes.h>

#include <Time.h>
#include <TimeAlarms.h>

#define EN 1
#define DE 2
#define LANGUAGE DE


class SettingStates
{
public:
  enum state  { Time, Date, Timer, Timers=4, SetTime=Timer+Timers+1, SetDate, SetYear, StoreTime, SetAlarm, SetAlarmMode, SetAlarmDay, SetAlarmMelody,
                LastState     = Timer+Timers,
                FirstSetState = SetAlarm,
                LastSetState  = SetAlarmDay };
  // Mo, Mi, Ho, Al -> mode, minute, hour, alarm
  enum button { Mode=0, Hour=1, Minute=2, AlarmBtn=3, buttons=4, Plus=Hour, Minus=Minute, Start=AlarmBtn, Stop=AlarmBtn, DateTime = AlarmBtn, ClearAll=10};
  enum alarm_mode  { Once, Daily, Weekly, FirstAlarmMode=Once, LastAlarmMode=Weekly }; 
  //enum set_time    { SetHourMinute, SetMonthDay, SetYear };
  enum consts      { Inactive = 1, Active = 0x80, LED_Bit = 1, Disabled=0xff};
  SettingStates();

  void setAlarmFunction(OnTick_t aF);
  void setTimerFunction(OnTick_t aF);
  void setTonePin(uint8_t aPin);
  void setBlinkMode(uint8_t aMode);
  
  bool    isButtonPressed(button aBtn);
  uint8_t getButtonState(button aBtn);
  String  getStateName();
  uint8_t getState();

  bool isLightOn();
  bool isLED_DisplayOn();
  bool isAnalogDisplayOn();
  bool isTimerActive();
  bool isTimerState();
  bool isAlarmActive();
  bool hasDisplayChanged();

  int  onTimerAlarm();

  int getSeconds();
  int getMinutes();
  int getHours();
  int getAlarmMelody();

  void stopTimer(int aIndex);
  void triggerButton(button aButton, uint8_t aState);
  void onTrigger();
  void tick(unsigned long fNow);
  void printTime(const char* aText);
  
private:
  bool readButtons();
  void playAlarm();
  void beep();
  void handleStateChanged();

  void handleTimeState();
  void handleDateState();

  void handleTimerState();
  void handleActivateTimer();
  void handleActivateAlarm();
  int  getTimerIndex();

  void handleSetTimeState();
  void handleHourMinute();
  void handleMonthDay();
  void handleYear();

  void handleSetAlarmState(); 
  void handleSetAlarmModeState(); 
  void handleSetAlarmDayState(); 
  void handleSetAlarmMelody(); 

  void handleModeInSetTimeStates();
  void handleModeInSetAlarmStates();
  void handleModeInTimeStates();

  
  uint8_t mButtonState[buttons];
  uint8_t mTonePin;
  int8_t  mModeBlink;
  int8_t  mModeLight;
  unsigned long mLastTickTime;
  uint8_t mAlarmMelody;
  bool    mAlarmActive;
  bool    mTimeChanged;
  bool    mDisplayChanged;
  tmElements_t mTime;
  state        mState;
  alarm_mode   mAlarmMode;
  AlarmId  mAlarmID;
  AlarmId  mTimerID[Timers];
  time_t   mTimerStartTime[Timers];
  OnTick_t mTimerFunction;
  OnTick_t mAlarmFunction;
  String   mTimerName;
};

#endif
