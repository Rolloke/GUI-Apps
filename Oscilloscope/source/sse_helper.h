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
#ifdef __AVX__
class Float8
{
public:
  Float8 (__m256 v) : xmm (v) {}

  Float8 (float v=0.0f) { xmm = _mm256_set1_ps(v); }

  Float8 (float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8)
  {
      xmm = _mm256_set_ps(a1,a2,a3,a4,a5,a6,a7,a8);
  }

  Float8 (const float *v)
  {
      xmm = _mm256_load_ps(v);
  }

  Float8 operator* (const Float8 &v) const
  {
      return Float8(_mm256_mul_ps(xmm, v.xmm));
  }

  float & operator[] (std::uint32_t n)
  {
      return reinterpret_cast<float*>(&xmm)[n];
  }

  Float8 operator+ (const Float8 &v) const
  {
      return Float8(_mm256_add_ps(xmm, v.xmm));
  }

  Float8 operator- (const Float8 &v) const
  {
      return Float8(_mm256_sub_ps(xmm, v.xmm));
  }

  Float8 operator/ (const Float8 &v) const
  {
      return Float8(_mm256_div_ps(xmm, v.xmm));
  }

  void operator*= (const Float8 &v)
  {
      xmm = _mm256_mul_ps(xmm, v.xmm);
  }

  void operator+= (const Float8 &v)
  {
      xmm = _mm256_add_ps(xmm, v.xmm);
  }

  void operator-= (const Float8 &v)
  {
      xmm = _mm256_sub_ps(xmm, v.xmm);
  }

  void operator/= (const Float8 &v)
  {
      xmm = _mm256_div_ps(xmm, v.xmm);
  }

  void operator>> (float *v)
  {
      _mm256_store_ps(v, xmm);
  }

  void sqrt()
  {
      xmm = _mm256_sqrt_ps(xmm);
  }

private:
  __m256 xmm;
};

class Double4
{
public:
  Double4 (__m256d v) : xmm (v) {}

  Double4 (double v=0.0) { xmm = _mm256_set1_pd(v); }

  Double4 (double a1, double a2, double a3, double a4)
  {
      xmm = _mm256_set_pd(a1,a2,a3,a4);
  }

  Double4 (const double *v)
  {
      xmm = _mm256_load_pd(v);
  }

  Double4 operator* (const Double4 &v) const
  {
      return Double4(_mm256_mul_pd(xmm, v.xmm));
  }

  double & operator[] (std::uint32_t n)
  {
      return reinterpret_cast<double*>(&xmm)[n];
  }

  Double4 operator+ (const Double4 &v) const
  {
      return Double4(_mm256_add_pd(xmm, v.xmm));
  }

  Double4 operator- (const Double4 &v) const
  {
      return Double4(_mm256_sub_pd(xmm, v.xmm));
  }

  Double4 operator/ (const Double4 &v) const
  {
      return Double4(_mm256_div_pd(xmm, v.xmm));
  }

  void operator*= (const Double4 &v)
  {
      xmm = _mm256_mul_pd(xmm, v.xmm);
  }

  void operator+= (const Double4 &v)
  {
      xmm = _mm256_add_pd(xmm, v.xmm);
  }

  void operator-= (const Double4 &v)
  {
      xmm = _mm256_sub_pd(xmm, v.xmm);
  }

  void operator/= (const Double4 &v)
  {
      xmm = _mm256_div_pd(xmm, v.xmm);
  }

  void operator>> (double *v)
  {
      _mm256_store_pd(v, xmm);
  }

  void sqrt()
  {
      xmm = _mm256_sqrt_pd(xmm);
  }

private:
  __m256d xmm;
};
#endif

void calcAmplitude(const std::complex<double>* pdFourier, std::vector<double>& aDest);

void convertBufferFloat128bit(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer);

#ifdef __AVX__
void convertBufferFloat256bit(size_t aStartPos, size_t aSize, std::uint32_t aStartChannel, std::uint32_t aChannels, const Channel* aCP, const void *aInputBuffer, std::vector< std::vector<double> >&aOutputBuffer);
#endif
} // namespace sse

#endif // SSE_HELPER_H
