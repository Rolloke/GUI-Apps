#include <texttools.h>

#include <EEPROM.h>
#include <Time.h>
#include <menu.h>
#include <button.h>
#include <LiquidCrystal.h>


LiquidCrystal LCD(6, 7, 8, 9, 10, 11);

#define  BTN_LEFT    1
#define  BTN_RIGHT   2
#define  BTN_UP      3
#define  BTN_DOWN    4
#define  BTN_ESCAPE  5
#define  BTN_ENTER   6

#define BTN_BIT0 10
#define BTN_BIT1 11
#define BTN_BIT2 12

struct SimulatorItem
{
  uint16_t mInterval;
  float    mMin;
  float    mMax;
  float    mValue;
  float    mAccuracy;
};

struct PersistentData
{
  long mBaudrate;
  uint16_t mInterval;     // ms;
  uint16_t mProtocoll;    // Off, PASHR, TSS1, SSV
  uint16_t mSimulate;     // simulation on, off
  SimulatorItem mHeave;
  SimulatorItem mRoll;
  SimulatorItem mPitch;
  SimulatorItem mHeading;
  SimulatorItem mSSV;
};


enum eKeys
{
  Baudrate = 0,
  Protocol,
  Settings,
  Value,
  Simulate,
  Interval
};


void   check_sentence(String& fString);
const char* read_from_serial();
void   printPASHR();
void   printTSS1();
void   printSSV();
void   readValues();
void   modifyValues(unsigned long aNow);
void   storeConfig();

void displayMenuItem();
void displayAll();

PersistentData gData;
char  gEEPROMid[]   = "Octans";
int   g_ms_per_byte = 2;
bool  gInvalid = true;
// Menu implementation

ItemValue<uint16_t>  gSimulate(gData.mSimulate, 0, 1);

ItemValue<uint16_t>  gInterval(gData.mInterval, 5, 10000);

const char * gBaudrate[] = { "2400", "4800", "9600", "14400", "19200", "38400", "57600", "115200"};
ItemSelect gBaudrateItem(gBaudrate);

enum eProtocol
{
    Off,
    Pashr,
    Tss1,
    SSV,
};
const char * gProtocol[] = { "Off", "PASHR", "TSS1", "SSV"};
ItemSelect gProtocolItem(gProtocol);

enum eSettings
{
    Store,
    Clear
};
const char * gSettings[] = { "Store", "Clear"};
ItemSelect gSettingsItem(gSettings);

const char * sValue    = "Value:";
const char * sMin      = "Min:";
const char * sMax      = "Max:";
const char * sAccuracy = "Accuracy:";
const char * sInterval = "Interval:";
namespace ItemMenuHeading
{
    ItemValue<float>    gValue(   gData.mHeading.mValue   ,   0, 360);
    ItemValue<float>    gMin(     gData.mHeading.mMin     , -45, 0);
    ItemValue<float>    gMax(     gData.mHeading.mMax     ,   0, 45);
    ItemValue<float>    gAccuracy(gData.mHeading.mAccuracy,   0, 10);
    ItemValue<uint16_t> gInterval(gData.mHeading.mInterval,   0, 65535);

    MenuItem gSimulator[] =
    {
        MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sInterval, &gInterval, Value, 5)
    };
}

namespace ItemMenuHeave
{
    ItemValue<float>    gValue(   gData.mHeave.mValue   , -30, 30);
    ItemValue<float>    gMin(     gData.mHeave.mMin     , -10, 0);
    ItemValue<float>    gMax(     gData.mHeave.mMax     ,   0, 10);
    ItemValue<float>    gAccuracy(gData.mHeave.mAccuracy,   0, 10);
    ItemValue<uint16_t> gInterval(gData.mHeave.mInterval,   0, 65535);

    MenuItem gSimulator[] =
    {
        MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sInterval, &gInterval, Value, 5)
    };
}

namespace ItemMenuRoll
{
    ItemValue<float>    gValue(   gData.mRoll.mValue   , -90, 90);
    ItemValue<float>    gMin(     gData.mRoll.mMin     , -45, 0);
    ItemValue<float>    gMax(     gData.mRoll.mMax     ,   0, 45);
    ItemValue<float>    gAccuracy(gData.mRoll.mAccuracy,   0, 10);
    ItemValue<uint16_t> gInterval(gData.mRoll.mInterval,   0, 65535);

    MenuItem gSimulator[] =
    {
        MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sInterval, &gInterval, Value, 5)
    };
}

namespace ItemMenuPitch
{
    ItemValue<float>    gValue(   gData.mPitch.mValue   , -90, 90);
    ItemValue<float>    gMin(     gData.mPitch.mMin     , -45, 0);
    ItemValue<float>    gMax(     gData.mPitch.mMax     ,   0, 45);
    ItemValue<float>    gAccuracy(gData.mPitch.mAccuracy,   0, 10);
    ItemValue<uint16_t> gInterval(gData.mPitch.mInterval,   0, 65535);

