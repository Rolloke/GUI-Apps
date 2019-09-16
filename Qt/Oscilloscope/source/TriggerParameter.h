#ifndef TRIGGER_PARAMETER_H
#define TRIGGER_PARAMETER_H
#include "circlevector.h"

typedef circlevector<double>::const_circlevector_iterator cvditerator;

enum eConstants
{
    InvalidIndex = -1,
    InitializationIndex = -2
};

struct TriggerType
{
    enum eType
    {
        Stop,   // standbild
        Off,    // without trigger
        Single, // wait for single trigger signal
        Auto,   // trigger but also show picture, if no trigger comes in
        Normal, // show picture only if a trigger signal is detected
        Manual  // trigger on manual button signal
    };
};

struct Trigger
{
    Trigger();
    Trigger(double aEgde, const cvditerator& aIt);
    void updateTrigger();
    double edge;         // trigger rising, falling egdge
    double slope;        // slope trigger value
    double inv_timestep; // current timestep for slope calculation
    double offset;       // calculated offset
    cvditerator current;
    cvditerator last;

    static bool isRising( Trigger& aCompare);
    static bool isFalling( Trigger& aCompare);
    static bool isSlope( Trigger& aCompare);

    static double calculateSlope(const std::vector<qreal>&, const std::vector<qreal>&, qreal, int aPolynomeOrder=0);
    static double calculateX_AtSlope(const std::vector<qreal>&, const std::vector<qreal>&, qreal );


    static void forward(Trigger& aTrigger)
    {
        ++aTrigger.current;
    }

    static void backward(Trigger& aTrigger)
    {
        --aTrigger.current;
    }
};

struct TriggerParameter
{
    struct HoldOff
    {
        enum eType { Off, Time, Events };
    };
    TriggerParameter();

    TriggerType::eType   mType;
    int                  mActiveChannel;
    bool                 mEdgeRising;

    HoldOff::eType       mHoldOffType;
    double               mHoldOffTime;
    int                  mHoldOffEvents;
    int                  mHoldOffPosition;
    int                  mHoldOffWrapAround;
    int                  mLastTriggerpoint;

    bool                 mDelayActive;
    double               mDelay;

    int                  mSingleTriggerDelayBuffers;
    int                  mSingleTriggerPos;
    double               mSingleTriggerOffset;

    bool                 mSlopeActive;
    double               mSlope;
};

#endif // TRIGGER_PARAMETER_H
