#include "TriggerParameter.h"
#include "main.h"
#include "matrix.h"

Trigger::Trigger()
    : edge(0)
    , slope(0)
    , inv_timestep(1)
{

}

Trigger::Trigger(double aEdge, const cvditerator& aIt)
    : edge(aEdge)
    , slope(0)
    , inv_timestep(1)
    , offset(0)
{
    current = last = aIt;
}

void Trigger::updateTrigger()
{
    last    = current;
}

bool Trigger::isRising( Trigger& aCompare)
{
    return (*aCompare.current > aCompare.edge && *aCompare.last <= aCompare.edge);
}

bool Trigger::isFalling( Trigger& aCompare)
{
    return (*aCompare.current <= aCompare.edge && *aCompare.last > aCompare.edge);
}

bool Trigger::isSlope(Trigger& aCompare)
{
    int fDiff = std::distance(aCompare.last, aCompare.current);
    if      (fDiff >  1) fDiff = -1;
    else if (fDiff < -1) fDiff =  1;
#if 0
    double f1  = *aCompare.current;
    double f2  = *aCompare.last;
    double f3  = *aCompare.last_m1;
    double fSlope1 = (f2 - f1) * aCompare.inv_timestep;
    double fSlope2 = (f3 - f2) * aCompare.inv_timestep;
    //Logger::printDebug(Logger::warning, "%f, %f, %f: %f < %f < %f\n", f1, f2, f3, fSlope1, aCompare.slope, fSlope2);
    if (aCompare.slope > 0) return  fSlope1 < aCompare.slope && fSlope2 >= aCompare.slope ;
    else                    return  fSlope1 > aCompare.slope && fSlope2 <= aCompare.slope ;
#else
    // TODO improve Triggerslope function
    double fX = 1.0 / aCompare.inv_timestep;
    std::vector<qreal> fXval;
    std::vector<qreal> fYval;
    auto fIter = aCompare.current;
    for (int i=0; i < 5; ++i)
    {
        fXval.push_back(fX*i);
        fYval.push_back(*fIter);
        if (fDiff > 0) ++fIter;
        else           --fIter;
    }
    double fSlope = Trigger::calculateSlope(fXval, fYval, fX, 2);
    if (fabs(aCompare.slope - fSlope) < fabs(aCompare.slope) * 0.01)
    {
        return true;
    }
    return false;
#endif
}

double Trigger::calculateSlope(const std::vector<qreal>& aXvalues, const std::vector<qreal>& aYvalues, qreal aX, int aPolynomeOrder)
{
    using namespace Algorithmics;
    if (aPolynomeOrder == 0) aPolynomeOrder = aXvalues.size();
    std::vector<qreal> fPolynome(aPolynomeOrder);
    if (Matrix::fitCurveToPolynome(aXvalues, aYvalues, fPolynome))
    {
        std::vector<qreal> fDerived;
        Helper::Polynome::derivePolynome(fPolynome, fDerived);
        return Helper::Polynome::calcPolynomeValueByHorner(aX, fDerived);
    }
    return 0;
}

double Trigger::calculateX_AtSlope(const std::vector<qreal>& aXvalues, const std::vector<qreal>& aYvalues, qreal aSlope)
{
    using namespace Algorithmics;
    std::vector<qreal> fPolynome(3);
    if (Matrix::fitCurveToPolynome(aXvalues, aYvalues, fPolynome))
    {
        std::vector<qreal> fDerived;
        Helper::Polynome::derivePolynome(fPolynome, fDerived);
        return  (aSlope - fDerived[0]) / fDerived[1];
    }
    return 0;
}


TriggerParameter::TriggerParameter() :
  mType(TriggerType::Off)
, mActiveChannel(InvalidIndex)
, mEdgeRising(true)
, mHoldOffType(HoldOff::Off)
, mHoldOffTime(0)
, mHoldOffEvents(1)
, mHoldOffPosition(InvalidIndex)
, mHoldOffWrapAround(0)
, mLastTriggerpoint(InvalidIndex)
, mDelayActive(false)
, mDelay(0)
, mSingleTriggerDelayBuffers(-1)
, mSingleTriggerPos(0)
, mSingleTriggerOffset(0)
, mSlopeActive(false)
, mSlope(0)
{

}
