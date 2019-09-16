

#include <menu.h>

#include <button.h>
#include <LiquidCrystal.h>

// LCD display
  LiquidCrystal LCD(3, 4, 5, 6, 7, 8);


#include <inttypes.h>

#define  BTN_LEFT    1
#define  BTN_RIGHT   2
#define  BTN_UP      3
#define  BTN_DOWN    4
#define  BTN_ESCAPE  5
#define  BTN_ENTER   6

void displayMenuItem();
void displayAll();

long gLongValue = 123;
double gDoubleValue = 123;

void notifyMenu(uint8_t aDir, uint8_t aID);

// Menu implementation
const char * test[] = { "item1", "item2", "item3", "item4", "item5", "item6", 0 };
ItemSelect gEditTest(test);
ItemValue<long>  gEditLong(gLongValue, 100, 200);
ItemValue<double>  gEditDouble(gDoubleValue, -10, 1200);

enum eIDs
{
    eItem,
    eLongValue,
    eDoubleValue,
    eCommand
};


MenuItem gMainMenu[] =
{
    MenuItem("Item:", &gEditTest, eItem, 6, MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Long:", &gEditLong, eLongValue, 3),
    MenuItem("Double:", &gEditDouble, eLongValue, 3, MenuItem::float_val|MenuItem::limit_turn),
    MenuItem("Command", (ItemEditBase*)0, eCommand)
};

MenuItem gMenu("Settings...", gMainMenu, length_of(gMainMenu));

// Button implementation
void triggerButton(uint8_t aState, uint8_t aID);
uint8_t gButtonPins[3] = { 12, 11, 10 };
//struct btn { enum eBtn { left, right, up, down, escape, enter, count }; };
Button gButtons(gButtonPins, sizeof(gButtonPins), triggerButton);

void setup()
{
//  MenuItem::setNotificationFunction(notifyMenu);
  LCD.begin(16, 4);
  LCD.clear();

  gButtons.setDelay(1000);
  gButtons.setRepeat(250);

  Serial.begin(115200);
}

bool gInvalid = true;
void loop()
{
  unsigned long fNow = millis();
  gButtons.tick(fNow);

  if (gInvalid)
  {
    displayAll();
    gInvalid = false;
  }
}


void notifyMenu(uint8_t aDir, uint8_t aID)
{
    Serial.print("notifyMenu(");
    Serial.print((int)aDir);
    Serial.print((int)aID);
    Serial.println(")");
}

MenuItem::dir::to toMenuBtn(uint8_t aPin)
{
  switch(aPin)
  {
    case BTN_LEFT:   return MenuItem::dir::left;
    case BTN_RIGHT:  return MenuItem::dir::right;
    case BTN_UP:     return MenuItem::dir::up;
    case BTN_DOWN:   return MenuItem::dir::down;
    case BTN_ESCAPE: return MenuItem::dir::escape;
    case BTN_ENTER:  return MenuItem::dir::enter;
    default:         return MenuItem::dir::unknown;
  }
}

void triggerButton(uint8_t aState, uint8_t aPin)
{
  switch (aState)
  {
    case Button::pressed:
    case Button::delayed:
    case Button::repeated:
    gMenu.pressBtn(toMenuBtn(aPin));
    displayMenuItem();
    break;
  }
}


void displayMenuItem()
{
  int fCursor = 0;
  String fString = gMenu.getText(&fCursor);
  LCD.setCursor(0, 3);
  LCD.print("                ");
  LCD.setCursor(0, 3);
  LCD.print(fString);
  if (fCursor)
  {
      LCD.setCursor(fCursor, 3);
      LCD.cursor();
  }
  else
  {
      LCD.noCursor();
  }
}

void displayAll()
{
    String fString = "Wave Generator";
    LCD.setCursor(0, 0);
    LCD.print(fString);

//    fString = MenuFunctionWaveForm(MenuItem::dir::text, 0);
//    LCD_PRINT_LINE(LEFT, 1, fString, fString.length());

//    fString = MenuFunctionFrequency(MenuItem::dir::text, 0) + " Hz";
//    LCD_PRINT_LINE(LEFT, 2, fString, fString.length());

    displayMenuItem();
}


