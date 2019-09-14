
#include "button.h"

const int toneOutput      =  53;
const int dimLED          =  10;

//                     data, clock, d1, d2, d3, d4 
ClockMux gTimeDisplay(    1,     1,  1,  1,  1,  1);
//                Button    8    2       9     5
//                       Mode, Hour, Minute, Alarm
SettingStates  gSettings (toneOutput);

const int gModeBtnPin = A1;
const int gHourBtnPin = A2;
const int gMinuteBtnPin = A3;
const int gAlarmBtnPin = A4;


