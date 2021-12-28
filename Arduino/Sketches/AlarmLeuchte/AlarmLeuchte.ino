
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
    { 0, 0 },
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
    { 0, 0 },
};

DigitalPinController gPinControl(pin_numbers, circle1);

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
    { Function::linear_ramp, 255, 1000 },
    { Function::linear_ramp,   0, 1000 },
    { Function::none,          0,    0 }
};

Function triangle[] =
{
    { Function::linear_ramp, 255, 2000 },
    { Function::linear_ramp,   0, 2000 },
    { Function::none,          0,    0 }
};

Function sine_half[] =
{
    { Function::sine_half, 255, 3000 },
    { Function::none, 0, 0}
};

AnalogPinController gAnalogPin(analog_pins, triangle);

void button_pressed(uint8_t aState, uint8_t aPin);
Button gButton(A2, button_pressed);
struct state {
enum e { stop, circle1, circle2, size };
};

state::e gState = state::circle1;

void setup() 
{
    //gAnalogPin.stop();
    gPinControl.start();
}

void loop() 
{
  // the button uses ticks in ms for working
  unsigned long fNow = millis();
  //gAnalogPin.tick(fNow);
  gPinControl.tick(fNow);
  gButton.tick(fNow);
}

void button_pressed(uint8_t aState, uint8_t )
{
    if (aState == Button::released)
    {
        gState = static_cast<state::e>(gState + 1);
        switch (gState) {
        case state::circle1:
            gPinControl.setCommands(circle1);
            gPinControl.start();
            break;
        case state::circle2:
            gPinControl.setCommands(circle2);
            gPinControl.start();
            break;
        case state::size: gState = state::stop;
            gPinControl.stop(); break;
        default: break;
        }
    }
}
