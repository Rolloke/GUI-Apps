
/*
  LCD Clock

 Shows clock time on an LCD Display
 * 4 buttons for settings
 * 1 alarm tone output
 created 2015
 by Rolf Kary-Ehlers
 
 */

#include "SettingStates.h"


#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <button.h>
#include <Melody.h>

#define LCD_PRINTER 1

#include "LCD_print.h"

//#include "ioMega2560.h"  // pins for test with at mega 2560
#include "ioAt328.h"   // pins for at mega 328


#define TICK_INTERVAL_MS 500

#if LanguageSelection == SELECT
 #define SELECT_ENGLISH if (gSettings.getLanguage()== EN)
 #define SELECT_GERMAN  else

#else
 #define SELECT_ENGLISH
 #define SELECT_GERMAN
#endif

String gLine1;
String gLine2;

void PrintLCD_Time();

void triggerModeButton(uint8_t, uint8_t);
void triggerHourMinutButton(uint8_t, uint8_t);
void triggerAlarmButton(uint8_t, uint8_t);
void onTimerAlarm();
void onAlarm();


//#define TEST_PERIPHERY
Button gModeButton(gModeBtnPin, triggerModeButton);
Button gAlarmButton(gAlarmBtnPin, triggerAlarmButton);
uint8_t gHMPins[2] = {static_cast<uint8_t>(gHourBtnPin), static_cast<uint8_t>(gMinuteBtnPin) };
Button gHourMinuteButton(gHMPins, 2, triggerHourMinutButton);
OnTick_t gTimerFunctions[SettingStates::Timers];


Tone gTones1[] =
{
    { NOTE_C7, 1, 8},
    { 0, 2, 1},
    { 0, 0, 0}
};

#if NO_OF_TONES >= 2
Tone gTones2[] =
{
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};
#endif

#if NO_OF_TONES >= 3
Tone gTones3[] =
{
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};
#endif

#if NO_OF_TONES >= 4
Tone gTones4[] =
{
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};
#endif

#if NO_OF_TONES >= 5
Tone gTones5[] =
{
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};
#endif

Tone gTones6[] =
{
    #include "big_ben.h"
};

Tone gTones7[] =
{
    { NOTE_C7, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_A6, 1, 16},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};

Tone gTones8[] =
{
    { NOTE_A6, 1, 16},
    { SILENCE, 1, 16},
    { NOTE_C7, 1, 16},
    { SILENCE, 2, 1},
    { 0, 0, 0}
};

Melody gMelody(toneOutput, gTones6, 20, 1000);

#define countof(X) sizeof(X) / sizeof(X[0])

void setup() 
{
    gModeButton.setDeBounce(10);
    gAlarmButton.setDeBounce(10);
    gHourMinuteButton.setDeBounce(10);
    gModeButton.setDelay(2000);
    gHourMinuteButton.setDelay(1000);
    gHourMinuteButton.setRepeat(250);


    gSettings.setTimerFunction(&onTimerAlarm);
    gSettings.setAlarmFunction(&onAlarm);
    gSettings.setMeasureCurrentPins(vddPulsePin, measureCurrentPin);
    gSettings.setMeasureTemperaturePin(measureTemperturePin);
    gSettings.setMeasureVoltagePin(measureVoltagePin);

    LCD_BEGIN

    tmElements_t fTime;
    if (RTC.read(fTime))
    {
        LCD_PRINT_AT(0, 0, F("got time from RTC"));
        delay(1000);
    }

    if (RTC.chipPresent())
    {
        setSyncProvider(RTC.get);
    }
}


void loop()
{
    unsigned long fNow = millis();

    gModeButton.tick(fNow);
    gHourMinuteButton.tick(fNow);
    gAlarmButton.tick(fNow);
    gSettings.tick(fNow);
    gMelody.tick(fNow);

    if (gSettings.hasDisplayChanged())
    {
        analogWrite(dimLED, gSettings.isLightOn());
        analogWrite(contrastPin, gSettings.getContrast());

        // display
        PrintLCD_Time();
    }
}

