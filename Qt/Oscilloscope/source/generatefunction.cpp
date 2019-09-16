#include "main.h"
#include "generatefunction.h"
#include "FFT.h"
#include "pseudorandomgenerator.h"
#include "filterfunctions.h"

#include <cmath>
#include <cstdlib>
#include <time.h>

//#define VALIDATE 1

double GenerateFunction::mSampleFrequency = 44100;

GenerateFunction::GenerateFunction(QObject *parent) : QObject(parent)
  , mBuffer(0)
  , mType(sine)
  , mFrequencySine1(1000.0)
  , mFrequencySine2(1000.0)
  , mStartPhaseSine(0.0)
  , mSweepInterval(1.0)
  , mAmplitudeSine(1.0)
  , mFrequencyRectangle(1000.0)
  , mPulsewidthFactorRectangle(0.5)
  , mHighLevelRectangle(1.0)
  , mLowLevelRectangle(-1.0)
  , mFrequencyTriangle(1000.0)
  , mPulsewidthFactorTriangle(0.5)
  , mHighLevelTriangle(1.0)
  , mLowLevelTriangle(-1.0)
  , mNoiseInterval(1)
  , mUseNoiseFilter(false)
{

}

void GenerateFunction::setFrequency1(double aF)
{
    mFrequencySine1 = aF;
}

void GenerateFunction::setFrequency2(double aF)
{
    mFrequencySine2 = aF;
}

void GenerateFunction::setStartPhase(double aSPh)
{
    mStartPhaseSine = aSPh;
}

void GenerateFunction::setSweepInterval(double aSweepInterval)
{
    mSweepInterval = aSweepInterval;
}

void GenerateFunction::setAmplitudeSine(double aAmp)
{
    mAmplitudeSine = aAmp;
}

void GenerateFunction::setFrequencyRectangle(double aF)
{
    mFrequencyRectangle = aF;
}

void GenerateFunction::setPulsewidthFactorRectangle(double aF)
{
    mPulsewidthFactorRectangle = aF;
}

void GenerateFunction::setHighLevelRectangle(double aL)
{
    mHighLevelRectangle = aL;
}

void GenerateFunction::setLowLevelRectangle(double aL)
{
    mLowLevelRectangle = aL;
}

void GenerateFunction::setFrequencyTriangle(double aF)
{
    mFrequencyTriangle = aF;
}

void GenerateFunction::setPulsewidthFactorTriangle(double aF)
{
    mPulsewidthFactorTriangle = aF;
}

void GenerateFunction::setHighLevelTriangle(double aL)
{
    mHighLevelTriangle = aL;
}

void GenerateFunction::setLowLevelTriangle(double aL)
{
    mLowLevelTriangle = aL;
}

void GenerateFunction::setNoiseInterval(double aInterval)
{
    mNoiseInterval = aInterval;
}

void GenerateFunction::useNoiseFilter(bool aUse)
{
   mUseNoiseFilter = aUse;
}

void GenerateFunction::setType(int aType)
{
    mType = static_cast<eType>(aType);
}

double GenerateFunction::getFrequency1() const
{
    return mFrequencySine1;
}

double GenerateFunction::getFrequency2() const
{
    return mFrequencySine2;
}

double GenerateFunction::getStartPhase() const
{
    return mStartPhaseSine;
}

double GenerateFunction::getSweepInterval() const
{
    return mSweepInterval;
}

double GenerateFunction::getAmplitudeSine() const
{
    return mAmplitudeSine;
}

double GenerateFunction::getFrequencyRectangle() const
{
    return mFrequencyRectangle;
}

double GenerateFunction::getPulsewidthFactorRectangle() const
{
    return mPulsewidthFactorRectangle;
}

double GenerateFunction::getHighLevelRectangle() const
{
    return mHighLevelRectangle;
}

double GenerateFunction::getLowLevelRectangle() const
{
    return mLowLevelRectangle;
}

double GenerateFunction::getFrequencyTriangle() const
{
    return mFrequencyTriangle;
}

double GenerateFunction::getPulsewidthFactorTriangle() const
{
    return mPulsewidthFactorTriangle;
}

double GenerateFunction::getHighLevelTriangle() const
{
    return mHighLevelTriangle;
}

double GenerateFunction::getLowLevelTriangle() const
{
    return mLowLevelTriangle;
}

double GenerateFunction::getNoiseInterval() const
{
    return mNoiseInterval;
}

bool GenerateFunction::useNoiseFilter() const
{
    return mUseNoiseFilter;
}

