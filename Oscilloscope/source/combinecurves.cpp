#include "combinecurves.h"
#include "circlevector.h"

CombineCurves::CombineCurves(QObject *parent) : QObject(parent)
  , mType(none)
  , mChannel1(0)
  , mChannel2(1)
  , mMathChannelIndex(0)
{
}

void CombineCurves::setType(eType aType)
{
    mType = aType;
}

void CombineCurves::setChannel1(int aC1)
{
    mChannel1 = aC1;
}

void CombineCurves::setChannel2(int aC2)
{
    mChannel2 = aC2;
}

void CombineCurves::setMathChannelIndex(int aMathChannelIndex)
{
    mMathChannelIndex = aMathChannelIndex;
}

CombineCurves::eType CombineCurves::getType() const
{
    return mType;
}

int CombineCurves::getChannel1() const
{
    return mChannel1;
}

int CombineCurves::getChannel2() const
{
    return mChannel2;
}

int CombineCurves::getMathChannelIndex() const
{
    return mMathChannelIndex;
}

double CombineCurves::f_add(double a1, double a2)
{
    return a1+a2;
}

double CombineCurves::f_subtract(double a1, double a2)
{
    return a1-a2;
}

double CombineCurves::f_multiply(double a1, double a2)
{
    return a1*a2;
}

double CombineCurves::f_divide(double a1, double a2)
{
    if (a2 != 0.0) return a1/a2;
    return 0.0;
}

bool CombineCurves::combineCurves(const std::vector<double>& aC1, const std::vector<double>& aC2, std::vector<double>& aCout, int aStart, int aStop) const
{
    bool fReturn = false;
    double (*fCombine)(double, double) = nullptr;
    const circlevector<double>& fCurve1 = static_cast<const circlevector<double>&>(aC1);
    const circlevector<double>& fCurve2 = static_cast<const circlevector<double>&>(aC2);
    switch (mType)
    {
    case add:      fCombine = f_add;      break;
    case subtract: fCombine = f_subtract; break;
    case multiply: fCombine = f_multiply; break;
    case divide:   fCombine = f_divide;   break;
    default:                              break;
    }
    if (fCombine)
    {
        int fSize = 0;
        if (aStart > aStop)
        {
            fSize = aC1.size() - aStart + aStop;
        }
        else
        {
            fSize = aStop - aStart;
        }
        aCout.resize(fSize);
        fReturn = fSize > 0;

        circlevector<double>::const_circlevector_iterator fCI1 = fCurve1.begin(aStart);
        circlevector<double>::const_circlevector_iterator fCI2 = fCurve2.begin(aStart);
        for (int i=0; i<fSize; ++i, ++fCI1, ++fCI2)
        {
            aCout[i] = fCombine(*fCI1, *fCI2);
        }
    }
    return fReturn;
}
