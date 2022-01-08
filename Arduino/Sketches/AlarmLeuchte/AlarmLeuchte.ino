#ifndef EMULATED
#include <avr/sleep.h>
#endif

#include "PinController.h"
#include "button.h"
#include "Melody.h"


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

Command blink1[] =
{
    { PINS08(1,1,1,1, 1,1,1,1), dwell_time },
    { PINS08(0,0,0,0, 0,0,0,0),        500 }
};

Command blink2[] =
{
    { PINS08(1,1,1,1, 1,1,1,1),  50 },
    { PINS08(0,0,0,0, 0,0,0,0), 100 },
    { PINS08(1,1,1,1, 1,1,1,1),  50 },
    { PINS08(0,0,0,0, 0,0,0,0), 500 }
};

Command switch_off[] =
{
    { PINS08(1,1,1,1, 1,1,1,1), 300 },
    { PINS08(1,1,1,0, 0,1,1,1), 300 },
    { PINS08(1,1,0,0, 0,0,1,1), 300 },
    { PINS08(1,0,0,0, 0,0,0,1), 300 },
    { PINS08(0,0,0,0, 0,0,0,0), 300 },
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
    { Function::constant       ,   0, 1000 },
    { Function::none           ,   0,    0 }
};

Function triangle[] =
{
    { Function::set_start_value,   0,    0 },
    { Function::linear_ramp    , 255, 2000 },
    { Function::constant       , 255,  200 },
    { Function::linear_ramp    ,   0,   20 },
    { Function::constant       ,   0, 1000 },
    { Function::none,              0,    0 }
};

Function sine_half[] =
{
    { Function::set_start_value,   0,    0 },
    { Function::sine_half_1    , 255, 2000 },
    { Function::constant       ,   0, 1000 },
    { Function::none           , 0  ,    0 }
};

Function sine[] =
{
    { Function::set_start_value, 127,    0 },
    { Function::sine           , 255, 2000 },
    { Function::none           , 0  ,    0 }
};

AnalogPinController gAnalogPin(analog_pins, triangle);

// Fire horn control
Tone gFireHorn[] =
{
    { NOTE_FS4, 1, 1},
    { SILENCE , 1,16},
    { NOTE_DS5, 1, 1},
    { SILENCE , 1, 8},
    { 0, 0, 0}
};

Melody gSoundControl(8, gFireHorn, 20, 1000);

// switch off timer
bool timer_event();
TimerFunction gTimer(timer_event, 0, 0);
bool gSleeping = false;

// control functions with button
const uint8_t button_pin = 2;
void button_pressed(uint8_t aState, uint8_t aPin);
Button gButton(button_pin, button_pressed);

// several states
struct state {
enum e { circle1, blink1, rev_circle1, circle2, blink2, rev_circle2, circle3, rev_circle3, sine, sine_half, triangle, trapez, size };
};

state::e gState = state::circle1;
void set_state();


// Arduino setup
void setup() 
{
    gAnalogPin.stop();
    gPinControl.start();
    gButton.setDelay(2000);
    gTimer.stop();
    gSoundControl.stopMelody();
}

// Arduino loop
void loop()
{
  const unsigned long fNow = millis();
  gAnalogPin.tick(fNow);
  gPinControl.tick(fNow);
  gButton.tick(fNow);
  gSoundControl.tick(fNow);
  gTimer.tick(fNow);
}

void button_pressed(uint8_t aState, uint8_t )
{
    if (aState == Button::released)
    {
        gState = static_cast<state::e>(gState + 1);
//        if (gState == state::triangle) gState = state::circle1;
        if (gState == state::size) gState = state::circle1;
        set_state();
    }
    else if (aState == Button::delayed)
    {
        gSoundControl.stopMelody();

        gAnalogPin.stop();
        gAnalogPin.set_value_of_all_pins(0);

        gPinControl.setCommands(switch_off, lengthof(switch_off), TimerController::one_shot);
        gPinControl.setFlags(DigitalPinController::reverse, false);
        gPinControl.start();

        gTimer.setTimerFunction(timer_event, 2000, TimerController::one_shot);
        gTimer.start();
    }
}

bool timer_event()
{
#ifndef EMULATED
    enterSleepMode();
#else
    digitalWrite(A4, HIGH);
#endif
    return true;
}

void set_state()
{
    gSoundControl.stopMelody();
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
    case state::blink1:
        gPinControl.setCommands(blink1, lengthof(blink1));
        gPinControl.setFlags(DigitalPinController::reverse, false);
        gSoundControl.startMelody();
        break;
    case state::blink2:
        gPinControl.setCommands(blink2, lengthof(blink2));
        gPinControl.setFlags(DigitalPinController::reverse, false);
        gSoundControl.startMelody();
        break;
    case state::sine:
        is_pin_control = false;
        gAnalogPin.setFunctions(sine);
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
        gAnalogPin.set_value_of_all_pins(0);
        gPinControl.start();
    }
    else // is analog
    {
        gPinControl.stop();
        gPinControl.set_all_pins(LOW);
        gAnalogPin.start();
    }
}


#ifndef EMULATED
void isrAwake(void)
{
  if (gSleeping)
  {
    cli(); // deactivate interrupts
    detachInterrupt(digitalPinToInterrupt(button_pin));
    sei(); // 
    
    sleep_disable();
    gState = state::circle1;
    set_state();
  }
  gSleeping = false;
}

void enterSleepMode(void)
{
    cli(); // deactivate interrupts
    attachInterrupt(digitalPinToInterrupt(button_pin), isrAwake, FALLING);
    sei(); // 
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    gSleeping = true;
}
#endif
