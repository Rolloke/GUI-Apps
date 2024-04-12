#define SIMD512 3
#define SIMD256 2
#define SIMD128 1

/// TODO:
/// SSE3
/// SSSE3
/// SSE4.1
/// SSE4.2

#define USE_SIMD SIMD256

#ifndef USE_SIMD
#if __AVX512F__
#define USE_SIMD SIMD512
#elif __AVX2__ || __AVX__
#define USE_SIMD SIMD256
#elif __SSE__ || __SSE2__ || __SSE3__ || __SSE4_1__ || __SSE4_2__
#define USE_SIMD SIMD128
#endif
#endif

#include <type_traits>
#include <vector>
/// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
///
#ifndef SIMD_H
#define SIMD_H


void limit(std::vector<int>& order_array, int the_limit);

// AVX512_FP16

#if USE_SIMD == SIMD512

#include <immintrin.h>
struct Order
{
    std::vector<int> even = { 0, 2, 4, 6,  8, 10, 12, 14,  16, 18, 20, 22,  24, 26, 28, 30 };
    std::vector<int> odd  = { 1, 3, 5, 7,  9, 11, 13, 15,  17, 19, 21, 23,  25, 27, 29, 31 };
};

template <class type=float>
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

    SimdVar(type value)
    {
        set(value);
    }

    SimdVar(type* values)
    {
        set(values);
    }

    SimdVar(type* values, const int* order)
    {
        set(values, order);
    }

    template <class in_out_type>
    void set(in_out_type value)
    {
        if constexpr(std::is_same<type, float>::value)
        {
           m_var.s = _mm512_set1_ps(static_cast<type>(value));
        }
        else if constexpr(std::is_same<type, double>::value)
        {
           m_var.d = _mm512_set1_pd(static_cast<type>(value));
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
           m_var.i = _mm512_set1_epi32(static_cast<type>(value));
        }
    }

    template <class in_out_type>
    void set(const type* values, size_t limit = 0)
    {
        if (limit == 0)
        {
            if constexpr(std::is_same<type, float>::value)
            {
                if constexpr(std::is_same<in_out_type, float>::value)
                {
                    m_var.s = _mm512_load_ps(values);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, double>::value)
            {
                if constexpr(std::is_same<in_out_type, double>::value)
                {
                    m_var.d =  _mm512_load_pd(values);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, int32_t>::value)
            {
                if constexpr(std::is_same<in_out_type, int32_t>::value)
                {
                    m_var.i = _mm512_loadu_epi32(values);
                }
                else
                {
                    limit = elements();
                }
            }
        }
        if (limit)
        {
            for (size_t i=0; i<limit; ++i)
            {
                (*this)[i] = static_cast<type>(values[i]);
            }
        }
    }

    template <class in_out_type>
    void set(const in_out_type* values, const std::vector<int>& order)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_set_ps(
                    static_cast<type>(values[order[0]]), static_cast<type>(values[order[1]]), static_cast<type>(values[order[2]]), static_cast<type>(values[order[3]]),
                    static_cast<type>(values[order[4]]), static_cast<type>(values[order[5]]), static_cast<type>(values[order[6]]), static_cast<type>(values[order[7]]),
                    static_cast<type>(values[order[8]]), static_cast<type>(values[order[9]]), static_cast<type>(values[order[10]]), static_cast<type>(values[order[11]]),
                    static_cast<type>(values[order[12]]), static_cast<type>(values[order[13]]), static_cast<type>(values[order[14]]),static_cast<type>(values[order[15]]));
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_set_pd(
                    static_cast<type>(values[order[0]]), static_cast<type>(values[order[1]]), static_cast<type>(values[order[2]]), static_cast<type>(values[order[3]]),
                    static_cast<type>(values[order[4]]), static_cast<type>(values[order[5]]), static_cast<type>(values[order[6]]), static_cast<type>(values[order[7]]));
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_set_epi32(
                        static_cast<type>(values[order[0]]), static_cast<type>(values[order[1]]), static_cast<type>(values[order[2]]), static_cast<type>(values[order[3]]),
                        static_cast<type>(values[order[4]]), static_cast<type>(values[order[5]]), static_cast<type>(values[order[6]]), static_cast<type>(values[order[7]]),
                        static_cast<type>(values[order[8]]), static_cast<type>(values[order[9]]), static_cast<type>(values[order[10]]), static_cast<type>(values[order[11]]),
                        static_cast<type>(values[order[12]]), static_cast<type>(values[order[13]]), static_cast<type>(values[order[14]]), static_cast<type>(values[order[15]]));
        }
    }

    template <class in_out_type>
    void get(in_out_type* values, size_t limit = 0)
    {
        if (limit == 0)
        {
            if constexpr(std::is_same<type, float>::value)
            {
                if constexpr(std::is_same<in_out_type, float>::value)
                {
                    _mm512_store_ps(values, m_var.s);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, double>::value)
            {
                if constexpr(std::is_same<in_out_type, double>::value)
                {
                    _mm512_store_pd(values, m_var.d);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, int32_t>::value)
            {
                if constexpr(std::is_same<in_out_type, int32_t>::value)
                {
                    _mm512_storeu_epi32(values, m_var.i);
                }
                else
                {
                    limit = elements();
                }
            }
        }
        if (limit)
        {
            for (size_t i=0; i<limit; ++i)
            {
                values[i] = static_cast<in_out_type>((*this)[i]);
            }
        }
    }

    type & operator[](size_t __n)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            return reinterpret_cast<float*>(&m_var.s[0])[__n];
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            return reinterpret_cast<double*>(&m_var.s[0])[__n];
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
            m_var.s = _mm512_add_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_add_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_add_epi32(a.m_var.i,  b.m_var.i);
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
            m_var.s = _mm512_sub_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_sub_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_sub_epi32(a.m_var.i,  b.m_var.i);
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
            m_var.s = _mm512_mul_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_mul_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_mul_epi32(a.m_var.i,  b.m_var.i);
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
            m_var.s = _mm512_div_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_div_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.s = _mm512_div_(a.m_var.i,  b.m_var.i);
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
            m_var.s = _mm512_fmadd_ps(a.m_var.s, b.m_var.s, c.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_fmadd_pd(a.m_var.d, b.m_var.d, c.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm512_madd_(a.m_var.s, b.m_var.s, c.m_var.s);
        }
    }

    void invert(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_rcp_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_rcp_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
//            m_var.i = _mm512_rcp_ps(a.m_var.i);
        }
    }

    void sqrt(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_sqrt_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_sqrt_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
//            m_var.i = _mm512_sqrt_ps(a.m_var.i);
        }
    }

    void rsqrt(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_rsqrt14_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_rsqrt14_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm512_rsqrt_ps(a.m_var.i);
        }
    }

    void hypot(const SimdVar& a, const SimdVar& b)
    {
        SimdVar r;
        r.muladd(a, a, b*b);
        sqrt(r);
    }

    void min(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_min_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_min_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_min_epi32(a.m_var.i, b.m_var.i);
        }
    }

    void max(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_max_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_max_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_max_epi32(a.m_var.i, b.m_var.i);
        }
    }

    void round(const SimdVar& a, int scale = _MM_FROUND_TO_NEAREST_INT)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_roundscale_ps(a.m_var.s, scale);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_roundscale_pd(a.m_var.s, scale);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
