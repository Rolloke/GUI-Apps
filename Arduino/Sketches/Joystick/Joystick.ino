#include <Mouse.h>
#include <Keyboard.h>
#include <Joystick.h>
#include <button.h>


void triggerButton(uint8_t, uint8_t);

/// @brief Pin Configuration for Arduino Micro
const int PinRight = 2;
const int PinLeft  = 3;
const int PinUp    = 4;
const int PinDown  = 5;
const int PinBtn1  = 6;
const int PinBtn2  = 7;

const int ConfigPin1 = A1;
const int ConfigPin2 = A2;
const int ConfigPin3 = A3;

Button ButtonRight(PinRight, triggerButton);
Button ButtonLeft (PinLeft,  triggerButton);
Button ButtonUp   (PinUp,    triggerButton);
Button ButtonDown (PinDown,  triggerButton);
Button ButtonBtn1 (PinBtn1,  triggerButton);
Button ButtonBtn2 (PinBtn2,  triggerButton);

int8_t XAxis = 0;
int8_t YAxis = 0;
bool   isJoystick  = false;
bool   isKeyboard  = false;
bool   isMouse     = false;
bool   doRepeatBtn = false;

const int mouse_speed = 5;

void setup() 
{
  int debounce_time   = 20;
  int delay_time      = 10;
  int repetition_time = 10;

  pinMode(ConfigPin1, INPUT_PULLUP);
  pinMode(ConfigPin2, INPUT_PULLUP);
  pinMode(ConfigPin3, INPUT_PULLUP);

  int fMode = 0;
  if (digitalRead(ConfigPin1) == LOW) fMode |= 1;
  if (digitalRead(ConfigPin2) == LOW) fMode |= 2;

  switch (fMode)
  {
  case 0: isJoystick = true;  break;
  case 1:
    isKeyboard = true;
    delay_time = 0;
    repetition_time = 0;
  break;
  case 2: 
    isMouse    = true;  
    delay_time = 100;
    break;
  case 3:
    isJoystick = true;
    delay_time  = 5;
    repetition_time = 5;
    break;
  }

  ButtonRight.setDeBounce(debounce_time);
  ButtonLeft.setDeBounce(debounce_time);
  ButtonUp.setDeBounce(debounce_time);
  ButtonDown.setDeBounce(debounce_time);

  ButtonBtn1.setDeBounce(debounce_time);
  ButtonBtn2.setDeBounce(debounce_time);

  if (digitalRead(ConfigPin3) == LOW)
  {
    ButtonBtn1.setDelay(200);
    ButtonBtn1.setRepeat(100);
    ButtonBtn2.setDelay(200);
    ButtonBtn2.setRepeat(100);
  }
  
  if (isJoystick || isMouse)
  {
    ButtonRight.setDelay(delay_time);
    ButtonRight.setRepeat(repetition_time);
    ButtonLeft.setDelay(delay_time);
    ButtonLeft.setRepeat(repetition_time);
    ButtonUp.setDelay(delay_time);
    ButtonUp.setRepeat(repetition_time);
    ButtonDown.setDelay(delay_time);
    ButtonDown.setRepeat(repetition_time);
  }
  if (isJoystick)
  {
    Joystick.begin(false);
    Joystick.setXAxis(XAxis);
    Joystick.setYAxis(YAxis);
    Joystick.setZAxis(0);
    Joystick.sendState();
  }
  if (isKeyboard)
  {
    Keyboard.begin();
  }
  if (isMouse)
  {
    Mouse.begin();
  }
}


void loop()
{
  unsigned long fNow = millis();
  ButtonRight.tick(fNow);
  ButtonLeft. tick(fNow);
  ButtonUp.   tick(fNow);
  ButtonDown. tick(fNow);
  ButtonBtn1. tick(fNow);
  ButtonBtn2. tick(fNow);
}

