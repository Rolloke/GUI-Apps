#include "Channel.h"

Channel::Channel() :
  mLevelPerDivision(0)
  , mOffset(0)
  , mOffsetIndex(0)
  , mTriggerLevel(0)
  , mAudioInputScale(1)
  , mAudioInputOffset(0)
  , mAudioScale(1)
  , mVisible(true)
  , mPen(Qt::SolidLine)
{
    mPen.setWidth(0);
}

void Channel::setMinMaxValue(double aMin, double aMax)
{
    mAvgMinValue.addValue(aMin);
    mAvgMaxValue.addValue(aMax);
}

double Channel::getLevelPerDivision(bool aScaled) const
{
    return mLevelPerDivision * (aScaled ? mAudioScale : 1.0);
}

double& Channel::setLevelPerDivision()
{
    return mLevelPerDivision;
}

double Channel::getOffset() const
{
    return mOffset;
}

double& Channel::setOffset()
{
    return mOffset;
}

int Channel::getOffsetIndex() const
{
    return mOffsetIndex;
}

int& Channel::setOffsetIndex()
{
    return mOffsetIndex;
}


double  Channel::getAudioScale() const
{
    return mAudioScale;
}

double& Channel::setAudioScale()
{
    return mAudioScale;
}

double  Channel::getAudioInputScale() const
{
    return mAudioInputScale;
}

double& Channel::setAudioInputScale()
{
    return mAudioInputScale;
}

double  Channel::getAudioInputOffset() const
{
    return mAudioInputOffset;
}

double& Channel::setAudioInputOffset()
{
    return mAudioInputOffset;
}

double  Channel::getTriggerLevel() const
{
    return mTriggerLevel;
}

double& Channel::setTriggerLevel()
{
    return mTriggerLevel;
}

void Channel::setNoOfAvgValues(int aValues)
{
    mAvgMinValue.setNoOfAvgValues(aValues);
    mAvgMaxValue.setNoOfAvgValues(aValues);
}

double  Channel::getAvgMinValue() const
{
    return mAvgMinValue.getAverage();
}

double  Channel::getAvgMaxValue() const
{
    return mAvgMaxValue.getAverage();
}

