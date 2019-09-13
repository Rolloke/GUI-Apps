#include "outputpin.h"

OutputPin::OutputPin()
    : mValue(-1)
    , mInvert(-1)
    , mToggle(-1)

{
    mRepeat =-2;
}

QString OutputPin::getPinDefinitions() const
{
    QString                     fPinDefinitions =  QString::asprintf("OUTPUT[%d]:PIN[%d]:%s\n", mIndex, mPinNumber, PinCommon::name_of(mPinType));
    if (mEdge != Edge::Unknown) fPinDefinitions += QString::asprintf("OUTPUT[%d]:EDGE:%s\n", mIndex, PinCommon::name_of(mEdge));
    if (mDelay != -1)           fPinDefinitions += QString::asprintf("OUTPUT[%d]:DELAY:%d\n", mIndex, mDelay);
    if (mPulse != -1)           fPinDefinitions += QString::asprintf("OUTPUT[%d]:PULSE:%d\n", mIndex, mPulse);
    if (mRepeat != -2)          fPinDefinitions += QString::asprintf("OUTPUT[%d]:REPEAT:%d\n", mIndex, mRepeat);
    if (mValue != -1)           fPinDefinitions += QString::asprintf("OUTPUT[%d]:VALUE:%d\n", mIndex, mValue);
    if (mInvert != -1)          fPinDefinitions += QString::asprintf("OUTPUT[%d]:INVERT:%d\n", mIndex, mInvert);
    if (mToggle != -1)          fPinDefinitions += QString::asprintf("OUTPUT[%d]:TOGGLE:%d\n", mIndex, mToggle);
    if (mConnection > 0)        fPinDefinitions += QString::asprintf("OUTPUT[%d]:CONNECT:OUTPUT[%d]\n", mIndex, mConnection);


    return fPinDefinitions;
}