void triggerModeButton(uint8_t aState, uint8_t )
{
    gSettings.triggerButton(SettingStates::Mode, aState);
    gSettings.onTrigger();
    gSettings.triggerButton(SettingStates::ClearAll, Button::none);
}

void triggerHourMinutButton(uint8_t aState, uint8_t aPin)
{
    if (aPin & 1) gSettings.triggerButton(SettingStates::Hour, aState);
    if (aPin & 2) gSettings.triggerButton(SettingStates::Minute, aState);
    if (aPin & 3)
    {
        gSettings.onTrigger();
        gSettings.triggerButton(SettingStates::ClearAll, Button::none);
    }
}

void triggerAlarmButton(uint8_t aState, uint8_t )
{
    if (gMelody.isPlaying())
    {
        gMelody.stopMelody();
    }
    else
    {
        gSettings.triggerButton(SettingStates::AlarmBtn, aState);
        gSettings.onTrigger();
        gSettings.triggerButton(SettingStates::ClearAll, Button::none);
    }
}
void fillStringWithSpaces(String& aStr, unsigned int aLen)
{
    while (aStr.length() < aLen)
    {
        aStr += " ";
    }
}

void print2Decimals(int aNumber)
{
    if (aNumber >= 0)
    {
        if (aNumber < 10) LCD_PRINT("0");
        LCD_PRINT(aNumber);
    }
}

void print2Decimals(String& aStr, int aNumber)
{
    if (aNumber >= 0)
    {
        if (aNumber < 10) aStr += "0";
        aStr += String(aNumber);
    }
}

