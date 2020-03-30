
const int toneOutput      =  9;
const int dimLED          =  8;

SettingStates  gSettings;

#ifdef LCD_PRINTER
#include <LiquidCrystal.h>
//                rs,e, 0, 1, 2, 3
LiquidCrystal LCD(7, 6, 5, 4, 3, 2);
#endif

const int gModeBtnPin   = 22;
const int gHourBtnPin   = 53;
const int gMinuteBtnPin = 26;
const int gAlarmBtnPin  = 24;
