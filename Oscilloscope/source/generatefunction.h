#ifndef GENERATEFUNCTION_H
#define GENERATEFUNCTION_H
#include "filterfunctions.h"
#include <QObject>
#include <vector>

class GenerateFunction : public QObject
{
    Q_OBJECT
public:
    enum eType
    {
        silent,
        sine,
        sine_sweep,
        rectangle,
        triangle,
        noise
    };

    explicit GenerateFunction(QObject *parent = 0);

    void setFrequency1(double aF);
    void setFrequency2(double aF);
    void setStartPhase(double aSPh);
    void setSweepInterval(double aSweepPeriod);
    void setAmplitudeSine(double aAmp);

    void setFrequencyRectangle(double aF);
    void setPulsewidthFactorRectangle(double aF);
    void setHighLevelRectangle(double aL);
    void setLowLevelRectangle(double aL);

    void setFrequencyTriangle(double aF);
    void setPulsewidthFactorTriangle(double aF);
    void setHighLevelTriangle(double aL);
    void setLowLevelTriangle(double aL);

    void setNoiseInterval(double aInterval);
    void useNoiseFilter(bool aUse);


    void setType(int aType);

    double getFrequency1() const;
    double getFrequency2() const;
    double getStartPhase() const;
    double getSweepInterval() const;
    double getAmplitudeSine() const;

    double getFrequencyRectangle() const;
    double getPulsewidthFactorRectangle() const;
    double getHighLevelRectangle() const;
    double getLowLevelRectangle() const;

    double getFrequencyTriangle() const;
    double getPulsewidthFactorTriangle() const;
    double getHighLevelTriangle() const;
    double getLowLevelTriangle() const;

    double getNoiseInterval() const;
    bool   useNoiseFilter() const;
    FilterFunctions& getNoiseFilter() ;

    int    getType() const;

    void calculate();

    void setBuffer(std::vector<float>* aBuffer);
    static void   setSampleFrequency(double aSampleFrequency);
    static double getSampleFrequency() ;
Q_SIGNALS:

public Q_SLOTS:

private:
   void calculateSilence();
   void calculateSine();
   void calculateSineSweep();
   void calculateRectangle();
   void calculateTriangle();
   void calculateNoise();

   std::vector<float>& getBuffer() const;
   std::vector<float>* mBuffer;

   eType  mType;

   double mFrequencySine1;
   double mFrequencySine2;
   double mStartPhaseSine;
   double mSweepInterval;
   double mAmplitudeSine;

   double mFrequencyRectangle;
   double mPulsewidthFactorRectangle;
   double mHighLevelRectangle;
   double mLowLevelRectangle;

   double mFrequencyTriangle;
   double mPulsewidthFactorTriangle;
   double mHighLevelTriangle;
   double mLowLevelTriangle;

   double mNoiseInterval;
   FilterFunctions mNoiseFilter;
   bool   mUseNoiseFilter;

   static double mSampleFrequency;
};

#endif // GENERATEFUNCTION_H
