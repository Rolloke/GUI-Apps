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

#include <type_traits>
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

    constexpr size_t elements() const
    {
        return  sizeof(m_var) / sizeof(type);
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
struct order
{
    static constexpr int even[] = { 0, 2, 4, 6 };
    static constexpr int odd[]  = { 1, 3, 5, 7 };
};

template <class type=float, class in_out_type=type>
class SimdVar
{
public:
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
	    if constexpr(std::is_same<type, float>::value)
        {
           m_var.s = _mm_set1_ps(static_cast<type>(value));
        }
        else if constexpr(std::is_same<type, double>::value)
        {
           m_var.d = _mm_set1_pd(static_cast<type>(value));
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
           m_var.i = _mm_set1_epi32(static_cast<type>(value));
        }
    }

    void set(const type* values)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_load1_ps(values);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_load1_pd(values);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.d = _mm_loadu_si32(values);
        }
    }

    void set(const in_out_type* values, const int32_t* order)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_set_ps(
                    static_cast<type>(values[order[0]]),
                    static_cast<type>(values[order[1]]),
                    static_cast<type>(values[order[2]]),
                    static_cast<type>(values[order[3]]));
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_set_pd(
                    static_cast<type>(values[order[0]]),
                    static_cast<type>(values[order[1]]));
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_set_epi32(
                    static_cast<type>(values[order[0]]),
                    static_cast<type>(values[order[1]]),
                    static_cast<type>(values[order[2]]),
                    static_cast<type>(values[order[3]]));
        }
    }

    void get(in_out_type* values)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            if constexpr(std::is_same<in_out_type, float>::value)
            {
                 _mm_store1_ps(values, m_var.s);
            }
            else
            {
                for (size_t i=0; i<elements(); ++i)
                {
                    values[i] = static_cast<in_out_type>((*this)[i]);
                }
            }

        }
        else if constexpr(std::is_same<type, double>::value)
        {
            if constexpr(std::is_same<in_out_type, double>::value)
            {
                _mm_store1_pd(values, m_var.d);
            }
            else
            {
                for (size_t i=0; i<elements(); ++i)
                {
                    values[i] = static_cast<in_out_type>((*this)[i]);
                }
            }
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            if constexpr(std::is_same<in_out_type, int32_t>::value)
            {
                _mm_storeu_si32(values, m_var.i);
            }
            else
            {
                for (size_t i=0; i<elements(); ++i)
                {
                    values[i] = static_cast<in_out_type>((*this)[i]);
                }
            }
        }
    }

    constexpr type & operator[](size_t __n)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            return m_var.s[__n];
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            return m_var.d[__n];
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            return reinterpret_cast<int32_t*>(&m_var.i[0])[__n];
        }
    }

    constexpr size_t elements() const
    {
        if constexpr(std::is_same<type, float>::value)
        {
            return sizeof(m_var.s) / sizeof(type);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            return sizeof(m_var.d) / sizeof(type);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            return sizeof(m_var.i) / sizeof(type);
        }
    }


    void add(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_add_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_add_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_add_epi32(a.m_var.i,  b.m_var.i);
        }
    }

    void operator+=(const SimdVar& a)
    {
        add(*this, a);
    }

    void subtract(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_sub_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_sub_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_sub_epi32(a.m_var.i,  b.m_var.i);
        }
    }

    void operator-=(const SimdVar& a)
    {
        subtract(*this, a);
    }

    void multiply(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_mul_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_mul_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_mul_epi32(a.m_var.i,  b.m_var.i);
        }
    }

    void operator*=(const SimdVar& a)
    {
        multiply(*this, a);
    }

    void divide(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_div_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_div_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.s = _mm_div_(a.m_var.i,  b.m_var.i);
        }
    }

    void operator/=(const SimdVar& a)
    {
        divide(*this, a);
    }

    void muladd(const SimdVar& a, const SimdVar& b, const SimdVar& c)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_fmadd_ps(a.m_var.s, b.m_var.s, c.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_fmadd_pd(a.m_var.d, b.m_var.d, c.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm_fmadd_ps(a.m_var.s, b.m_var.s, c.m_var.s);
        }
    }

    void rcp(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_rcp_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
//            m_var.d = _mm_rcp_(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
//            m_var.i = _mm_rcp_ps(a.m_var.i);
        }
    }

    void sqrt(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_sqrt_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_sqrt_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
//            m_var.i = _mm_sqrt_ps(a.m_var.i);
        }
    }

    void rsqrt(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_rsqrt_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            //m_var.d = _mm_rsqrt_ps(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm_rsqrt_ps(a.m_var.i);
        }
    }

    void hypot(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        r.muladd(a, a, b*b);
        sqrt(r);
    }

    void gt(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_cmpgt_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_cmpgt_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_cmpgt_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void ge(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_cmpge_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_cmpge_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.s = _mm_cmpge_epi32_mask(a.m_var.s, b.m_var.s);
        }
    }

    void lt(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_cmplt_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_cmplt_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_cmplt_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void le(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_cmple_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_cmple_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_cmple_epi32_mask(a.m_var.s, b.m_var.s);
        }
    }

    void eq(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_cmpeq_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_cmpeq_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_cmpeq_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void neq(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_cmpneq_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_cmpneq_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_cmpneq_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void and_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_and_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_and_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_and_si128(a.m_var.i, b.m_var.i);
        }
    }

    void or_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_or_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_or_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_or_si128(a.m_var.i, b.m_var.i);
        }
    }

    void xor_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_xor_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_xor_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_xor_si128(a.m_var.i, b.m_var.i);
        }
    }

    void min(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_min_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_min_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_min_epi32(a.m_var.i, b.m_var.i);
        }
    }

    void max(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_max_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_max_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_max_epi32(a.m_var.i, b.m_var.i);
        }
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
    union var
    {
        __m128  s;
        __m128d d;
        __m128i i;
    } m_var;

};

#endif

#if __SSE__ && USE_SIMD == SSE

#endif

#endif // SIMD_H
