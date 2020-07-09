
const int toneOutput      =   9;
const int dimLED          =   8;
const int contrastPin     =  10;

SettingStates  gSettings;

#ifdef LCD_PRINTER
#include <LiquidCrystal.h>
//                rs,  e, 7, 6, 5, 4
LiquidCrystal LCD(12, 11, 5, 4, 1, 0);
#endif

const int gModeBtnPin   = 22;
const int gHourBtnPin   = 53;
const int gMinuteBtnPin = 26;
const int gAlarmBtnPin  = 24;
