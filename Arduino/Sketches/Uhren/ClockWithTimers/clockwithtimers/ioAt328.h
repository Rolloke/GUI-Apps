
const int toneOutput      =  8;
const int dimLED          =  9;
const int contrastPin     =  5;
const int vddPulsePin     =  3;

const int measureCurrentPin = 14;

#ifdef LCD_PRINT
#include <LiquidCrystal.h>
LiquidCrystal LCD(7, 6, 4, 2, 1, 0);
#endif

SettingStates  gSettings;

const int gModeBtnPin   = 10;
const int gHourBtnPin   = 11;
const int gMinuteBtnPin = 12;
const int gAlarmBtnPin  = 13;
