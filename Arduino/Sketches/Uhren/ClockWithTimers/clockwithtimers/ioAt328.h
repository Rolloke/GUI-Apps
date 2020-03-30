
const int toneOutput      =  4;
const int dimLED          =  10;

#ifdef LCD_PRINT
#include <LiquidCrystal.h>
LiquidCrystal LCD(13, 12, 8, 7, 6, 5);
#endif

SettingStates  gSettings;

const int gModeBtnPin = A1;
const int gHourBtnPin = A2;
const int gMinuteBtnPin = A3;
const int gAlarmBtnPin = A4;
