#define AVX2 5
#define AVX  4
#define SSE3 3
#define SSE2 2
#define SSE  1

#define USE_SIMD SSE2

#ifndef USE_SIMD
#if __AVX2__
#define USE_SIMD AVX2
#elif __AVX__
#define USE_SIMD AVX
#elif __SSE3__
#define USE_SIMD SSE3
#elif __SSE2__
#define USE_SIMD SSE2
#else
#define USE_SIMD SSE
#endif
#endif

/// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
///
#ifndef SIMD_H
#define SIMD_H

#if __AVX2__ && USE_SIMD == AVX2


#include <immintrin.h>
template <class type>
class SimdVar
{
public:
    static constexpr int even[] = { 0, 2, 4, 6 };
    static constexpr int odd[]  = { 1, 3, 5, 7 };

    SimdVar(bool set_zero=false)
    {
        if (set_zero)
        {
            set(static_cast<type>(0));
        }
    }

    SimdVar(type value)
    {
        set(value);
    }

    SimdVar(type* values)
    {
        set(values);
    }

    SimdVar(type *values, const int *order)
    {
        set(values, order);
    }

    void set(type value)
    {
        m_var = _mm512_set1_pd(value);
    }

    void set(type* values)
    {
        m_var = _mm_load1_ps(values);
    }

    void set(type* values, const int* order)
    {
        m_var = _mm_set_ps(values[order[0]], values[order[1]], values[order[2]], values[order[3]]);
    }

    type & operator[](size_t __n)
    {
        return m_var[__n];
    }

    size_t elements() const
    {
        return sizeof(m_var) / sizeof(type);
    }

    static void add(const SimdVar& a, const SimdVar& b, SimdVar& r)
    {
        r.m_var = _mm512_add_pd(a.m_var,  b.m_var);
    }

    void operator+=(const SimdVar& a)
    {
        SimdVar& r =*this;
        add(a, r, r);
    }

    static void subtract(const SimdVar& a, const SimdVar& b, SimdVar& r)
    {
        r.m_var = _mm512_sub_pd(a.m_var,  b.m_var);
    }

    void operator-=(const SimdVar& a)
    {
        SimdVar& r =*this;
        subtract(a, r, r);
    }

    static void multiply(const SimdVar& a, const SimdVar& b, SimdVar& r)
    {
        r.m_var = _mm512_mul_pd(a.m_var,  b.m_var);
    }

    void operator*=(const SimdVar& a)
    {
        SimdVar& r =*this;
        multiply(a, r, r);
    }

    static void divide(const SimdVar& a, const SimdVar& b, SimdVar& r)
    {
        r.m_var = _mm512_div_pd(a.m_var,  b.m_var);
    }

    void operator/=(const SimdVar& a)
    {
        SimdVar& r =*this;
        divide(a, r, r);
    }

    static inline void sqrt(const SimdVar<type>& a, SimdVar<type>& r)
    {
        r.m_var = _mm512_sqrt_pd(a.m_var);
    }


/// friends
    friend SimdVar<type> operator+(const SimdVar<type>& a, const SimdVar<type>& b)
    {
        SimdVar<type> r;
        add(a, b, r);
        return r;
    }

    friend SimdVar operator-(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        subtract(a, b, r);
        return r;
    }

    friend SimdVar operator*(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        multiply(a, b, r);
        return r;
    }

    friend SimdVar operator/(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        divide(a, b, r);
        return r;
    }

private:
    __m512d m_var;
};

#endif

#if __AVX__ && USE_SIMD == AVX

#endif

#if __SSE3__  && USE_SIMD == SSE3

#endif

#if __SSE2__ && USE_SIMD == SSE2

#include <emmintrin.h>
#include <immintrin.h>

template <class type=float, class in_out_type=type>
class SimdVar
{
public:
    static constexpr int even[] = { 0, 2, 4, 6 };
    static constexpr int odd[]  = { 1, 3, 5, 7 };
    SimdVar(bool set_zero = false)
    {
        if (set_zero)
        {
            set(static_cast<type>(0));
        }
    }

