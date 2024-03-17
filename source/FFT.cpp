
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

#if __SSE2__ && USE_SIMD
__attribute__((noinline))
void calcAmplitude(const double*pdFourier, std::vector<double>&p, int n)
{
#if 0
    int j, k;
    __m128 a, b;
    static const float fac = 10.0f;
    static const __m128 r = { fac, fac, fac, fac };
    for (k=-1, j=-1; j < n; )
    {
        a[0] = pdFourier[++k];
        b[0] = pdFourier[++k];
        a[1] = pdFourier[++k];
        b[1] = pdFourier[++k];
        a[2] = pdFourier[++k];
        b[2] = pdFourier[++k];
        a[3] = pdFourier[++k];
        b[3] = pdFourier[++k];
        a = _mm_mul_ps(a, a);
        b = _mm_mul_ps(b, b);
        b = _mm_add_ps(a, b);
        a = _mm_sqrt_ps(b);
        b = _mm_mul_ps(a, r);
        p[++j] = (double)(float)b[0];
        p[++j] = (double)(float)b[1];
        p[++j] = (double)(float)b[2];
        p[++j] = (double)(float)b[3];
    }
#else

//     __m512 m_var = _mm512_set1_ps(1.0f);

    typedef float myfloat;
    SimdVar<myfloat, double> a, b;
    const float fac = 10.0f;
    const SimdVar<myfloat, double> r(fac);
    const int step = r.elements();
    const int step_2 = step * 2;
    for (int j=0; j < n; j+=step)
    {
        a.set(pdFourier, SimdVar<myfloat, double>::even);
        b.set(pdFourier, SimdVar<myfloat, double>::odd);

        a.hypot(a, b);
        b = a * r;

        p[j  ] = static_cast<double>(b[0]);
        p[j+1] = static_cast<double>(b[1]);
        p[j+2] = static_cast<double>(b[2]);
        p[j+3] = static_cast<double>(b[3]);

        pdFourier += step_2;
    }
#endif
}
#endif

void calcAmplitude(const std::complex<double>* pdFourier, std::vector<double>& aDest)
{
#if __SSE2__ && USE_SIMD
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

