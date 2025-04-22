
#include "FFT.h"
#include "main.h"

#include <cmath>

#include "simd.h"


namespace
{
    double sinc(double fSincArg)
    {
        if (fSincArg == 0)
        {
            return 1.0;
        }
        double fArg = fSincArg * M_PI;
        return sin(fArg) / fArg;
    }
}

double calcRectFilter(int, int)
{
    // Integral ueber das Fenster: 1.0
    return 1.0;
}

double calcHammingFilter(int aSize, int aIndex)
{
    // Integral ueber das Fenster: 0.54
    return 0.54 - 0.46 * cos(2 * M_PI * aIndex / (aSize - 1));
}

double calcHanningFilter(int aSize, int aIndex)
{
    // Integral ueber das Fenster: 0.5
    return 0.5 * (1 - cos(2 * M_PI * aIndex / (aSize - 1)));
}

double calcCosineFilter(int aSize, int aIndex)
{
    // Integral ueber das Fenster: 0.63646431640192724
    return sin(M_PI * aIndex / (aSize - 1));
}

double calcGaussFilter(int aSize, int aIndex)
{
    double sigma = 0.5;
    double d = ((aIndex - (aSize - 1) * 0.5) / (sigma * (aSize - 1) * 0.5));
    // Integral ueber das Fenster: 0.59803100549220389
    return exp(-0.5 * d * d);
}

double calcLanczosFilter(int aSize, int aIndex)
{
    double fHalf = aSize * 0.5;
    return sinc((aIndex - fHalf) / fHalf);
}


void calcPhase(const std::complex<double>*pdFourier, std::vector<double>& aDest)
{
    const double fDeg2Rad = 180/M_PI;
    for (unsigned int j=0; j<aDest.size(); ++j)
    {
        aDest[j] = arg(pdFourier[j])*fDeg2Rad;
    }
}

void calcReal(const std::complex<double>*pdFourier, std::vector<double>& aDest)
{
    for (unsigned int j=0; j<aDest.size(); ++j)
    {
        aDest[j] = pdFourier[j].real();
    }
}

void calcImaginary(const std::complex<double>*pdFourier, std::vector<double>& aDest)
{
    for (unsigned int j=0; j<aDest.size(); ++j)
    {
        aDest[j] = pdFourier[j].imag();
    }
}

//#include <immintrin.h>

#if USE_SIMD != 0
void calcAmplitude(const double*pdFourier, std::vector<double>&p, int n)
{
/// TODO: test this
//    _may_i_use_cpu_feature(0);
//    __cpufeature(1);
//    _mm_cpufeature(2);

    typedef float myfloat;
    SimdVar<myfloat> a, b;
    const float fac = 10.0f;
    const SimdVar<myfloat> r(fac);
    const int step = r.elements();
    const int step_2 = step * 2;
    Order order;
    for (int j=0; j < n; j+=step)
    {
        a.set(pdFourier, order.even);
        b.set(pdFourier, order.odd);
        a.hypot(a, b);
        b = a * r;
        b.get(&p[j]);
        pdFourier += step_2;
    }
}
#endif

void calcAmplitude(const std::complex<double>* pdFourier, std::vector<double>& aDest)
{
#if USE_SIMD != 0
    calcAmplitude(reinterpret_cast<const double*>(pdFourier), aDest, aDest.size());
#else
    for (unsigned int j=0; j<aDest.size(); ++j)
    {
        aDest[j] = abs(pdFourier[j]);
    }
#endif
}
#if 0
void calcAmplitudeF(const float*pdIn, std::vector<double>& aDest)
{
    const std::complex<float>*pdFourier = reinterpret_cast<const std::complex<float>* >(pdIn);
    for (unsigned int j=0; j<aDest.size(); ++j)
    {
        //p[j].y =(LONG)_hypot(pdFourier[j].real(), pdFourier[j].imag());
        aDest[j] = static_cast<double>(abs(pdFourier[j]));
    }
}
#endif