    SimdVar(in_out_type value)
    {
        set(value);
    }

    SimdVar(in_out_type* values)
    {
        set(values);
    }

    SimdVar(in_out_type* values, const int* order)
    {
        set(values, order);
    }

    void set(in_out_type value)
    {
        m_var = _mm_set1_ps(value);
    }

    void set(const in_out_type* values)
    {
        m_var = _mm_load1_ps(values);
    }

    void set(const in_out_type* values, const int* order)
    {
        m_var = _mm_set_ps(
                static_cast<type>(values[order[0]]),
                static_cast<type>(values[order[1]]),
                static_cast<type>(values[order[2]]),
                static_cast<type>(values[order[3]]));
    }

    void get(in_out_type* values)
    {
//        __m128 _mm_castpd_ps (__m128d a)
//        __m128i _mm_castpd_si128 (__m128d a)
//        __m128d _mm_castps_pd (__m128 a)
//        __m128i _mm_castps_si128 (__m128 a)
//        __m128d _mm_castsi128_pd (__m128i a)
//        __m128 _mm_castsi128_ps (__m128i a)
        _mm_store1_ps(values, m_var);
    }

    type & operator[](size_t __n)
    {
        return m_var[__n];
    }

    size_t elements() const
    {
        return sizeof(m_var) / sizeof(type);
    }


    void add(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_add_ps(a.m_var,  b.m_var);
    }

    void operator+=(const SimdVar& a)
    {
        SimdVar& r =*this;
        r.add(r, a);
    }

    void subtract(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_sub_ps(a.m_var,  b.m_var);
    }

    void operator-=(const SimdVar& a)
    {
        SimdVar& r =*this;
        r.subtract(r, a);
    }

    void multiply(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_mul_ps(a.m_var,  b.m_var);
    }

    void operator*=(const SimdVar& a)
    {
        SimdVar& r =*this;
        r.multiply(r, a);
    }

    void divide(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_div_ps(a.m_var,  b.m_var);
    }

    void operator/=(const SimdVar& a)
    {
        SimdVar& r =*this;
        r.divide(r, a);
    }

    void muladd(const SimdVar& a, const SimdVar& b, const SimdVar& c)
    {
        m_var = _mm_fmadd_ps(a.m_var, b.m_var, c.m_var);
    }

    void rcp(const SimdVar& a)
    {
        m_var = _mm_rcp_ps(a.m_var);
    }

    void sqrt(const SimdVar& a)
    {
        m_var = _mm_sqrt_ps(a.m_var);
    }

    void rsqrt(const SimdVar& a)
    {
        m_var = _mm_rsqrt_ps(a.m_var);
    }

    void hypot(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        r.muladd(a, a, b*b);
        sqrt(r);
    }

    void gt(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_cmpgt_ps(a, b);
    }

    void ge(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_cmpge_ps(a, b);
    }

    void lt(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_cmplt_ps(a, b);
    }

    void le(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_cmple_ps(a, b);
    }

    void eq(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_cmpeq_ps(a, b);
    }

    void neq(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_cmpneq_ps(a, b);
    }

    void and_(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_and_ps(a, b);
    }

    void or_(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_or_ps(a, b);
    }

    void xor_(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_xor_ps(a, b);
    }

    void min(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_min_ps(a, b);
    }

    void max(const SimdVar& a, const SimdVar& b)
    {
        m_var = _mm_max_ps(a, b);
    }

/// friends
    friend SimdVar operator+(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        r.add(a, b);
        return r;
    }

    friend SimdVar operator-(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        r.subtract(a, b);
        return r;
    }

    friend SimdVar operator*(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        r.multiply(a, b);
        return r;
    }

    friend SimdVar operator/(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        r.divide(a, b);
        return r;
    }

private:
    __m128 m_var;
};

#endif

#if __SSE__ && USE_SIMD == SSE

#endif

#endif // SIMD_H
