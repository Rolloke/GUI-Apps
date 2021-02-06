
#include "FFT.h"
#include "main.h"

#include <cmath>


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

#if 1

void calcAmplitude(const std::complex<double>* pdFourier, std::vector<double>& aDest)
{
    for (unsigned int j=0; j<aDest.size(); ++j)
    {
        aDest[j] = abs(pdFourier[j]);
    }
}

#else

void calcAmplitudeF(const float*pdIn, std::vector<double>& aDest)
{
    const complex<float>*pdFourier = reinterpret_cast<const complex<float>* >(pdIn);
    for (unsigned int j=0; j<aDest.size(); ++j)
    {
        //p[j].y =(LONG)_hypot(pdFourier[j].real(), pdFourier[j].imag());
        aDest[j] = static_cast<LONG>(abs(pdFourier[j]));
    }
}

void calcAmplitude(const float*pdFourier, std::vector<double>&p, int n)
{
    int j, k;
    __m128 a, b;
    static const __m128 r = { 0.5f, 0.5f, 0.5f, 0.5f };
    for (k=-1, j=-1; j < n; )
    {
        a.m128_f32[0] = pdFourier[++k];
        b.m128_f32[0] = pdFourier[++k];
        a.m128_f32[1] = pdFourier[++k];
        b.m128_f32[1] = pdFourier[++k];
        a.m128_f32[2] = pdFourier[++k];
        b.m128_f32[2] = pdFourier[++k];
        a.m128_f32[3] = pdFourier[++k];
        b.m128_f32[3] = pdFourier[++k];
        a = _mm_mul_ps(a, a);
        b = _mm_mul_ps(b, b);
        b = _mm_add_ps(a, b);
        a = _mm_sqrt_ps(b);
        b = _mm_add_ps(a, r);
        p[++j] = (LONG)(float)b.m128_f32[0];
        p[++j] = (LONG)(float)b.m128_f32[1];
        p[++j] = (LONG)(float)b.m128_f32[2];
        p[++j] = (LONG)(float)b.m128_f32[3];
    }
}

#endif
