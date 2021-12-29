#ifndef EMULATED
#include <avr/sleep.h>
#endif
#include "PinController.h"
#include "button.h"


uint8_t pin_numbers[] =
{
    3, // 1
   A5, // 2
   10, // 3
    9, // 4
    6, // 5
    7, // 6
    5, // 7
    4, // 8
    0
};

uint16_t dwell_time  = 100;
Command circle1[] =
{
    { PINS08(1,0,0,0, 0,0,0,0), dwell_time },
    { PINS08(0,1,0,0, 0,0,0,0), dwell_time },
    { PINS08(0,0,1,0, 0,0,0,0), dwell_time },
    { PINS08(0,0,0,1, 0,0,0,0), dwell_time },
    { PINS08(0,0,0,0, 1,0,0,0), dwell_time },
    { PINS08(0,0,0,0, 0,1,0,0), dwell_time },
    { PINS08(0,0,0,0, 0,0,1,0), dwell_time },
    { PINS08(0,0,0,0, 0,0,0,1), dwell_time },
};

Command circle2[] =
{
    { PINS08(1,1,0,0, 0,0,0,0), dwell_time },
    { PINS08(0,1,1,0, 0,0,0,0), dwell_time },
    { PINS08(0,0,1,1, 0,0,0,0), dwell_time },
    { PINS08(0,0,0,1, 1,0,0,0), dwell_time },
    { PINS08(0,0,0,0, 1,1,0,0), dwell_time },
    { PINS08(0,0,0,0, 0,1,1,0), dwell_time },
    { PINS08(0,0,0,0, 0,0,1,1), dwell_time },
    { PINS08(1,0,0,0, 0,0,0,1), dwell_time },
};

Command circle3[] =
{
    { PINS08(1,0,0,0, 1,0,0,0), dwell_time },
    { PINS08(0,1,0,0, 0,1,0,0), dwell_time },
    { PINS08(0,0,1,0, 0,0,1,0), dwell_time },
    { PINS08(0,0,0,1, 0,0,0,1), dwell_time },
};

Command all_zero[] =
{
    { PINS08(0,0,0,0, 0,0,0,0), dwell_time }
};

DigitalPinController gPinControl(pin_numbers, circle1, lengthof(circle1));

// Analog pin control
uint8_t analog_pins[] =
{
   pin_numbers[0], // 1
   pin_numbers[2], // 3
   pin_numbers[4], // 5
   pin_numbers[6], // 7
   0
};

Function trapez[] =
{
    { Function::set_start_value,   0,    0 },
    { Function::linear_ramp    , 255, 1000 },
    { Function::linear_ramp    ,   0, 1000 },
    { Function::none           ,   0,    0 }
};

Function triangle[] =
{
    { Function::set_start_value,   0,    0 },
    { Function::linear_ramp    , 255, 2000 },
    { Function::constant       , 255,  200 },
    { Function::linear_ramp    ,   0,   20 },
    { Function::none,              0,    0 }
};

Function sine_half[] =
{
    { Function::set_start_value,   0,    0 },
    { Function::sine_half      , 255, 2000 },
    { Function::none           , 0  ,    0 }
};

AnalogPinController gAnalogPin(analog_pins, triangle);

// control functions with button
uint8_t button_pin = A2;
void button_pressed(uint8_t aState, uint8_t aPin);
Button gButton(button_pin, button_pressed);

struct state {
enum e { circle1, rev_circle1, circle2, rev_circle2, circle3, rev_circle3, sine_half, trapez, triangle, size };
};

state::e gState = state::circle1;


void setup() 
{
    gAnalogPin.stop();
    gPinControl.start();
    gButton.setDelay(3000);
}

void loop() 
{
  unsigned long fNow = millis();
  gAnalogPin.tick(fNow);
  gPinControl.tick(fNow);
  gButton.tick(fNow);
}

void button_pressed(uint8_t aState, uint8_t )
{
    if (aState == Button::released)
    {
        gState = static_cast<state::e>(gState + 1);
        if (gState == state::size) gState = state::circle1;

        bool is_pin_control = true;
        switch (gState)
        {
        case state::circle1:
            gPinControl.setCommands(circle1, lengthof(circle1));
            gPinControl.setFlags(DigitalPinController::reverse, false);
            break;
        case state::rev_circle1:
            gPinControl.setCommands(circle1, lengthof(circle1));
            gPinControl.setFlags(DigitalPinController::reverse, true);
            break;
        case state::circle2:
            gPinControl.setCommands(circle2, lengthof(circle2));
            gPinControl.setFlags(DigitalPinController::reverse, false);
            break;
        case state::rev_circle2:
            gPinControl.setCommands(circle2, lengthof(circle2));
            gPinControl.setFlags(DigitalPinController::reverse, true);
            break;
        case state::circle3:
            gPinControl.setCommands(circle3, lengthof(circle3));
            gPinControl.setFlags(DigitalPinController::reverse, false);
            break;
        case state::rev_circle3:
            gPinControl.setCommands(circle3, lengthof(circle3));
            gPinControl.setFlags(DigitalPinController::reverse, true);
            break;
        case state::sine_half:
            is_pin_control = false;
            gAnalogPin.setFunctions(sine_half);
            break;
        case state::trapez:
            is_pin_control = false;
            gAnalogPin.setFunctions(trapez);
            break;
        case state::triangle:
            is_pin_control = false;
            gAnalogPin.setFunctions(triangle);
            break;
        default: break;
        }
        if (is_pin_control)
        {
            gAnalogPin.stop();
            gPinControl.start();
        }
        else
        {
            gPinControl.start();
            gPinControl.setCommands(all_zero, lengthof(all_zero));
            gPinControl.setFlags(DigitalPinController::reverse, false);
            gAnalogPin.start();
            gPinControl.tick(millis()+1);
            gPinControl.stop();
        }
    }
    else if (aState == Button::delayed)
    {
#ifndef EMULATED
        enterSleepMode();
#endif
    }
}

#ifndef EMULATED
void isrAwake(void)
{
    detachInterrupt(0);
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
    sleep_disable();
}

void enterSleepMode(void)
{
    attachInterrupt(digitalPinToInterrupt(button_pin), isrAwake, LOW);

    gPinControl.start();
    gPinControl.setCommands(all_zero, lengthof(all_zero));
    gPinControl.setFlags(DigitalPinController::reverse, false);
    gPinControl.tick(millis()+1);
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    sleep_disable();
}
#endif
