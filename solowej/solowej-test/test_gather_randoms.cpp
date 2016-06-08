//
// Created by szuyev on 19.05.16.
//

#include "../Catch/single_include/catch.hpp"
#include "../gorynych/gorynych/gorynych.h"
#include "../gorynych/gorynych/unit_test_ext.h"
#include "../solowej/noise/noisegenerators.h"


namespace zzsystems { namespace solowej { namespace test {
            using namespace gorynych;
            using namespace gorynych::test;


#if defined(COMPILE_AVX2)
            using capability = capability_AVX2;
#define TYPE_PREFIX "gather randoms - avx2"
#elif defined(COMPILE_AVX1)
            using capability = capability_AVX1;
#define TYPE_PREFIX "gather randoms - avx1"
#elif defined(COMPILE_SSE4FMA)
            using capability = capability_SSE4FMA;
    #define TYPE_PREFIX "gather randoms - sse4 fma"
#elif defined(COMPILE_SSE4)
            using capability = capability_SSE4;
#define TYPE_PREFIX "gather randoms - sse4"
#elif defined(COMPILE_SSSE3)
            using capability = capability_SSSE3;
    #define TYPE_PREFIX "gather randoms - ssse3"
#elif defined(COMPILE_SSE3)
            using capability = capability_SSE3;
    #define TYPE_PREFIX "gather randoms - sse3"
#elif defined(COMPILE_SSE2)
            using capability = capability_SSE2;
    #define TYPE_PREFIX "gather randoms - sse2"
#else
            using vtest = capability_FPU;
#define TYPE_PREFIX "gather randoms - x87"
#endif

            using vreal = static_dispatcher<capability>::vreal;
            using vint = static_dispatcher<capability>::vint;

            using sint = int;
            using sreal = float;

            TEST_CASE(TYPE_PREFIX" gather", "[gather]")
            {
                for(int i = 0; i < random_vectors<float>::size / dim<vreal>(); i++)
                {
                    int indices[dim<vreal>()];
                    for(size_t d = 0; d < dim<vreal>(); d++)
                    {
                        indices[d] = i + d;
                    }

                    auto gathered = vgather(random_vectors<float>::values, vint(indices));
                    float tested[dim<vreal>()];
                    extract(gathered, tested);

                    for(int d = 0; d < dim<vreal>(); d++)
                    {
                        REQUIRE(random_vectors<float>::values[i + d] == tested[d]);
                    }
                }
            }

            TEST_CASE(TYPE_PREFIX" gather randoms", "[gather]")
            {
                for(int i = 0; i < random_vectors<float>::size / dim<vreal>(); i++)
                {
                    int indices[dim<vreal>()];
                    for(size_t d = 0; d < dim<vreal>(); d++)
                    {
                        indices[d] = i + d;
                    }

                    auto ii = vint(indices);
                    auto gathered = noisegen<SIMD_T>::gather_randoms(ii);

                    float testedx[dim<vreal>()], testedy[dim<vreal>()], testedz[dim<vreal>()];
                    extract(gathered.x, testedx);
                    extract(gathered.y, testedy);
                    extract(gathered.z, testedz);

                    for(int d = 0; d < dim<vreal>(); d++)
                    {
                        REQUIRE(random_vectors<float>::values[i + d] == testedx[d]);
                        REQUIRE(random_vectors<float>::values[i + d + 1] == testedy[d]);
                        REQUIRE(random_vectors<float>::values[i + d + 2] == testedz[d]);
                    }
                }
            }
        }}}