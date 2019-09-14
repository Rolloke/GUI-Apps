



//AD9833 Frequenzgenerator 0-12.5 MHz Auflösung 1 Hz
//
// Sinus und Dreieck:
// Frequenz 0 - 12.499.999 Hz (Ri ca. 700 Ohm)
// Amplitude ca. 0,6 VSS hochohmig und ca. 0,042 VSS an 50 Ohm
// Ab ca. 1 MHz nimmt die Amplitude ab.
//
// Rechteck:
// Frequenz 0-6.245.000 Hz (Ri ca. 500 Ohm)
// Amplitude ca. 5 VSS hochohmig und ca. 0,45 VSS an 50 Ohm 
// Die Amplitude ist konstant bis hoch zu 6,2 MHz

#include "ioATMegaNano.h"
//#include "ioAT328.h"
//#include "ioMega2560.h"
#include "LCD_print.h"

#include "AD9833.h"

#include <menu.h>
#include <button.h>



// LCD display
#ifdef LCD_PRINTER
  LiquidCrystal LCD(LCD_RS, LCD_ENABLE, LCD_D1, LCD_D2, LCD_D3, LCD_D4);
#endif


#include <inttypes.h>
#define _USE_FG 1

#define  BTN_LEFT    1
#define  BTN_RIGHT   2
#define  BTN_UP      3
#define  BTN_DOWN    4
#define  BTN_ESCAPE  5
#define  BTN_ENTER   6

#define DEBUG_PRINTLN(P) // Serial.println(P)
#define DEBUG_PRINT(P)   // Serial.print(P)

AD9833 gGenerator(FSYNC, SDATA, SCLK);


long          gFrequency;
int           gWaveForm;

enum eIDs
{
    eFrequency,
    eWaveForms,
    eStore
};

void displayMenuItem();
void displayAll();

// todo! how to control output level
// todo! control offset
// put a level from the pwm output pin through a low pass to an OP-Amp
// todo! rectangular level is about 10 times higher
// pull different resistors to GND with 2 Pins driving an OP-Amp
// todo! persistent storage
// todo! define AT328 pins

// Menu implementation

ItemValue<long>  gSimulate(gFrequency, 1, 12000000);
const char * gBaudrate[] = { "Sine", "Triangle", "Rectangle"};
ItemSelect gBaudrateItem(gBaudrate);

MenuItem gMainMenu[] =
{
    MenuItem("Freq:", &gSimulate, eFrequency, 7),
    MenuItem("Wave:", &gBaudrateItem, eWaveForms, 3,  MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Store:", (ItemEditBase*)0, eStore)
};

MenuItem gMenu("Settings...", gMainMenu, length_of(gMainMenu));

void notifyMenu(uint8_t aDir, uint8_t aID);
// Button implementation
void triggerButton(uint8_t aState, uint8_t aID);
uint8_t gButtonPins[3] = { BTN_BIT2, BTN_BIT1, BTN_BIT0 };
//struct btn { enum eBtn { left, right, up, down, escape, enter, count }; };
Button gButtons(gButtonPins, sizeof(gButtonPins), triggerButton); 

void setup() 
{  
  MenuItem::setNotificationFunction(notifyMenu);
  LCD_BEGIN
  LCD_CLEAR
          // todo load from persistent
  gFrequency = 1000;
  gWaveForm  = 0;
  
  gButtons.setDeBounce(50);
  gButtons.setDelay(1000);
  gButtons.setRepeat(250);

  gGenerator.setup();
  gGenerator.updateFreqency(gFrequency, AD9833::toWaveForm(gWaveForm)); // Sinus 1000 Hz

  Serial.begin(115200);
}

bool gInvalid = true;
void loop() 
{
  unsigned long fNow = millis();
  //uint8_t fB;
  gButtons.tick(fNow);

  if (gInvalid)
  {
    displayAll();
    gGenerator.updateFreqency(gFrequency, AD9833::toWaveForm(gWaveForm));
    gInvalid = false;
  }
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

  DEBUG_PRINT("trigger: ");
  DEBUG_PRINT(Button::nameOf(static_cast<Button::eState>(aState)));
  // the button number is identified by the bit that is set
  // bit 1 for pin  8 -> aButtonNo: 1
  // bit 2 for pin  9 -> aButtonNo: 2
  // bit 3 for pin 10 -> aButtonNo: 4
  DEBUG_PRINT(", Button No: ");
  DEBUG_PRINTLN(aPin);
  
}

void notifyMenu(uint8_t aDir, uint8_t aID)
{
    if (aID == eWaveForms)
    {
        gWaveForm = aDir;
    }
    gInvalid = true;
}


void displayMenuItem()
{
  int fCursor = 0;
  String fString = gMenu.getText(&fCursor);
  LCD_PRINT_LINE(LEFT, 3, fString, fString.length())
  DEBUG_PRINTLN(fString);
  if (fCursor)
  {  
    LCD_SETCURSOR(fCursor, 3);
    LCD_CURSOR
  }
  else
  {
      LCD_NOCURSOR
  }
}

void displayAll()
{
    String fString = "Wave Generator";
    LCD_PRINT_LINE(LEFT, 0, fString, fString.length())
    DEBUG_PRINTLN(fString);
  
    fString = gBaudrate[gWaveForm];
    LCD_PRINT_LINE(LEFT, 1, fString, fString.length())
    DEBUG_PRINTLN(fString);

    fString = String(gFrequency) + " Hz";
    LCD_PRINT_LINE(LEFT, 2, fString, fString.length())
    DEBUG_PRINTLN(fString);

    displayMenuItem();
}
