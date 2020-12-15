#include "pintype.h"
#include "pincommon.h"

PinType::PinType()
    : mName("")
    , mPinNumber(-1)
    , mInputPin(false)
    , mOutputPin(false)
    , mAnalog(false)
    , mPWM(false)
    , mTone(false)
    , mBinary(false)
    , mInterrupt(false)
{
}

PinType::PinType(const QString& aName, int aNo, bool aInput, bool aOutput, bool aAnalog, bool aPWM, bool aTone, bool aBinary, bool aInterrupt)
    : mName(aName)
    , mPinNumber(aNo)
    , mInputPin(aInput)
    , mOutputPin(aOutput)
    , mAnalog(aAnalog)
    , mPWM(aPWM)
    , mTone(aTone)
    , mBinary(aBinary)
    , mInterrupt(aInterrupt)
{
    init_types();
}

void PinType::init_types()
{
    if (mAnalog)      mInputTypes.append(PinCommon::name_of(PinCommon::Type::Analog));
    if (!mOutputPin)  mInputTypes.append(PinCommon::name_of(PinCommon::Type::Switch));
    else if (mBinary) mInputTypes.append(PinCommon::name_of(PinCommon::Type::Binary));
    if (mInterrupt)   mInputTypes.append(PinCommon::name_of(PinCommon::Type::Interrupt));

    if (mBinary)     mOutputTypes.append(PinCommon::name_of(PinCommon::Type::Binary));
    if (mPWM)        mOutputTypes.append(PinCommon::name_of(PinCommon::Type::Analog));
    if (mTone)       mOutputTypes.append(PinCommon::name_of(PinCommon::Type::Tone));
}

const QString& PinType::getName() const
{
    return mName;
}

int PinType::getPinNo() const
{
    return mPinNumber;
}
bool PinType::isInput() const
{
    return mInputPin;
}
bool PinType::isOutput() const
{
    return mOutputPin;
}
bool PinType::isAnalog() const
{
    return mAnalog;
}
bool PinType::isPWM() const
{
    return mPWM;
}
bool PinType::isTone() const
{
    return mTone;
}
bool PinType::isBinary() const
{
    return mBinary;
}
bool PinType::isInterrupt() const
{
    return mInterrupt;
}

const QStringList& PinType::getTypes(bool aOutput) const
{
    return aOutput ? mOutputTypes : mInputTypes;
}



