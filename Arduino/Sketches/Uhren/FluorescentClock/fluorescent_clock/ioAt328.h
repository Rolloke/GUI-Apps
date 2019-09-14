// C:\Program Files\Arduino\hardware\arduino\avr\variants\standard\pins_arduino.h
#include <Arduino.h>
#include "button.h"


#if 1
 //current settings for 

const int toneOutput      =  2;
//                     data, clock, d1, d2, d3, d4 
ClockMux gTimeDisplay(    8,     7,  6,  5,  4,  3);

SettingStates  gSettings ( toneOutput);

const int gModeBtnPin = 9;
const int gHourBtnPin = 10;
const int gMinuteBtnPin = 11;
const int gAlarmBtnPin = 12;


/*
Digital Pin 0  (PCINT15,RXD)      (RX)  
Digital Pin 1  (PCINT17,TXD)      (TX)  
Digital Pin 2  (PCINT18,INT0)           toneOutput
Digital Pin 3  (PCINT19,OC2B,INT1)(PWM) D0
Digital Pin 4  (PCINT20,XCK,T0)         D1
Digital Pin 5  (PCINT21,OC0B,T1)  (PWM) D2
Digital Pin 6  (PCINT22mOCA0,AIN0)(PWM) D3
Digital Pin 7  (PCINT23,AIN1)           clock
Digital Pin 8  (PCINT0,CLK0,ICP1)       data
Digital Pin 9  (OCA1,PCINT1       (PWM) 
Digital Pin 10 (SS,OC1B,PCINT2)   (PWM) 
Digital Pin 11 (MOSI.OCA2,PCINT3) (PWM) 
Digital Pin 12 (MISO/PCINT4)            
Digital Pin 13 (SCK/PCINT5)             

Analog PIN  0  (PCINT8)                 T0
Analog PIN  1  (PCINT9)                 T1
Analog PIN  2  (PCINT10)                T2
Analog PIN  3  (PCINT11)                T3
Analog PIN  4  (PCINT12,SDA)            Time SDA
Analog PIN  5  (PCINT13,SCL)            Time SCL
*/

#else

const int toneOutput      =  0;
const int dimLED          =  3;
//                           0,  1,  2,  3,  4,  5,  S,   M,   H
ClockMux gTimeDisplay(  13, 12,  11, 10, 9, 8, 7, 6,  5,  4,  3,  1);
//                       Mode, Hour, Minute, Alarm
SettingStates  gSettings ( 12,   13,     A0,    A1, toneOutput);

//#define SOFTWARE_SERIAL
/*
Digital Pin 0  (PCINT15,RXD)      (RX)  toneOutput
Digital Pin 1  (PCINT17,TXD)      (TX)  D0
Digital Pin 2  (PCINT18,INT0)           D1
Digital Pin 3  (PCINT19,OC2B,INT1)(PWM) D2
Digital Pin 4  (PCINT20,XCK,T0)         D3
Digital Pin 5  (PCINT21,OC0B,T1)  (PWM) analogMinute
Digital Pin 6  (PCINT22mOCA0,AIN0)(PWM) analogHour
Digital Pin 7  (PCINT23,AIN1)           D4
Digital Pin 8  (PCINT0,CLK0,ICP1)       D5
Digital Pin 9  (OCA1,PCINT1       (PWM) H
Digital Pin 10 (SS,OC1B,PCINT2)   (PWM) M
Digital Pin 11 (MOSI.OCA2,PCINT3) (PWM) S
Digital Pin 12 (MISO/PCINT4)            T0
Digital Pin 13 (SCK/PCINT5)             T1

Analog PIN  0  (PCINT8)                 T2
Analog PIN  1  (PCINT9)                 T3
Analog PIN  2  (PCINT10)                MeasureBattery
Analog PIN  3  (PCINT11)                
Analog PIN  4  (PCINT12,SDA)            Time SDA
Analog PIN  5  (PCINT13,SCL)            Time SCL
*/

#endif


