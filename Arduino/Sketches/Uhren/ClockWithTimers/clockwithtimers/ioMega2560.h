
const int toneOutput      =   9;
const int dimLED          =   8;
const int contrastPin     =  10;

SettingStates  gSettings;

#ifdef LCD_PRINTER
#include <LiquidCrystal.h>
//                rs,e, 0, 1, 2, 3
LiquidCrystal LCD(50, 48, 32, 34, 36, 38, 40, 42, 44, 46);
#endif

const int gModeBtnPin   = 22;
const int gHourBtnPin   = 53;
const int gMinuteBtnPin = 26;
const int gAlarmBtnPin  = 24;
