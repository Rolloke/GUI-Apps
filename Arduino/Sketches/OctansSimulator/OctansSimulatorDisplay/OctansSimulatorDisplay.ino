#include <texttools.h>

#include <EEPROM.h>
#include <Time.h>
#include <menu.h>
#include <button.h>
#include <LiquidCrystal.h>

//#define EMULATED 1
#ifndef EMULATED
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards

bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

#endif
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal LCD(rs, en, d4, d5, d6, d7);

#define LCD_XDIM 16
#define LCD_YDIM 2

#define  BTN_LEFT    1
#define  BTN_RIGHT   2
#define  BTN_UP      3
#define  BTN_DOWN    4
#define  BTN_ESCAPE  5
#define  BTN_ENTER   6

#define BTN_BIT0 31
#define BTN_BIT1 33
#define BTN_BIT2 35

#define USE_ACCURACY 1
#define USE_SSV 1

struct SimulatorItem
{
    uint16_t mInterval;
    float    mMin;
    float    mMax;
    float    mValue;
#if USE_ACCURACY == 1
    float    mAccuracy;
#endif
};

enum eProtocol
{
    Off,
    Pashr,
    Tss1,
#if USE_SSV == 1
    SSV,
#endif
};

enum eOperationMode : uint16_t
{
    Simulation,
    MeasureYawPitchRoll,
    MeasureEuler
};