//            m_var.i = _mm512_rcp_ps(a.m_var.i);
        }
    }

    void ceil(const SimdVar& a)
    {
        return round(a, _MM_FROUND_TO_POS_INF);
    }

    void floor(const SimdVar& a)
    {
        return round(a, _MM_FROUND_TO_NEG_INF);
    }

    void abs(const SimdVar& a, int scale = _MM_FROUND_TO_NEAREST_INT)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_abs_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_abs_pd(a.m_var.s);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_abs_epi32(a.m_var.i);
        }
    }

    void gt(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.m16 = _mm512_cmp_ps_mask(a.m_var.s, b.m_var.s, _CMP_GT_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.m8 = _mm512_cmp_pd_mask(a.m_var.s, b.m_var.s, _CMP_GT_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.m16 = _mm512_cmp_epi32_mask(a.m_var.i, b.m_var.i, _CMP_GT_OS);
        }
    }

    void ge(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.m16 = _mm512_cmp_ps_mask(a.m_var.s, b.m_var.s, _CMP_GE_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.m8 = _mm512_cmp_pd_mask(a.m_var.s, b.m_var.s, _CMP_GE_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.m16 = _mm512_cmp_epi32_mask(a.m_var.i, b.m_var.i, _CMP_GE_OS);
        }
    }

    void lt(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.m16 = _mm512_cmp_ps_mask(a.m_var.s, b.m_var.s, _CMP_LT_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.m8 = _mm512_cmp_pd_mask(a.m_var.s, b.m_var.s, _CMP_LT_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.m16 = _mm512_cmp_epi32_mask(a.m_var.i, b.m_var.i, _CMP_LT_OS);
        }
    }

    void le(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.m16 = _mm512_cmp_ps_mask(a.m_var.s, b.m_var.s, _CMP_LE_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.m8 = _mm512_cmp_pd_mask(a.m_var.s, b.m_var.s, _CMP_LE_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.m16 = _mm512_cmp_epi32_mask(a.m_var.i, b.m_var.i, _CMP_LE_OS);
        }
    }

    void eq(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.m16 = _mm512_cmp_ps_mask(a.m_var.s, b.m_var.s, _CMP_EQ_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.m8 = _mm512_cmp_pd_mask(a.m_var.s, b.m_var.s, _CMP_EQ_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.m16 = _mm512_cmp_epi32_mask(a.m_var.i, b.m_var.i, _CMP_EQ_OS);
        }
    }

    void neq(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.m16 = _mm512_cmp_ps_mask(a.m_var.s, b.m_var.s, _CMP_NEQ_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.m8 = _mm512_cmp_pd_mask(a.m_var.s, b.m_var.s, _CMP_NEQ_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.m16 = _mm512_cmp_epi32_mask(a.m_var.i, b.m_var.i, _CMP_NEQ_OS);
        }
    }


#if __AVX512DQ__
    void and_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_and_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_and_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_and_epi32(a.m_var.i, b.m_var.i);
        }
    }

    void or_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_or_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_or_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_or_epi32(a.m_var.i, b.m_var.i);
        }
    }

    void xor_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm512_xor_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm512_xor_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm512_xor_epi32(a.m_var.i, b.m_var.i);
        }
    }
