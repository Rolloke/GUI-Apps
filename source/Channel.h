#ifndef CHANNEL_H
#define CHANNEL_H

#include <QPen>
#include <QString>
#include <boost/function.hpp>
#include "main.h"

#include "Average.h"

class Channel
{
public:
    Channel();

    double  getLevelPerDivision(bool aScaled = true) const;
    double& setLevelPerDivision();

    double  getOffset() const;
    double& setOffset();

    int     getOffsetIndex() const;
    int&    setOffsetIndex();

    double  getAudioScale() const;
    double& setAudioScale();

    double  getAudioInputScale() const;
    double& setAudioInputScale();

    double  getAudioInputOffset() const;
    double& setAudioInputOffset();

    double  getTriggerLevel() const;
    double& setTriggerLevel();

    void    setNoOfAvgValues(int aValues);
    void    setMinMaxValue(double aMin, double aMax);
    double  getAvgMinValue() const;
    double  getAvgMaxValue() const;

private:
    double mLevelPerDivision;
    double mOffset;
    int    mOffsetIndex;
    double mTriggerLevel;

    double mAudioInputScale;
    double mAudioInputOffset;
    double mAudioScale;

    Average<double> mAvgMinValue;
    Average<double> mAvgMaxValue;

public:

    bool   mVisible;
    QPen   mPen;
    QString mName;
};

struct PaStreamCallbackTimeInfo;

#if (_USE_QT_AUDIO == 1)
typedef boost::function< int( const void *, unsigned long ) > AudioCallback;
#else
typedef boost::function< int( const void *, void *, unsigned long, const PaStreamCallbackTimeInfo*, unsigned long ) > AudioCallback;
#endif

#endif // CHANNEL_H
