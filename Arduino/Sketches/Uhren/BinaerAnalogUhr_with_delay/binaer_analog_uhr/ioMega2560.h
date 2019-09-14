const int analogMinute    =  12;                   // the pwm pin for minute pointer instrument
const int analogHour      =  13;                   // the pwm pin for hour pointer instrument
const int toneOutput      =  53;
const int dimLED          =  10;

//                           0,  1,  2,  3,  4,  5,  S,  M,  H
BinaryClockMux gTimeDisplay(36, 38, 40, 42, 44, 46, 48, 50, 52);
//                Button    8    2       9     5
//                       Mode, Hour, Minute, Alarm
SettingStates  gSettings ( A0,   A1,     A2,    A3);

//SoftwareSerial mySerial(10, 11); // RX, TX

#define SOFTWARE_SERIAL

//#define LCD_PRINT 
#ifdef LCD_PRINT
#include <LiquidCrystal.h>
LiquidCrystal LCD(32, 30, 28, 26, 24, 22);
#define gLCD LCD
#endif