struct PersistentData
{
    long mBaudrate;
    uint16_t mInterval;     // ms;
    uint16_t mProtocoll;    // Off, PASHR, TSS1, SSV
    uint16_t mOperationMode;// Simulation, YawPitchRoll, Euler
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

void   readMPU6050Values();
void   modifyValues(unsigned long aNow);
void   calculateValue(SimulatorItem* aItem, unsigned long aNow);
void   storeConfig();

void printLineToLCD(String& fString, int line=0);
void displayMenuItem();
void displayAll();

PersistentData gData;
char  gEEPROMid[]   = "Octans";
int   g_ms_per_byte = 2;
bool  gInvalid = true;
// Menu implementation

const char * gOperationMode[] = { "Simulation", "YPR Angles", "Euler Angles"};
ItemValue<uint16_t>  gOperationModeItem(gData.mOperationMode, Simulation, MeasureEuler);

ItemValue<uint16_t>  gInterval(gData.mInterval, 5, 10000);

const char * gBaudrate[] = { "2400", "4800", "9600", "14400", "19200", "38400", "57600", "115200"};
ItemSelect gBaudrateItem(gBaudrate);

const char * gProtocol[] = { "Off", "PASHR", "TSS1"
                             #if USE_SSV == 1
                             , "SSV"
                             #endif
                           };
ItemSelect gProtocolItem(gProtocol);

enum eSettings
{
    Store,
    Clear
};
const char * gSettings[] = { "Store", "Clear" };
ItemSelect gSettingsItem(gSettings);


const char * sValue    = "Value:";
const char * sMin      = "Min:";
const char * sMax      = "Max:";
const char * sAccuracy = "Accuracy:";
const char * sInterval = "Interval:";
namespace ItemMenuHeading
{
ItemValue<float>    gValue(   gData.mHeading.mValue   ,   0, 360);
ItemValue<float>    gMin(     gData.mHeading.mMin     ,   0, 360);
ItemValue<float>    gMax(     gData.mHeading.mMax     ,   0, 360);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mHeading.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mHeading.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

namespace ItemMenuHeave
{
ItemValue<float>    gValue(   gData.mHeave.mValue   , -30, 30);
ItemValue<float>    gMin(     gData.mHeave.mMin     , -10, 10);
ItemValue<float>    gMax(     gData.mHeave.mMax     , -10, 10);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mHeave.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mHeave.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

namespace ItemMenuRoll
{
ItemValue<float>    gValue(   gData.mRoll.mValue   , -90, 90);
ItemValue<float>    gMin(     gData.mRoll.mMin     , -45, 45);
ItemValue<float>    gMax(     gData.mRoll.mMax     , -45, 45);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mRoll.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mRoll.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

namespace ItemMenuPitch
{
ItemValue<float>    gValue(   gData.mPitch.mValue   , -90, 90);
ItemValue<float>    gMin(     gData.mPitch.mMin     , -45, 45);
ItemValue<float>    gMax(     gData.mPitch.mMax     , -45, 45);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mPitch.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mPitch.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

#if USE_SSV == 1
namespace ItemMenuSSV
{
ItemValue<float>    gValue(   gData.mSSV.mValue   , 1200, 1800);
ItemValue<float>    gMin(     gData.mSSV.mMin     , 1200, 1800);
ItemValue<float>    gMax(     gData.mSSV.mMax     , 1200, 1800);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mSSV.mAccuracy, 0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mSSV.mInterval, 0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 3, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 3, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 3, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}
#endif

MenuItem gMainMenu[] =
{
    MenuItem("Protocol:" , &gProtocolItem, Protocol, length_of(gProtocol), MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Heave..."  , ItemMenuHeave::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Roll..."   , ItemMenuRoll::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Pitch..."  , ItemMenuPitch::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Heading...", ItemMenuHeading::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    #if USE_SSV == 1
    MenuItem("SSV..."    , ItemMenuSSV::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    #endif
    MenuItem("Baudrate:" , &gBaudrateItem, Baudrate, length_of(gBaudrate), MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem(sInterval   , &gInterval, Interval, 5),
    MenuItem("Operation:", &gOperationModeItem, Simulate, 1, MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Settings:" , &gSettingsItem, Settings, length_of(gSettings), MenuItem::no_cursor|MenuItem::limit_turn)
};

MenuItem gMenu(0, gMainMenu, length_of(gMainMenu));

void notifyMenu(uint8_t aDir, uint8_t aID);
// Button implementation
void triggerButton(uint8_t aState, uint8_t aID);
uint8_t gButtonPins[3] = { BTN_BIT0, BTN_BIT1, BTN_BIT2 };
Button gButtons(gButtonPins, sizeof(gButtonPins), triggerButton);

void  readMPU6050Values();

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
        gData.mBaudrate       = 38400; // Bd
        gData.mInterval       = 1000;   // ms
        gData.mProtocoll      = Off;
        gData.mOperationMode  = Simulation;
#if USE_ACCURACY == 1
        gData.mHeave   = { 10000, -5.0, 5.0,   0.0, 0.1 };
        gData.mRoll    = { 10000, -5.0, 5.0,   0.0, 0.1 };
        gData.mPitch   = { 10000, -5.0, 5.0,   0.0, 0.1 };
        gData.mHeading = { 10000, -5.0, 5.0,   0.0, 0.1 };
        gData.mSSV     = { 10000, 1450, 1550, 1500, 0.1 };
#else
        gData.mHeave   = { 10000, -5.0, 5.0,   0.0 };
        gData.mRoll    = { 10000, -5.0, 5.0,   0.0 };
        gData.mPitch   = { 10000, -5.0, 5.0,   0.0 };
        gData.mHeading = { 10000, -5.0, 5.0,   0.0 };
        gData.mSSV     = { 10000, 1450, 1550, 1500 };
#endif
    }

    gProtocolItem.select(gData.mProtocoll);
    String f(gData.mBaudrate);
    gBaudrateItem.select(f.c_str());

    gButtons.setDeBounce(50);
    gButtons.setDelay(1000);
    gButtons.setRepeat(250);

    LCD.begin(LCD_XDIM, LCD_YDIM);

#ifndef EMULATED
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    String fString = "Initializing MPU";
    printLineToLCD(fString);

    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0)
    {
        fString = F("Calibrating MPU...");
        printLineToLCD(fString);

        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        fString = "Enabling DMP...";
        printLineToLCD(fString);
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        fString = "DMP ready!";
        printLineToLCD(fString);
        dmpReady = true;
        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        fString = "DMP Init failed:";
        printLineToLCD(fString);

        fString = String(devStatus);
        printLineToLCD(fString, 1);
    }
#endif

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
        if (gData.mOperationMode != Simulation)
        {
            readMPU6050Values();
        }
        switch (gData.mProtocoll)
        {
        case Pashr: printPASHR(); break;
        case Tss1:  printTSS1(); break;
#if USE_SSV == 1
        case SSV:   printSSV(); break;
#endif
        }
        if (gData.mOperationMode == Simulation)
        {
            modifyValues(fNow);
        }
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
#if USE_ACCURACY ==1
    sPASHR.concat(',');  // Roll Accuracy
    sPASHR.concat(printFloat(gData.mRoll.mAccuracy, 0, 3));
    sPASHR.concat(',');  // Pitch Accuracy
    sPASHR.concat(printFloat(gData.mPitch.mAccuracy, 0, 3));
    sPASHR.concat(',');  // Heading Accuracy
    sPASHR.concat(printFloat(gData.mHeading.mAccuracy, 0, 3));
#else
    sPASHR.concat(",0.1,0.1,0.1");
#endif
    sPASHR.concat(",1"); // No Position
    sPASHR.concat(",1,*"); // INS Status Flag
    uint8_t fChkSum = 0;
    for (uint8_t i=0; i<sPASHR.length(); ++i)
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

#if USE_SSV == 1
void  printSSV()
{
    Serial.println(printFloat(gData.mSSV.mValue, 1, 2, false));
}
#endif

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

void  readMPU6050Values()
{
#ifndef EMULATED
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize)
    {
        if (mpuInterrupt && fifoCount < packetSize) {
            // try to get out of the infinite loop
            fifoCount = mpu.getFIFOCount();
        }
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    if(fifoCount < packetSize)
    {
        //Lets go back and wait for another interrupt. We shouldn't be here, we got an interrupt from another event
        // This is blocking so don't do it   while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    }
    // check for overflow (this should never happen unless our code is too inefficient)
    else if ((mpuIntStatus & (0x01 << MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024)
    {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        //  fifoCount = mpu.getFIFOCount();  // will be zero after reset no need to ask

        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
    else if (mpuIntStatus & (0x01 << MPU6050_INTERRUPT_DMP_INT_BIT))
    {
        // read a packet from FIFO
        while(fifoCount >= packetSize)
        { // Lets catch up to NOW, someone is using the dreaded delay()!
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;
        };

        if (gData.mOperationMode == MeasureEuler)
        {
            // get Euler angles
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetEuler(euler, &q);
            gData.mHeading.mValue = euler[0];
            gData.mPitch.mValue   = euler[1];
            gData.mRoll.mValue    = euler[2];
        }
        if (gData.mOperationMode == MeasureYawPitchRoll)
        {
            // get yaw pitch roll angles
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            gData.mHeading.mValue = ypr[0];
            gData.mPitch.mValue   = ypr[1];
            gData.mRoll.mValue    = ypr[2];
        }
    }
#endif
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
#if USE_SSV == 1
    case SSV:
        calculateValue(&gData.mSSV, aNow);
        break;
#endif
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

String fLine0;
String fLine1;

void displayMenuItem()
{
    int fCursor = 0;
    String fString = gMenu.getText(&fCursor);
    fillStringWithSpaces(fString, LCD_XDIM);
    if (fLine1 != fString)
    {
        LCD.setCursor(0, 1);
        LCD.print(fString);
        fLine1 = fString;
    }

    fString = gMenu.getPath();
    if (fString.length())
    {
        int pos = fString.indexOf("...");
        if (pos != -1)
        {
            fString = fString.substring(0, pos);
        }
    }
    else
    {
        fString  = gOperationMode[gData.mOperationMode];
        fString += ":";
        fString += gProtocol[gData.mProtocoll];
    }
    fillStringWithSpaces(fString, LCD_XDIM);
    if (fLine0 != fString)
    {
        LCD.setCursor(0, 0);
        LCD.print(fString);
        fLine0 = fString;
    }

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

void printLineToLCD(String& fString, int line)
{
    fillStringWithSpaces(fString, LCD_XDIM);
    LCD.setCursor(0, line);
    LCD.print(fString);
}

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
        break;
    default:
        break;
    }
}