#endif

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
        __m512  s;
        __m512d d;
        __m512i i;
        __mmask8 m8;
        __mmask16 m16;
    } m_var;

};
#endif

#if  USE_SIMD == SIMD256

#include <immintrin.h>
struct Order
{
    std::vector<int> even = { 0, 2, 4, 6,  8, 10, 12, 14 };
    std::vector<int> odd  = { 1, 3, 5, 7,  9, 11, 13, 15 };
};

template <class type=float>
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

    SimdVar(type value)
    {
        set(value);
    }

    SimdVar(type* values)
    {
        set(values);
    }

    SimdVar(type* values, const int* order)
    {
        set(values, order);
    }

    template <class in_out_type>
    void set(in_out_type value)
    {
        if constexpr(std::is_same<type, float>::value)
        {
           m_var.s = _mm256_set1_ps(static_cast<type>(value));
        }
        else if constexpr(std::is_same<type, double>::value)
        {
           m_var.d = _mm256_set1_pd(static_cast<type>(value));
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
           m_var.i = _mm256_set1_epi32(static_cast<type>(value));
        }
    }

    template <class in_out_type>
    void set(const type* values, size_t limit = 0)
    {
        if (limit == 0)
        {
            if constexpr(std::is_same<type, float>::value)
            {
                if constexpr(std::is_same<in_out_type, float>::value)
                {
                    m_var.s = _mm256_load_ps(values);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, double>::value)
            {
                if constexpr(std::is_same<in_out_type, double>::value)
                {
                    m_var.d =  _mm256_load_pd(values);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, int32_t>::value)
            {
                if constexpr(std::is_same<in_out_type, int32_t>::value)
                {
#if __AVX512F__ && __AVX512VL__
                    m_var.i = _mm256_loadu_epi32(values);
#else
                    limit = elements();
#endif
                }
                else
                {
                    limit = elements();
                }
            }
        }
        if (limit)
        {
            for (size_t i=0; i<limit; ++i)
            {
                (*this)[i] = static_cast<type>(values[i]);
            }
        }
    }

    template <class in_out_type>
    void set(const in_out_type* values, const std::vector<int>& order)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_set_ps(
                    static_cast<type>(values[order[0]]), static_cast<type>(values[order[1]]), static_cast<type>(values[order[2]]), static_cast<type>(values[order[3]]),
                    static_cast<type>(values[order[4]]), static_cast<type>(values[order[5]]), static_cast<type>(values[order[6]]), static_cast<type>(values[order[7]]));
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_set_pd(
                    static_cast<type>(values[order[0]]), static_cast<type>(values[order[1]]), static_cast<type>(values[order[2]]), static_cast<type>(values[order[3]]));
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_set_epi32(
                        static_cast<type>(values[order[0]]), static_cast<type>(values[order[1]]), static_cast<type>(values[order[2]]), static_cast<type>(values[order[3]]),
                        static_cast<type>(values[order[4]]), static_cast<type>(values[order[5]]), static_cast<type>(values[order[6]]), static_cast<type>(values[order[7]]));
        }
    }

    template <class in_out_type>
    void get(in_out_type* values, size_t limit = 0)
    {
        if (limit == 0)
        {
            if constexpr(std::is_same<type, float>::value)
            {
                if constexpr(std::is_same<in_out_type, float>::value)
                {
                    _mm256_store_ps(values, m_var.s);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, double>::value)
            {
                if constexpr(std::is_same<in_out_type, double>::value)
                {
                    _mm256_store_pd(values, m_var.d);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, int32_t>::value)
            {
                if constexpr(std::is_same<in_out_type, int32_t>::value)
                {
#if __AVX512F__ && __AVX512VL__
                    _mm256_storeu_epi32(values, m_var.i);
#else
                    limit = elements();
#endif
                }
            }
        }
        if (limit)
        {
            for (size_t i=0; i<limit; ++i)
            {
                values[i] = static_cast<in_out_type>((*this)[i]);
            }
        }
    }

    type & operator[](size_t __n)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            return reinterpret_cast<float*>(&m_var.s[0])[__n];
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            return reinterpret_cast<double*>(&m_var.s[0])[__n];
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
            m_var.s = _mm256_add_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_add_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX2__
            m_var.i = _mm256_add_epi32(a.m_var.i,  b.m_var.i);
#else
            //
#endif
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
            m_var.s = _mm256_sub_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_sub_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX2__
            m_var.i = _mm256_sub_epi32(a.m_var.i,  b.m_var.i);
#else
            //
#endif
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
            m_var.s = _mm256_mul_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_mul_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX2__
            m_var.i = _mm256_mul_epi32(a.m_var.i,  b.m_var.i);
#else
            //
#endif
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
            m_var.s = _mm256_div_ps(a.m_var.s,  b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_div_pd(a.m_var.d,  b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_div_epu32(a.m_var.i,  b.m_var.i);
        }
    }

    void operator/=(const SimdVar& a)
    {
        divide(*this, a);
    }

    void muladd(const SimdVar& a, const SimdVar& b, const SimdVar& c)
    {
#if __FMA__
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_fmadd_ps(a.m_var.s, b.m_var.s, c.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_fmadd_pd(a.m_var.d, b.m_var.d, c.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            add(a * b, c);
        }
#else
        add(a * b, c);
#endif
    }

    void invert(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_rcp_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_rcp14_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
//            m_var.i = _mm256_rcp_ps(a.m_var.i);
        }
    }

    void sqrt(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_sqrt_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_sqrt_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm256_sqrt_ps(a.m_var.i);
        }
    }

    void rsqrt(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_rsqrt_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_rsqrt14_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm256_rsqrt_ps(a.m_var.i);
        }
    }

    void hypot(const SimdVar& a, const SimdVar& b)
    {
#if __FMA__
        SimdVar r;
        r.muladd(a, a, b * b);
        sqrt(r);
#else
        sqrt(a * a + b * b);
#endif
    }

    void min(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_min_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_min_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX2__
            m_var.i = _mm256_min_epi32(a.m_var.i, b.m_var.i);
#else
#endif
        }
    }

    void max(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_max_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_max_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX2__
            m_var.i = _mm256_max_epi32(a.m_var.i, b.m_var.i);
#else
#endif
        }
    }

    void round(const SimdVar& a, int scale = (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC))
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_round_ps(a.m_var.s, scale);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_round_pd(a.m_var.d, scale);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
//            m_var.i = _mm256_rcp_ps(a.m_var.i);
        }
    }

    void ceil(const SimdVar& a)
    {
        return round(a, (_MM_FROUND_TO_POS_INF |_MM_FROUND_NO_EXC));
    }

    void floor(const SimdVar& a)
    {
        return round(a, (_MM_FROUND_TO_NEG_INF |_MM_FROUND_NO_EXC));
    }

    void abs(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            //m_var.s = _mm256_abs(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            //m_var.d = _mm256_(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX2__
            m_var.i = _mm256_abs_epi32(a.m_var.i);
#else

#endif
        }
    }

    void gt(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_cmp_ps(a.m_var.s, b.m_var.s, _CMP_GT_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_cmp_pd(a.m_var.d, b.m_var.d, _CMP_GT_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_cmpgt_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void ge(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_cmp_ps(a.m_var.s, b.m_var.s, _CMP_GE_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_cmp_pd(a.m_var.s, b.m_var.s, _CMP_GE_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.s = _mm256_cmpge_epi32_mask(a.m_var.s, b.m_var.s);
        }
    }

    void lt(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_cmp_ps(a.m_var.s, b.m_var.s, _CMP_LT_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_cmp_pd(a.m_var.s, b.m_var.s, _CMP_LT_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_cmplt_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void le(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_cmp_ps(a.m_var.s, b.m_var.s, _CMP_LE_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_cmp_pd(a.m_var.s, b.m_var.s, _CMP_LE_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_cmple_epi32_mask(a.m_var.s, b.m_var.s);
        }
    }

    void eq(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_cmp_ps(a.m_var.s, b.m_var.s, _CMP_EQ_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_cmp_pd(a.m_var.s, b.m_var.s, _CMP_EQ_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_cmpeq_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void neq(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_cmp_ps(a.m_var.s, b.m_var.s, _CMP_NEQ_OS);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_cmp_pd(a.m_var.s, b.m_var.s, _CMP_NEQ_OS);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_cmpneq_epi32_mask(a.m_var.i, b.m_var.i);
        }
    }

    void and_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_and_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_and_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm256_and_si256(a.m_var.i, b.m_var.i);
        }
    }

    void or_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_or_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_or_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX512F__ && __AVX512VL__
            m_var.i = _mm256_or_epi32(a.m_var.i, b.m_var.i);
#else
#endif
        }
    }

    void xor_(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm256_xor_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm256_xor_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
#if __AVX512F__ && __AVX512VL__
            m_var.i = _mm256_xor_epi32(a.m_var.i, b.m_var.i);
#else
#endif
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
        __m256  s;
        __m256d d;
        __m256i i;
    } m_var;

};

#endif



#if USE_SIMD == SIMD128

#include <emmintrin.h>
#include <immintrin.h>
struct Order
{
    std::vector<int> even = { 0, 2, 4, 6 };
    std::vector<int> odd  = { 1, 3, 5, 7 };
};

template <class type=float>
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

    SimdVar(type value)
    {
        set(value);
    }

    SimdVar(type* values)
    {
        set(values);
    }

    SimdVar(type* values, const int* order)
    {
        set(values, order);
    }

    template <class in_out_type>
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
#if __SSE2__
           m_var.i = _mm_set1_epi32(static_cast<type>(value));
#else

#endif
        }
    }

    template <class in_out_type>
    void set(const type* values, size_t limit = 0)
    {
        if (limit == 0)
        {
            if constexpr(std::is_same<type, float>::value)
            {
                if constexpr(std::is_same<in_out_type, float>::value)
                {
                    m_var.s = _mm_load1_ps(values);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, double>::value)
            {
                if constexpr(std::is_same<in_out_type, double>::value)
                {
                    m_var.d = _mm_load1_pd(values);
                }
                else
                {
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, int32_t>::value)
            {
                if constexpr(std::is_same<in_out_type, int32_t>::value)
                {
#if __SSE2__
                    m_var.i = _mm_loadu_si32(values);
#else
                    limit = elements();
#endif
                }
                else
                {
                    limit = elements();
                }
            }
        }
        if (limit)
        {
            for (size_t i=0; i<limit; ++i)
            {
                (*this)[i] = static_cast<type>(values[i]);
            }
        }
    }

    template <class in_out_type>
    void set(const in_out_type* values, const std::vector<int>& order)
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
#if __SSE2__
            m_var.i = _mm_set_epi32(
                    static_cast<type>(values[order[0]]),
                    static_cast<type>(values[order[1]]),
                    static_cast<type>(values[order[2]]),
                    static_cast<type>(values[order[3]]));
#else

#endif
        }
    }

    template <class in_out_type>
    void get(in_out_type* values, size_t limit = 0)
    {
        if (limit == 0)
        {
            if constexpr(std::is_same<type, float>::value)
            {
                if constexpr(std::is_same<in_out_type, float>::value)
                {
                    _mm_store1_ps(values, m_var.s);
                }
                else
                {
                    limit = elements();
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
                    limit = elements();
                }
            }
            else if constexpr(std::is_same<type, int32_t>::value)
            {
                if constexpr(std::is_same<in_out_type, int32_t>::value)
                {
#if __SSE2__
                    _mm_storeu_si32(values, m_var.i);
#else
                    limit = elements();
#endif
                }
                else
                {
                    limit = elements();
                }
            }
        }
        if (limit)
        {
            for (size_t i=0; i<limit; ++i)
            {
                values[i] = static_cast<in_out_type>((*this)[i]);
            }
        }
    }

    type & operator[](size_t __n)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            return reinterpret_cast<float*>(&m_var.s[0])[__n];
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
#if __SSE2__
            m_var.i = _mm_add_epi32(a.m_var.i,  b.m_var.i);
#else
#endif
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
#if __SSE2__
            m_var.i = _mm_sub_epi32(a.m_var.i,  b.m_var.i);
#else
#endif
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
#if __SSE4_1__
            m_var.i = _mm_mul_epi32(a.m_var.i,  b.m_var.i);
#else
#endif
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
        add(a * b, c);
    }

    void invert(const SimdVar& a)
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
        sqrt(a*a + b*b);
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

#if __SSE4_1__
    //! \brief round
    //! \param a
    //! \param rounding, see below, default  round to nearest
    //! (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC) // round to nearest, and suppress exceptions
    //! (_MM_FROUND_TO_NEG_INF |_MM_FROUND_NO_EXC)     // round down, and suppress exceptions
    //! (_MM_FROUND_TO_POS_INF |_MM_FROUND_NO_EXC)     // round up, and suppress exceptions
    //! (_MM_FROUND_TO_ZERO |_MM_FROUND_NO_EXC)        // truncate, and suppress exceptions
    //! _MM_FROUND_CUR_DIRECTION                       // use MXCSR.RC; see _MM_SET_ROUNDING_MODE
    void round(const SimdVar& a, int rounding = (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC))
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_round_ps(a.m_var.s, rounding);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_round_pd(a.m_var.d, rounding);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm_floor_pd(a.m_var.i);
        }
    }

    void ceil(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_ceil_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_ceil_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm_ceil_ps(a.m_var.i);
        }
    }

    void floor(const SimdVar& a)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_floor_ps(a.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_floor_pd(a.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm_floor_pd(a.m_var.i);
        }
    }
