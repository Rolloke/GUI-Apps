#ifndef SettingStates_h
#define SettingStates_h

#include <inttypes.h>

#include <Time.h>
#include <TimeAlarms.h>

class SettingStates
{
public:
  enum state  { Time, Date, SetTime, SetAlarm, SetAlarmMode, SetAlarmDay, SetAlarmMelody, Timer, Last = Timer, FirstSetState = SetTime }; 
  // Mo, Mi, Ho, Al -> mode, minute, hour, alarm
  enum button { Mode=0, Hour=1, Minute=2, AlarmBtn=3, buttons=4, Plus=Hour, Minus=Minute, Start=AlarmBtn, Stop=AlarmBtn, DateTime = AlarmBtn, ClearAll=10};
  enum alarm_mode  { Once, Daily, Weekly, FirstAlarmMode=Once, LastAlarmMode=Weekly }; 
  enum set_time    { SetHourMinute, SetMonthDay, SetYear };
  enum consts      { Inactive = 1, Active = 0x80, LED_Bit = 1};
  SettingStates();

  void setAlarmFunction(OnTick_t aF);
  void setTimerFunction(OnTick_t aF);
  void setTonePin(uint8_t aPin);
  void setBlinkMode(uint8_t aMode);
  
  bool    isButtonPressed(button aBtn);
  uint8_t getButtonState(button aBtn);
  const char* getStateName();
  uint8_t getState();

  bool isLightOn();
  bool isLED_DisplayOn();
  bool isAnalogDisplayOn();
  
  int getSeconds();
  int getMinutes();
  int getHours();
  int getAlarmMelody();

  void stopTimer();
  void triggerButton(button aButton, uint8_t aState);
  void onTrigger();
  void tick(unsigned long fNow);
  void printTime(const char* aText);
  
private:
  bool readButtons();
  void playAlarm();
  void beep();
  void handleStateChanged(state aOldState);
  void handleTimeState(); 
  void handleDateState();
  void handleSetTimeState();
  void handleSetAlarmState(); 
  void handleSetAlarmModeState(); 
  void handleSetAlarmDayState(); 
  void handleSetAlarmMelody(); 
  void handleTimerState(); 
  bool handleSetPlusMinus(uint8_t& aVar, uint8_t aMin, uint8_t aMax);

  void handleHourMinute();
  void handleMonthDay();
  void handleYear();
  void handleSwitchDateTime();  
  void handleModeInSetStates();
  void handleActivateTimer(); 
  void handleActivateAlarm(); 
  
  uint8_t mButtonState[buttons];
  uint8_t mTonePin;
  int8_t  mModeBlink;
  int8_t  mModeLight;
  unsigned long mLastTickTime;
  uint8_t mSetTimeSelect;
  uint8_t mAlarmMelody;
  time_t  mAlarmStartTime;
  bool    mTimeChanged;
  tmElements_t mTime;
  state        mState;
  alarm_mode   mAlarmMode;
  AlarmId  mAlarmID;
  AlarmId  mTimerID;
  time_t   mTimerStartTime;
  OnTick_t mTimerFunction;
  OnTick_t mAlarmFunction;
};

#endif
