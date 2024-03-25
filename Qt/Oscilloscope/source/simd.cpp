#include "simd.h"


void limit(std::vector<int>& order_array, int the_limit)
{
    for (auto& index : order_array)
    {
        if (index >= the_limit)
        {
            index = the_limit-1;
        }
    }
}

#if __AVX2__ && USE_SIMD == AVX2

#endif

#if __AVX__ && USE_SIMD == AVX

#endif

#if __SSE3__  && USE_SIMD == SSE3

#endif

#if __SSE2__ && USE_SIMD == SSE2

#endif

#if __SSE__ && USE_SIMD == SSE

#endif