void triggerButton(uint8_t aState, uint8_t aPin) 
{
  bool button_pressed = aState == Button::pressed;
  int mouseX = 0;
  int mouseY = 0;  
  switch (aState)
  {
    case Button::pressed:
    case Button::delayed:
    case Button::repeated:
      if (isJoystick)
      {
          if (aPin == PinRight && XAxis <  127) ++XAxis;
          if (aPin == PinLeft  && XAxis > -127) --XAxis;
          if (aPin == PinUp    && YAxis <  127) ++YAxis;
          if (aPin == PinDown  && YAxis > -127) --YAxis;
         
          if (aPin == PinBtn1)
          {
            if (aState != Button::pressed) Joystick.setButton(0, 0);
            Joystick.setButton(0, 1);
          }
          if (aPin == PinBtn2)
          {
            if (aState != Button::pressed) Joystick.setButton(1, 0);
            Joystick.setButton(1, 1);
          }
      }
      else if (isKeyboard)
      {
          if (aPin == PinRight) Keyboard.press(KEY_RIGHT_ARROW);
          if (aPin == PinLeft)  Keyboard.press(KEY_LEFT_ARROW);
          if (aPin == PinUp)    Keyboard.press(KEY_UP_ARROW);
          if (aPin == PinDown)  Keyboard.press(KEY_DOWN_ARROW);
          if (aPin == PinBtn1)
          {
            if (!button_pressed) Keyboard.release(KEY_PAGE_UP);
            Keyboard.press(KEY_PAGE_UP);
          }
          if (aPin == PinBtn2)
          {
            if (!button_pressed) Keyboard.release(KEY_PAGE_DOWN);
            Keyboard.press(KEY_PAGE_DOWN);
          }
      }
      else if (isMouse)
      {
          int depending_speed = button_pressed ? 1 : mouse_speed;
          if (aPin == PinRight) mouseX =  depending_speed;
          if (aPin == PinLeft ) mouseX = -depending_speed;
          if (aPin == PinUp   ) mouseY = -depending_speed;
          if (aPin == PinDown ) mouseY =  depending_speed;
          if (aPin == PinBtn1)
          {
            if (!button_pressed) Mouse.release(MOUSE_LEFT);
            Mouse.press(MOUSE_LEFT);
          }
          if (aPin == PinBtn2)
          {
            if (!button_pressed) Mouse.release(MOUSE_RIGHT);
            Mouse.press(MOUSE_RIGHT);
          }
      }
      break;
      case Button::released:
      if (isJoystick)
      {
          if (aPin == PinRight) XAxis = 0;
          if (aPin == PinLeft ) XAxis = 0;
          if (aPin == PinUp   ) YAxis = 0;
          if (aPin == PinDown ) YAxis = 0;
          if (aPin == PinBtn1)  Joystick.setButton(0, 0);
          if (aPin == PinBtn2)  Joystick.setButton(1, 0);
      }
      else if (isKeyboard)
      {
          if (aPin == PinRight) Keyboard.release(KEY_RIGHT_ARROW);
          if (aPin == PinLeft)  Keyboard.release(KEY_LEFT_ARROW);
          if (aPin == PinUp)    Keyboard.release(KEY_UP_ARROW);
          if (aPin == PinDown)  Keyboard.release(KEY_DOWN_ARROW);
          if (aPin == PinBtn1)  Keyboard.release(KEY_PAGE_UP);
          if (aPin == PinBtn2)  Keyboard.release(KEY_PAGE_DOWN);
      }
      else if (isMouse)
      {
          if (aPin == PinBtn1)  Mouse.release(KEY_PAGE_UP);
          if (aPin == PinBtn2)  Mouse.release(KEY_PAGE_DOWN);
      }
      break;
  }

  if (isJoystick)
  {
      Joystick.setXAxis(XAxis);
      Joystick.setYAxis(YAxis);
      Joystick.sendState();
  }
  if (isMouse && (mouseX || mouseY))
  {
    Mouse.move(mouseX, mouseY);
  }
}
