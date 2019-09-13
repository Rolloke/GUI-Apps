#ifndef OUTPUTPIN_H
#define OUTPUTPIN_H

#include "pincommon.h"

class OutputPin : public PinCommon
{
public:
    OutputPin();

    int mValue;
    int mInvert;
    int mToggle;

    static const int analog_value_range = 255;

    QString getPinDefinitions() const;

};

#endif // OUTPUTPIN_H
