#include "filterfunctions.h"
#include "FFT.h"

FilterFunctions::FilterFunctions()
    : mHPOrder(One)
    , mHPType(Butterworth)
    , mHPQFaktor(1.0)
    , mHPCutOffFrequency(1000)
    , mTPOrder(One)
    , mTPType(Butterworth)
    , mTPQFaktor(1.0)
    , mTPCutOffFrequency(1000)
    , mNoOfFrequencies(120)
{
}


void FilterFunctions::calculateFilter(double aStep, std::vector<double>& aFourier, bool aGetFilterFunction)
{
   int    i, j;
   double dFrequ (0);
   int aFrequ;

   if (aGetFilterFunction)
   {
      aFrequ = mNoOfFrequencies;
      mFilterBox.resize(aFrequ);
      mFilterFrq.clear();
   }
   else
   {
      dFrequ = 0;
      aFrequ = aFourier.size()/2;
   }

   if (mTPType == Pink)  // Pink Noise
   {
      double  dF0 = mTPCutOffFrequency;

      for(i = 0; i < aFrequ; i++ )
      {
         if (aGetFilterFunction) dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         else            dFrequ += aStep;

         if (aGetFilterFunction) // Filterfunktin im Array ablegen
         {
            mFilterBox[i] = calculatePinkNoiseFilter(dF0, dFrequ);
            mFilterFrq.push_back(dFrequ);
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            multiplyFilter(calculatePinkNoiseFilter(dF0, dFrequ), aFourier[j], aFourier[j+1]);
         }
      }
   }
   else                                             // Others
   {
      std::complex<double>     cSn;                              // = j*2Pi*freq/2Pi*Grenzfreq
      std::complex<double>     cFilterKoeff1, cFilterKoeff2;

      double      da1[5][5], da2[5][5], db1[5][5], db2[5][5]; // Koeffizienten[Order][Type] der Filterpolynome

      for( i = 0; i < 5; i++ )                      // alle Koeff. auf Null setzen, max. Ordnung=4; max. Typ=4
      {                                             // i = Ordnung, j = Typ
         for( j = 0; j < 5; j++ )
         {
            da1[i][j] = 0.0;
            da2[i][j] = 0.0;
            db1[i][j] = 0.0;
            db2[i][j] = 0.0;
         }
      }
      da1[1][0] = da1[1][2] = da1[1][3] = 1.0;      // 1. Ordnung nur f�r Butterworth, Bessel u. Tschebyscheff

      da1[2][0] = 1.4142;                           // 2. Ordnung, Butterworth
      db1[2][0] = 1.0;
      da1[2][1] = 1.0;                              // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      db1[2][1] = 1.0;
      da1[2][2] = 1.3617;                           // 2. Ordnung, Bessel
      db1[2][2] = 0.6180;
      da1[2][3] = 1.3614;                           // 2. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
      db1[2][3] = 1.3827;

      da1[3][0] = 1.0;                              // 3. Ordnung, Butterworth
      da2[3][0] = 1.0;
      db2[3][0] = 1.0;
      da1[3][1] = 1.0;                              // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      da2[3][1] = 1.0;
      db2[3][1] = 1.0;
      da1[3][2] = 0.7560;                           // 3. Ordnung, Bessel
      da2[3][2] = 0.9996;
      db2[3][2] = 0.4772;
      da1[3][3] = 1.8636;                           // 3. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
      da2[3][3] = 0.6402;
      db2[3][3] = 1.1931;

      da1[4][0] = 1.8478;                           // 4. Ordnung, Butterworth
      db1[4][0] = 1.0;
      da2[4][0] = 0.7654;
      db2[4][0] = 1.0;
      da1[4][2] = 1.3397;                           // 4. Ordnung, Bessel
      db1[4][2] = 0.4889;
      da2[4][2] = 0.7743;
      db2[4][2] = 0.3890;
      da1[4][3] = 2.6282;                           // 4. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
      db1[4][3] = 3.4341;
      da2[4][3] = 0.3648;
      db2[4][3] = 1.1509;
      da1[4][4] = 1.4142;                           // 4. Ordnung, Linkwitz-Riley
      db1[4][4] = 1.0;
      da2[4][4] = 1.4142;
      db2[4][4] = 1.0;


      for( i = 0; i < aFrequ; i++ )
      {
         if (aGetFilterFunction)
         {
            dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         }
         else
         {
            dFrequ += aStep;
         }
         cFilterKoeff1 = std::complex<double>( 1.0, 0.0 );
         cFilterKoeff2 = std::complex<double>( 1.0, 0.0 );

         if( mHPCutOffFrequency > 0.0 )
            cSn = std::complex<double>( 0.0, dFrequ/mHPCutOffFrequency );
         else
            cSn = std::complex<double>( 0.0, dFrequ);           // HPFreq = 1 HZ

         if( (mHPType == ButterworthVariableQ) && (mHPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/mHPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/mHPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (mHPOrder >= One) && (mHPOrder <= Four) &&
             (mHPType >= Butterworth) && (mHPType  <= LinkwitzRiley) )
         {
            cFilterKoeff1  = 1.0 + da1[mHPOrder][mHPType]/cSn +
                                   db1[mHPOrder][mHPType]/(cSn*cSn);
            cFilterKoeff1 *= 1.0 + da2[mHPOrder][mHPType]/cSn +
                                   db2[mHPOrder][mHPType]/(cSn*cSn);
            cFilterKoeff1  = 1.0/cFilterKoeff1;
         }

         if( mTPCutOffFrequency > 0.0 )
         {
            cSn = std::complex<double>( 0.0, dFrequ/mTPCutOffFrequency);
         }
         else
         {
            cSn = std::complex<double>( 0.0, dFrequ/1000000.0 ); // TPFreq = 1 MHZ
         }

         if( (mTPType == ButterworthVariableQ) && (mTPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/mTPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/mTPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (mTPOrder >= One) && (mTPOrder <= Four) &&
             (mTPType >= Butterworth) && (mTPType  <= LinkwitzRiley) )
         {
            cFilterKoeff2  = 1.0 + da1[mTPOrder][mTPType]*cSn +
                                   db1[mTPOrder][mTPType]*cSn*cSn;
            cFilterKoeff2 *= 1.0 + da2[mTPOrder][mTPType]*cSn +
                                   db2[mTPOrder][mTPType]*cSn*cSn;
            cFilterKoeff2  = 1.0/cFilterKoeff2;
         }

         if (aGetFilterFunction) // Filterfunktin im Array ablegen
         {
             mFilterBox[i] = cFilterKoeff1*cFilterKoeff2;
             mFilterFrq.push_back(dFrequ);
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            multiplyFilter(cFilterKoeff1*cFilterKoeff2, aFourier[j], aFourier[j+1]);
         }
      }
   }
}

void FilterFunctions::setHighPassType(eType aType)
{
    mHPType = aType;
}

void FilterFunctions::setHighPassOrder(eOrder aOrder)
{
    mHPOrder = aOrder;
}

void FilterFunctions::setHighPassQ(double aQ)
{
    mHPQFaktor = aQ;
}

void FilterFunctions::setHighPassCutOff(double aFrq)
{
    mHPCutOffFrequency = aFrq;
}

FilterFunctions::eType  FilterFunctions::getHighPassType() const
{
    return mHPType;
}

FilterFunctions::eOrder FilterFunctions::getHighPassOrder() const
{
    return mHPOrder;
}

double FilterFunctions::getHighPassQ() const
{
    return mHPQFaktor;
}

double FilterFunctions::getHighPassCutOff() const
{
    return mHPCutOffFrequency;
}

void FilterFunctions::setLowPassType(eType aType)
{
    mTPType = aType;
}

void FilterFunctions::setLowPassOrder(eOrder aOrder)
{
    mTPOrder = aOrder;
}

void FilterFunctions::setLowPassQ(double aQ)
{
    mTPQFaktor = aQ;
}

void FilterFunctions::setLowPassCutOff(double aFrq)
{
    mTPCutOffFrequency = aFrq;
}

FilterFunctions::eType  FilterFunctions::getLowPassType() const
{
    return mTPType;
}

FilterFunctions::eOrder FilterFunctions::getLowPassOrder() const
{
    return mTPOrder;
}

double FilterFunctions::getLowPassQ() const
{
    return mTPQFaktor;
}

double FilterFunctions::getLowPassCutOff() const
{
    return mTPCutOffFrequency;
}

const char* FilterFunctions::getNameOfType(eType aType)
{
    switch (aType)
    {
    case LowPassHalfOrder:     return "Pink Noise";
    case Butterworth:          return "Butterworth";
    case ButterworthVariableQ: return "Butterworth Q";
    case Bessel:               return "Bessel";
    case Tschebyscheff:        return "Tschebyscheff";
    case LinkwitzRiley:        return "Linkwitz Riley";
    default:                   return "Unknown";
    }
}

void FilterFunctions::multiplyFilterExp(double real, double imag, double &a, double &b)
{
   std::complex<double> cFK(real, imag);
   multiplyFilter(cFK, a, b);
}

void FilterFunctions::multiplyFilter(std::complex<double> cFK, double &a, double &b)
{
   double  dA    = hypot (a,b);
   double  dPhi  = atan2(a,b);
   std::complex<double> cTemp = dA * std::complex<double>(cos(dPhi), sin(dPhi)) * cFK;
   dA   = abs(cTemp);
   dPhi = atan2(cTemp.imag(), cTemp.real());
   a    = dA * sin(dPhi);
   b    = dA * cos(dPhi);
}

void FilterFunctions::calculatePinkNoiseFilterExp(double dF0, double dFrequ, std::complex<double>&pErg)
{
   pErg = calculatePinkNoiseFilter(dF0, dFrequ);
}

std::complex<double> FilterFunctions::calculatePinkNoiseFilter(double dF0, double dFrequ)
{
   double dArgument = atan2(-dFrequ, dF0) * 0.5;
   return std::complex<double>(cos(dArgument), sin(dArgument)) / pow(1.0 + (dFrequ*dFrequ)/(dF0*dF0), 0.25);
}