#endif

#if __SSE2__
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
#endif

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
#if __SSE2__
            m_var.i = _mm_and_si128(a.m_var.i, b.m_var.i);
#else
#endif
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
#if __SSE2__
            m_var.i = _mm_or_si128(a.m_var.i, b.m_var.i);
#else
#endif
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
#if __SSE2__
            m_var.i = _mm_xor_si128(a.m_var.i, b.m_var.i);
#else
#endif
        }
    }

#if __SSE3__
    //! \brief add_adjacent_pairs
    //! stores in self (r)
    //!  r[0] = a[0] + a[1]
    //!  r[1] = a[2] + a[3]
    //!  r[2] = b[0] + b[1]
    //!  r[3] = b[2] + b[3]
    //! \param a
    //! \param b
    void add_adjacent_pairs(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_hadd_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_hadd_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_hadd_epi32(a.m_var.i, b.m_var.i);
        }
    }

    //! \brief sub_adjacent_pairs
    //! stores in self (r)
    //!  r[0] = a[0] - a[1]
    //!  r[1] = a[2] - a[3]
    //!  r[2] = b[0] - b[1]
    //!  r[3] = b[2] - b[3]
    //! \param a
    //! \param b
    void sub_adjacent_pairs(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_hsub_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_hsub_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            m_var.i = _mm_hsub_epi32(a.m_var.i, b.m_var.i);
        }
    }

    //! \brief add_sub_adjacent_pairs
    //! stores in self (r)
    //!  r[0] = a[0] - a[1]
    //!  r[1] = a[2] + a[3]
    //!  r[2] = b[0] - b[1]
    //!  r[3] = b[2] + b[3]
    //! \param a
    //! \param b
    void add_sub_adjacent_pairs(const SimdVar& a, const SimdVar& b)
    {
        if constexpr(std::is_same<type, float>::value)
        {
            m_var.s = _mm_addsub_ps(a.m_var.s, b.m_var.s);
        }
        else if constexpr(std::is_same<type, double>::value)
        {
            m_var.d = _mm_addsub_pd(a.m_var.d, b.m_var.d);
        }
        else if constexpr(std::is_same<type, int32_t>::value)
        {
            //m_var.i = _mm_addsub_epi32(a.m_var.i, b.m_var.i);
        }
    }
#endif

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
