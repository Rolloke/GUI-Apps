#include "inputpin.h"
#include <map>
#include <string>

InputPin::InputPin()
    : mThreshold(-1)
    , mReport(-1)
{

}



QString InputPin::getPinDefinitions() const
{
    QString                     fPinDefinitions =  QString::asprintf("INPUT[%d]:PIN[%d]:%s\n", mIndex, mPinNumber, PinCommon::name_of(mPinType));
    if (mEdge != Edge::Unknown) fPinDefinitions += QString::asprintf("INPUT[%d]:EDGE:%s\n", mIndex, PinCommon::name_of(mEdge));
    switch (mPinType)
    {
    case PinCommon::Type::Switch:
        if (mDelay != -1)       fPinDefinitions += QString::asprintf("INPUT[%d]:DELAY:%d\n", mIndex, mDelay);
        if (mPulse != -1)       fPinDefinitions += QString::asprintf("INPUT[%d]:PULSE:%d\n", mIndex, mPulse);
        if (mRepeat != -1)      fPinDefinitions += QString::asprintf("INPUT[%d]:REPEAT:%d\n", mIndex, mRepeat);
        break;
    case PinCommon::Type::Analog:
        if (mThreshold != -1)   fPinDefinitions += QString::asprintf("INPUT[%d]:THRESHOLD:%d\n", mIndex, mThreshold);
        if (mPulse != -1)       fPinDefinitions += QString::asprintf("INPUT[%d]:PULSE:%d\n", mIndex, mPulse);
        if (mReport != -1)      fPinDefinitions += QString::asprintf("INPUT[%d]:REPORT:%d\n", mIndex, mReport);
        break;
    case PinCommon::Type::Interrupt:
    case PinCommon::Type::Binary:
    default:
        break;
    }
    if (mConnection > 0)
    {
        fPinDefinitions += QString::asprintf("INPUT[%d]:CONNECT:OUTPUT[%d]\n", mIndex, mConnection);
    }

    return fPinDefinitions;
}
