#ifndef INPUTPIN_H
#define INPUTPIN_H

#include "pincommon.h"

class InputPin : public PinCommon
{
public:
    InputPin();


    QString getPinDefinitions() const;

    int  mThreshold;
    int  mReport;

    static const int analog_value_range = 1023;

};

#endif // INPUTPIN_H