void PrintLCD_Time()
{
    String fLine1;
    fLine1 = gSettings.getStateName();

    if (   (gSettings.isTimerState()                        && gSettings.isTimerActive())
        || (gSettings.getState() == SettingStates::SetAlarm && gSettings.isAlarmActive()))
    {
        fLine1 += ": *";
    }
    switch (gSettings.isCalibrating())
    {
    case SettingStates::CalibrateCurrent:
    case SettingStates::CalibrateVoltage:
    case SettingStates::CalibrateTemperature: fLine1 += F(" <->"); break;
    case SettingStates::SetUpperTemperature:  fLine1 += F(" -->"); break;
    case SettingStates::SetLowerTemperature:  fLine1 += F(" <--"); break;
    case SettingStates::ResetCalibrationValue:fLine1 += F(" =>R"); break;
    case SettingStates::ResetWorkValue:       fLine1 += F(" =>0"); break;
    }

    if (gSettings.getState() == SettingStates::Time)
    {
        LCD_PRINT(": ");
        for (int i=0; i<SettingStates::Timers; ++i)
        {
            if (gSettings.isTimerActive(i))
            {
                fLine1 += "*";
                fLine1 += String(i+1);
            }
            else
            {
                fLine1 += "  ";
            }
        }
    }
    fillStringWithSpaces(fLine1, LCD_XDIM);
    if (fLine1 != gLine1)
    {
        LCD_PRINT_AT(0, 0, fLine1);
        gLine1 = fLine1;
    }

    String fLine2;
    bool fPrintTime = false;

    switch (gSettings.getState())
    {
    case SettingStates::Time:
    case SettingStates::SetAlarm:
        fPrintTime = true;
        break;
    case SettingStates::Date:
#if LanguageSelection == EN || LanguageSelection == SELECT
    SELECT_ENGLISH
        {
            fLine2 += monthShortStr(month());
            fLine2 += ",";
            fLine2 += String(day());
            fLine2 += ",";
            print2Decimals(fLine2, gSettings.getSeconds());
        }
#endif

#if LanguageSelection == DE || LanguageSelection == SELECT
    SELECT_GERMAN
        {
            print2Decimals(fLine2, gSettings.getMinutes());
            fLine2 += ":";
            print2Decimals(fLine2, gSettings.getHours());
            fLine2 += ":";
            print2Decimals(fLine2, gSettings.getSeconds());
        }
#endif
        break;
    case SettingStates::SetTime:
        print2Decimals(fLine2, gSettings.getHours());
        fLine2 += ":";
        print2Decimals(fLine2, gSettings.getMinutes());
        break;
    case SettingStates::SetDate:
        print2Decimals(fLine2, gSettings.getHours());
        fLine2 += ":";
        print2Decimals(fLine2, gSettings.getMinutes());
        break;
    case SettingStates::SetYear:
        print2Decimals(fLine2, gSettings.getSeconds());
        break;
    case SettingStates::SetAlarmMode:
        fLine2 += gSettings.getAlarmModeName();
        break;
    case SettingStates::SetAlarmDay:
        fLine2 += gSettings.getAlarmDayName();
        break;
    case SettingStates::SetMeasurementsActive:
        fLine2 += gSettings.getActiveMeasurements();
        break;
    case SettingStates::MeasureCurrent:
        fLine2 += String(gSettings.getUSBCurrentValue_mA(), (unsigned char)1);
        fLine2 += " mA";
        break;
    case SettingStates::MeasureVoltage:
        fLine2 += String(gSettings.getUSBVoltageValue_V(), (unsigned char)1);
        fLine2 += " V";
        break;
#if MeasureElectricPower == 1
    case SettingStates::MeasurePower:
    {
        fLine2 += String(gSettings.getUSBPowerValue_W(), (unsigned char)1);
        fLine2 += " W, ";
#if MeasureElectricWork ==1
        float value = gSettings.getUSBWorkValue_Ws();
        if (value > 3600)
        {
            value /= 3600;
            fLine2 += String(value, (unsigned char)1);
            fLine2 += " Wh";
        }
        else
        {
            fLine2 += String(value, (unsigned char)1);
            fLine2 += " Ws";
        }
#endif
    }   break;
#endif
    case SettingStates::MeasureTemperature:
        fLine2 += String(gSettings.getTemperatureValue(), (unsigned char)1);
        fLine2 += " ";
        fLine2 += SettingStates::degreeC;
        fLine2 += "C";
        break;
    case SettingStates::SetAlarmMelody:
    case SettingStates::SetContrast:
    case SettingStates::SetLightLow:
    case SettingStates::SetLightHigh:
    case SettingStates::SetOnLightTime:
        fLine2 += String(gSettings.getMinutes());
        break;
    default:
        fPrintTime = gSettings.isTimerState();
    }

    if (fPrintTime)
    {
        print2Decimals(fLine2, gSettings.getHours());
        fLine2 += F(":");
        print2Decimals(fLine2, gSettings.getMinutes());
        fLine2 += F(":");
        print2Decimals(fLine2, gSettings.getSeconds());
    }

    fillStringWithSpaces(fLine2, LCD_XDIM);
    if (fLine2 != gLine2)
    {
        LCD_PRINT_AT(0, 1, fLine2);
        gLine2 = fLine2;
    }
}

void onTimerAlarm()
{
    gMelody.stopMelody();
    switch (gSettings.onTimerAlarm())
    {
    case 0: gMelody.setTones(gTones1); break;
#if NO_OF_TONES >= 2
    case 1: gMelody.setTones(gTones2); break;
#endif
#if NO_OF_TONES >= 3
    case 2: gMelody.setTones(gTones3); break;
#endif
#if NO_OF_TONES >= 4
    case 3: gMelody.setTones(gTones4); break;
#endif
#if NO_OF_TONES >= 5
    case 4: gMelody.setTones(gTones5); break;
#endif
    }
    gMelody.startMelody();
}

void onAlarm()
{
    gMelody.stopMelody();
    if (gSettings.onTimerAlarm() == SettingStates::Timers)
    {
        gMelody.setTones(gTones6);
    }
    else
    {
        int8_t fAlarm = gSettings.getTemperatureAlarmActive();
        if (fAlarm > 0)
        {
            gMelody.setTones(gTones8);
        }
        else if (fAlarm < 0)
        {
            gMelody.setTones(gTones7);
        }
        else
        {
            return;
        }
    }
    gMelody.startMelody();


}
