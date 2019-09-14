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
  enum button { Mode=0, Hour=1, Minute=2, AlarmBtn=3, buttons=4, Plus=Hour, Minus=Minute, Start=AlarmBtn, Stop=AlarmBtn, DateTime = AlarmBtn};
  enum alarm_mode  { Once, Daily, Weekly, FirstAlarmMode=Once, LastAlarmMode=Weekly }; 
  enum set_time    { SetHourMinute, SetMonthDay, SetYear };
  SettingStates( uint8_t aBtnMode, uint8_t aBtnHour, uint8_t aBtnMinute, uint8_t aBtnAlarm, uint8_t aTonePin=0 );

  void setAlarmFunction(OnTick_t aF);
  void setTimerFunction(OnTick_t aF);
  
  bool isButtonPressed(button aBtn);
  bool isButtonReleased(button aBtn);
  state getState();
  const char* getStateName();

  bool isLightOn();
  bool isLED_DisplayOn();
  bool isAnalogDisplayOn();
  
  int getSeconds();
  int getMinutes();
  int getHours();
  int  getAlarmMelody();

  void stopTimer();
  void tick();
  
private:
  SettingStates();
  bool readButtons();
  void checkMode();
  void playAlarm();
  void beep();
  void printTime(const char* aText);
  void handleStateChanged(state aOldState);
  void handleTimeState(); 
  void handleDateState();
  void handleSetTimeState();
  void handleSetAlarmState(); 
  void handleSetAlarmModeState(); 
  void handleSetAlarmDayState(); 
  void handleSetAlarmMelody(); 
  void handleTimerState(); 

  void handleHourMinute();
  void handleMonthDay();
  void handleYear();
  void handleSwitchDateTime();  
  void handleModeInSetStates();
  void handleActivateTimer(); 
  void handleActivateAlarm(); 
  
  uint8_t mButtonPin[buttons];
  uint8_t mButtonState[buttons];
  bool    mPreviousButtonState[buttons];
  uint8_t mTonePin;
  uint8_t mModeCount;
  uint8_t mModeSwitchToSettings;
  uint8_t mModeSwitchToDate;
  uint8_t mModeSwitchStates;
  uint8_t mModeBlink;
  uint8_t mModeLight;
  uint8_t mSetTimeSelect;
  uint8_t mAlarmMelody;
  bool    mAlarmActive;
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