FilterFunctions& GenerateFunction::getNoiseFilter()
{
    return mNoiseFilter;
}

int GenerateFunction::getType() const
{
    return mType;
}

void GenerateFunction::setBuffer(std::vector<float>* aBuffer)
{
    mBuffer = aBuffer;
}

std::vector<float>& GenerateFunction::getBuffer() const
{
    if (mBuffer)
    {
        return *mBuffer;
    }
    else
    {
        static std::vector<float> fDummy;
        return fDummy;
    }
}

void GenerateFunction::setSampleFrequency(double aSampleFrequency)
{
    mSampleFrequency = aSampleFrequency;
}

double GenerateFunction::getSampleFrequency()
{
    return mSampleFrequency;
}

void GenerateFunction::calculate()
{
    switch (mType)
    {
    case silent:        calculateSilence();     break;
    case sine:          calculateSine();        break;
    case sine_sweep:    calculateSineSweep();   break;
    case rectangle:     calculateRectangle();   break;
    case triangle:      calculateTriangle();    break;
    case noise:         calculateNoise();       break;
    default: break;
    }
}

void GenerateFunction::calculateSilence()
{
    getBuffer().resize(1);
    getBuffer()[0] = 0;
}

void GenerateFunction::calculateSine()
{
    const double fDesiredDifference = 0.00001;
    const double fMaxDegree = 360;
    double fSamplesPerSineHR = mSampleFrequency / mFrequencySine1;
    bool   fRoundUp          = false;
    int    fBestFit          = 1;
    double fMinDifference    = 1;
    int    fSamplesPerSine;
    for (int n=1; n<mFrequencySine1-1; n++)
    {
        double fSinePeriods = fSamplesPerSineHR * n;
        double fDifference = fSinePeriods - floor(fSinePeriods);
        if (fDifference < fMinDifference)
        {
            fMinDifference = fDifference;
            fBestFit = n;
            fRoundUp = false;
        }
        fDifference = ceil(fSinePeriods) - fSinePeriods;
        if (fDifference < fMinDifference)
        {
            fMinDifference = fDifference;
            fBestFit = n;
            fRoundUp = true;
        }
        if (fDifference < fDesiredDifference)
            break;
    }
    if (fRoundUp)
    {
        fSamplesPerSine = (int)ceil(fSamplesPerSineHR * fBestFit);
    }
    else
    {
        fSamplesPerSine = (int)floor(fSamplesPerSineHR * fBestFit);
    }

    if (fSamplesPerSine < 1) fSamplesPerSine = 1;
    getBuffer().resize(fSamplesPerSine);

    const double fTwoPi = 2*M_PI;
    const double fArgStep = fTwoPi*fBestFit / (double)(fSamplesPerSine);

    double fArg = fTwoPi * mStartPhaseSine / fMaxDegree;
    for(int i=0; i < fSamplesPerSine; i++, fArg += fArgStep)
    {
        if (fArg>fTwoPi) fArg-= fTwoPi;
        getBuffer()[i] = static_cast<float>(sin(fArg)*mAmplitudeSine);
    }
}

void GenerateFunction::calculateSineSweep()
{
    const double fMaxDegree = 360;
    int fBuffersize = mSampleFrequency * mSweepInterval;
    if (fBuffersize < 1) fBuffersize = 1;
    getBuffer().resize(fBuffersize);

    const double fDelta = (mFrequencySine2 - mFrequencySine1) / static_cast<double>(fBuffersize);
    double fCurrentFreq = mFrequencySine1;
    const double fTwoPi = 2*M_PI;
    double fArg = fTwoPi * mStartPhaseSine / fMaxDegree;
    for(int i = 0; i < fBuffersize; i++)
    {
        double fTwoPIDivSpS = fCurrentFreq * fTwoPi / mSampleFrequency;
        fCurrentFreq += fDelta;
        fArg += fTwoPIDivSpS;
        getBuffer()[i]  = static_cast<float>(sin(fArg)*mAmplitudeSine);
    }
}

void GenerateFunction::calculateRectangle()
{
    const double fPeriod     = 1.0 / mFrequencyRectangle;
    const int fBuffersize    = static_cast<int>(mSampleFrequency * fPeriod + 0.5);
    const double fPeriodHigh = fPeriod * mPulsewidthFactorRectangle;
    const int fSamplesHigh   = static_cast<int>(mSampleFrequency * fPeriodHigh + 0.5);

    getBuffer().resize(fBuffersize);
    std::fill(getBuffer().begin(), getBuffer().begin() + fSamplesHigh, mHighLevelRectangle);
    std::fill(getBuffer().begin() + fSamplesHigh, getBuffer().end(), mLowLevelRectangle);
}

