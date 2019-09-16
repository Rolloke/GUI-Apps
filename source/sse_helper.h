#ifndef SSE_HELPER_H
#define SSE_HELPER_H

#include <vector>
#include <complex>
#include <immintrin.h>

class Channel;

namespace sse
{

class Float4
{
public:
  Float4 (__m128 v) : xmm (v) {}

  Float4 (float v=0.0f) { xmm = _mm_set1_ps(v); }

  Float4 (float x, float y, float z, float w)
  {
      xmm = _mm_set_ps(w,z,y,x);
  }

  Float4 (const float *v)
  {
      xmm = _mm_load_ps(v);
  }

  Float4 operator* (const Float4 &v) const
  {
      return Float4(_mm_mul_ps(xmm, v.xmm));
  }

  float & operator[] (std::uint32_t n)
  {
      return reinterpret_cast<float*>(&xmm)[n];
  }

  Float4 operator+ (const Float4 &v) const
  {
      return Float4(_mm_add_ps(xmm, v.xmm));
  }

  Float4 operator- (const Float4 &v) const
  {
      return Float4(_mm_sub_ps(xmm, v.xmm));
  }

  Float4 operator/ (const Float4 &v) const
  {
      return Float4(_mm_div_ps(xmm, v.xmm));
  }

  void operator*= (const Float4 &v)
  {
      xmm = _mm_mul_ps(xmm, v.xmm);
  }

  void operator+= (const Float4 &v)
  {
      xmm = _mm_add_ps(xmm, v.xmm);
  }

  void operator-= (const Float4 &v)
  {
      xmm = _mm_sub_ps(xmm, v.xmm);
  }

  void operator/= (const Float4 &v)
  {
      xmm = _mm_div_ps(xmm, v.xmm);
  }

  void operator>> (float *v)
  {
      _mm_store_ps(v, xmm);
  }

  void sqrt()
  {
      xmm = _mm_sqrt_ps(xmm);
  }

private:
  __m128 xmm;
};

class Double2
{
public:
  Double2 (__m128d v) : xmm (v) {}

  Double2 (double v=0.0) { xmm = _mm_set1_pd(v); }

  Double2 (double x, double y)
  {
      xmm = _mm_set_pd(y,x);
  }

  Double2 (const double *v)
  {
      xmm = _mm_load_pd(v);
  }

  Double2 operator* (const Double2 &v) const
  {
      return Double2(_mm_mul_pd(xmm, v.xmm));
  }

  double & operator[] (std::uint32_t n)
  {
      return reinterpret_cast<double*>(&xmm)[n];
  }

  Double2 operator+ (const Double2 &v) const
  {
      return Double2(_mm_add_pd(xmm, v.xmm));
  }

  Double2 operator- (const Double2 &v) const
  {
      return Double2(_mm_sub_pd(xmm, v.xmm));
  }

  Double2 operator/ (const Double2 &v) const
  {
      return Double2(_mm_div_pd(xmm, v.xmm));
  }

  void operator*= (const Double2 &v)
  {
      xmm = _mm_mul_pd(xmm, v.xmm);
  }

  void operator+= (const Double2 &v)
  {
      xmm = _mm_add_pd(xmm, v.xmm);
  }

  void operator-= (const Double2 &v)
  {
      xmm = _mm_sub_pd(xmm, v.xmm);
  }

  void operator/= (const Double2 &v)
  {
      xmm = _mm_div_pd(xmm, v.xmm);
  }

  void operator>> (double *v)
  {
      _mm_store_pd(v, xmm);
  }

  void sqrt()
  {
      xmm = _mm_sqrt_pd(xmm);
  }

private:
  __m128d xmm;
};
void calcAmplitude(const std::complex<double>* pdFourier, std::vector<double>& aDest);

void convertBufferFloat128bit(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer);

} // namespace sse

#endif // SSE_HELPER_H
