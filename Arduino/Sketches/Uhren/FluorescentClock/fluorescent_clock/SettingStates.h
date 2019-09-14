#ifndef SettingStates_h
#define SettingStates_h

#include <inttypes.h>

#include <Arduino.h>

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
  SettingStates(uint8_t aTonePin=0 );

  void setAlarmFunction(OnTick_t aF);
  void setTimerFunction(OnTick_t aF);
  
  void setButtonPressed(uint8_t aButton, uint8_t aState);
  bool isButtonPressed(button aBtn, uint8_t aState);
  
  state getState();
  const char* getStateName();
  String getPendingAlarmOrTimer();

  bool isDisplayOn();
  bool isChanged();
  
  int getSeconds();
  int getMinutes();
  int getHours();
  int getAlarmMelody();

  void stopTimer();
  void tick(unsigned long fNow);
  
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

  bool handleHourMinute();
  bool handleMonthDay();
  bool handleYear();
  void handleSwitchDateTime();  
  void handleModeInSetStates();
  void handleActivateTimer(); 
  void handleActivateAlarm();
  void readTime();
  void writeTime();
  
  uint8_t mButtonPressed[buttons];
  uint8_t mTonePin;
  uint8_t mModeBlink;
  uint8_t mSetTimeSelect;
  uint8_t mAlarmMelody;
  uint8_t mLastSeconds;
  bool    mAlarmActive;
  bool    mTimeChanged;
  bool    mChanged;
  tmElements_t mTime;
  state        mState;
  alarm_mode   mAlarmMode;
  AlarmId  mAlarmID;
  AlarmId  mTimerID;
  time_t   mTimerStartTime;
  unsigned long mEventTime;
  OnTick_t mTimerFunction;
  OnTick_t mAlarmFunction;
  String mPendingAlarm;
};

#endif