void GenerateFunction::calculateTriangle()
{
    const double fPeriod       = 1.0 / mFrequencyTriangle;
    const int fBuffersize      = static_cast<int>(mSampleFrequency * fPeriod + 0.5);
    const double fPeriodRising = fPeriod * mPulsewidthFactorTriangle;
    const int fSamplesRising   = static_cast<int>(mSampleFrequency * fPeriodRising + 0.5);
    const int fSamplesFalling  = fBuffersize - fSamplesRising;

    getBuffer().resize(fBuffersize);
    const double fAmplitude = mHighLevelTriangle - mLowLevelTriangle;
    const double fStepUp    = fAmplitude / ((double)fSamplesRising);
    const double fStepDown  = fAmplitude / ((double)fSamplesFalling);
    double fArg       = mLowLevelTriangle;
    for(int i = 0; i < fBuffersize; i++)
    {
        getBuffer()[i]  = fArg;
        if (i < fSamplesRising)
        {
            fArg += fStepUp;
        }
        else
        {
            fArg -= fStepDown;
        }
    }
}

void GenerateFunction::calculateNoise()
{
    int fMinimumSize    = static_cast<int>(mSampleFrequency * mNoiseInterval + 0.5);
    int fOrder = 0;
    {
        int fOdd   = 0;
        while (fMinimumSize)
        {
            if (fMinimumSize&1) fOdd ++;
            fOrder++;
            fMinimumSize >>= 1;
        }
        if (fOdd == 1) fOrder--;
    }
    int fCalculatedSize  = 1 << fOrder;
    int fRandomArraySize = 1 << (fOrder+1);
    std::vector<double> fRandomValues(fRandomArraySize);


#if RANDOM == 1
    time_t fTime = time(NULL);
    srand(fTime);
    double dNorm = 1.0 / static_cast<double>(RAND_MAX);
#else
    PseudoRandomGenerator fRG;
    double dNorm = 1.0 / static_cast<double>(RAND_MAX);
 #endif

    for(int i = 0; i < fRandomArraySize; i++)
    {
#if RANDOM == 1
        fRandomValues[i] = dNorm * static_cast<double>(rand());
#else
        fRandomValues[i] = dNorm * static_cast<double>(fRG.getRandomValue());
#endif
    }

    calcFFT(0, fOrder, &fRandomValues[0]);
    if (mUseNoiseFilter)
    {
        double fFrequencyStep  = mSampleFrequency / (double) fCalculatedSize;
        mNoiseFilter.calculateFilter(fFrequencyStep, fRandomValues);
#ifdef VALIDATE
        std::vector<double> fValues;
        mNoiseFilter.calculateFilter(fFrequencyStep, fValues, true);
        fValues.resize(mNoiseFilter.getFilterFunction().size());
        calcAmplitude(&mNoiseFilter.getFilterFunction()[0], fValues);
        Logger::printCurve(mNoiseFilter.getFilterFrequenzcies(), fValues, "Amplitudes" );
        calcPhase(&mNoiseFilter.getFilterFunction()[0], fValues);
        Logger::printCurve(mNoiseFilter.getFilterFrequenzcies(), fValues, "Phases" );
#endif
    }
    calcFFT(1, fOrder, &fRandomValues[0]);

    dNorm = 0;

    // TODO find endpoint to avoid click
//    int iFound = fCalculatedSize-1;
//    double fMin = fabs(fRandomValues[iFound] - fRandomValues[1]);
//    for(int i=fCalculatedSize-1; i > 1; --i)
//    {
//        if (fabs(fRandomValues[i] - fRandomValues[1]) < fMin)
//        {
//            iFound = i;
//            fMin = fabs(fRandomValues[i] - fRandomValues[1]);
//        }
//    }
//    fCalculatedSize = iFound;

    getBuffer().resize(fCalculatedSize-1);

    for(int i=1; i < fCalculatedSize; ++i)
    {
        double dTemp = fabs(fRandomValues[i]);
        if (dTemp > dNorm) dNorm = dTemp;
    }
    dNorm = 5.0 / dNorm;
    for(int i=1; i < fCalculatedSize; ++i)
    {
        getBuffer()[i] = fRandomValues[i] * dNorm;
    }
}