    MenuItem gSimulator[] =
    {
        MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sInterval, &gInterval, Value, 5)
    };
}

namespace ItemMenuSSV
{
    ItemValue<float>    gValue(   gData.mSSV.mValue   , 1200, 1800);
    ItemValue<float>    gMin(     gData.mSSV.mMin     , -200, 0);
    ItemValue<float>    gMax(     gData.mSSV.mMax     , 0, 200);
    ItemValue<float>    gAccuracy(gData.mSSV.mAccuracy, 0, 10);
    ItemValue<uint16_t> gInterval(gData.mSSV.mInterval, 0, 65535);

    MenuItem gSimulator[] =
    {
        MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_val|MenuItem::limit_turn),
        MenuItem(sInterval, &gInterval, Value, 5)
    };
}


MenuItem gMainMenu[] =
{
    MenuItem("Simulate:" , &gSimulate, Simulate, 1, MenuItem::limit_turn),
    MenuItem(sInterval   , &gInterval, Interval, 5),
    MenuItem("Baudrate:" , &gBaudrateItem, Baudrate, length_of(gBaudrate),  MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Protocol:" , &gProtocolItem, Protocol, length_of(gProtocol),  MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Settings:" , &gSettingsItem, Settings, length_of(gSettings)),
    MenuItem("Heave..."  , ItemMenuHeave::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Roll..."   , ItemMenuRoll::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Pitch..."  , ItemMenuPitch::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Heading...", ItemMenuHeading::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    MenuItem("SSV..."    , ItemMenuSSV::gSimulator, length_of(ItemMenuHeading::gSimulator))
};

MenuItem gMenu("Settings...", gMainMenu, length_of(gMainMenu));

void notifyMenu(uint8_t aDir, uint8_t aID);
// Button implementation
void triggerButton(uint8_t aState, uint8_t aID);
uint8_t gButtonPins[3] = { BTN_BIT0, BTN_BIT1, BTN_BIT2 };
//struct btn { enum eBtn { left, right, up, down, escape, enter, count }; };
Button gButtons(gButtonPins, sizeof(gButtonPins), triggerButton);


void notifyMenu(uint8_t aDir, uint8_t aID)
{
    switch (aID)
    {
    case Baudrate:
    {
        int fNewBaudrate = atoi(gBaudrate[aDir]);
        if (fNewBaudrate != gData.mBaudrate)
        {
            Serial.begin(fNewBaudrate);
            gData.mBaudrate = fNewBaudrate;
        }
    } break;
    case Protocol: gData.mProtocoll = aDir; break;
    case Settings:
        if (aDir == Store)
        {
            storeConfig();
        }
        else if (aDir == Clear)
        {
            EEPROM.write(0, 0);
        }
    default:
        break;
    }
}

void setup() 
{
  MenuItem::setNotificationFunction(notifyMenu);
  int i, len = strlen(gEEPROMid);
  for (i=0; i<len; ++i)
  {
    char fByte = EEPROM.read(i);
    if (fByte != gEEPROMid[i]) break;
  }
  if (i==len)
  {
    len = sizeof(PersistentData);
    char *fBlock = (char*) &gData;
    for (int j=0; j<len; ++j, ++i)
    {
      fBlock[j] = EEPROM.read(i);
    }
  }
  else // init default
  {
    gData.mBaudrate  = 38400; // Bd
    gData.mInterval  = 1000;   // ms
    gData.mProtocoll = Off;
    gData.mSimulate  = 1;
    gData.mHeave   = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mRoll    = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mPitch   = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mHeading = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mSSV     = { 10000, 1450, 1550, 1500, 0.1 };
  }

  gProtocolItem.select(gData.mProtocoll);
  String f(gData.mBaudrate);
  gBaudrateItem.select(f.c_str());

  gButtons.setDeBounce(50);
  gButtons.setDelay(1000);
  gButtons.setRepeat(250);

  LCD.begin(20, 4);

  Serial.begin(gData.mBaudrate);
}

unsigned long gNextOutput = 0;

void loop() 
{
  unsigned long fNow = millis();
  gButtons.tick(fNow);

  if (fNow >= gNextOutput)
  {
    gNextOutput = fNow + gData.mInterval;
    if (!gData.mSimulate)
    {
      readValues();      
    }
    switch (gData.mProtocoll)
    {
      case Pashr: printPASHR(); break;
      case Tss1:  printTSS1(); break;
      case SSV:   printSSV(); break;
    }
    if (gData.mSimulate)
    {
      modifyValues(fNow);
    }
    // measure times
  }
  if (gInvalid)
  {
      displayMenuItem();
      gInvalid = false;
  }
}

void  printPASHR()
{
  String sPASHR;
  // sPASHR = "$PASHR,200345.00,78.00,T,-3.00,+2.00,+0.00,1.000,1.000,1.000,1,1*32";
  time_t fNow = now();
  tmElements_t fTime;
  breakTime(fNow, fTime);
      
  sPASHR = "$PASHR,";
  if (fTime.Hour < 10) sPASHR.concat('0');
  sPASHR.concat(static_cast<int>(fTime.Hour));
  if (fTime.Minute < 10) sPASHR.concat('0');
  sPASHR.concat(static_cast<int>(fTime.Minute));
  if (fTime.Second < 10) sPASHR.concat('0');
  sPASHR.concat(static_cast<int>(fTime.Second));
  sPASHR.concat(','); // Heading HHH.HH
  sPASHR.concat(printFloat(gData.mHeading.mValue, 0, 2, false));
  sPASHR.concat(",T,"); // True Heading, Roll RRR.RR
  sPASHR.concat(printFloat(gData.mRoll.mValue, 0, 2));
  sPASHR.concat(','); // Pitch PPP.PP
  sPASHR.concat(printFloat(gData.mPitch.mValue, 0, 2));
  sPASHR.concat(','); // Pitch PPP.PP
  sPASHR.concat(printFloat(gData.mHeave.mValue, 0, 2));
  sPASHR.concat(',');  // Roll Accuracy
  sPASHR.concat(printFloat(gData.mRoll.mAccuracy, 0, 3));
  sPASHR.concat(',');  // Pitch Accuracy
  sPASHR.concat(printFloat(gData.mPitch.mAccuracy, 0, 3));
  sPASHR.concat(',');  // Heading Accuracy
  sPASHR.concat(printFloat(gData.mHeading.mAccuracy, 0, 3));
  sPASHR.concat(",1"); // No Position
  sPASHR.concat(",1,*"); // INS Status Flag
  uint8_t fChkSum = 0;
  for (uint16_t i=0; i<sPASHR.length(); ++i)
  {
    fChkSum = fChkSum ^ sPASHR[i];
  }
  sPASHR.concat(String(fChkSum, HEX)); // INS Status Flag
  Serial.println(sPASHR);
}

void  printTSS1()
{
//String sTSS1 = ":013D11 0000U-00510013"
  String sTSS1 = ":000000";
  sTSS1.concat(gData.mHeave.mValue < 0 ? "-" : " ");
  sTSS1.concat(printFloat(gData.mHeave.mValue*100, 4, 0, false));
  sTSS1.concat("U");
  sTSS1.concat(gData.mRoll.mValue < 0 ? "-" : " ");
  sTSS1.concat(printFloat(gData.mRoll.mValue*100, 4, 0, false));
  sTSS1.concat(gData.mPitch.mValue < 0 ? "-" : " ");
  sTSS1.concat(printFloat(gData.mPitch.mValue*100, 4, 0, false));
  Serial.println(sTSS1);
}

void  printSSV()
{
  Serial.println(printFloat(gData.mSSV.mValue, 1, 2, false));
}

void   storeConfig()
{
  int i, len = strlen(gEEPROMid);
  for (i=0; i<len; ++i)
  {
    EEPROM.write(i, gEEPROMid[i]);
  }
  if (i==len)
  {
    len = sizeof(PersistentData);
    char *fBlock = (char*) &gData;
    for (int j=0; j<len; ++j, ++i)
    {
       EEPROM.write(i, fBlock[j]);
    }
  }
}

void  readValues()
{
  // http://playground.arduino.cc/Main/MPU-6050
}

void calculateValue(SimulatorItem* aItem, unsigned long aNow)
{
  if (aItem->mInterval && (aItem->mMax - aItem->mMin) != 0)
  {
    uint16_t fArg =  aNow % aItem->mInterval;
    aItem->mValue = (aItem->mMax + aItem->mMin) * 0.5 +  (aItem->mMax - aItem->mMin) * 0.5 * sin(6.28*(float)fArg/(float)aItem->mInterval);
  }
}

void modifyValues(unsigned long aNow)
{
  switch (gData.mProtocoll)
  {
    case Pashr: 
      calculateValue(&gData.mHeave  , aNow);
      calculateValue(&gData.mRoll   , aNow);
      calculateValue(&gData.mPitch  , aNow);
      calculateValue(&gData.mHeading, aNow);
    break;
    case Tss1: 
      calculateValue(&gData.mHeave, aNow);
      calculateValue(&gData.mRoll , aNow);
      calculateValue(&gData.mPitch, aNow);
    break;
    case SSV:
      calculateValue(&gData.mSSV, aNow);
    break;
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
}

void displayMenuItem()
{
    int fCursor = 0;
    // TODO: set strings and replace, when changed
    String fString = gMenu.getText(&fCursor);
    LCD.setCursor(0, 1);
    LCD.print("                    ");
    LCD.setCursor(0, 1);
    LCD.print(fString);

    fString = gMenu.getPath();
    LCD.setCursor(0, 0);
    LCD.print("                    ");
    LCD.setCursor(0, 0);
    LCD.print(fString);

    if (fCursor)
    {
      LCD.setCursor(fCursor, 1);
      LCD.cursor();
    }
    else
    {
        LCD.noCursor();
    }
}

void displayAll()
{

}
