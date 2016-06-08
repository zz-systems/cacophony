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
#define TYPE_PREFIX "interpolation - avx2"
#elif defined(COMPILE_AVX1)
            using capability = capability_AVX1;
#define TYPE_PREFIX "interpolation - avx1"
#elif defined(COMPILE_SSE4FMA)
            using capability = capability_SSE4FMA;
    #define TYPE_PREFIX "interpolation - sse4 fma"
#elif defined(COMPILE_SSE4)
    using capability = capability_SSE4;
#define TYPE_PREFIX "interpolation - sse4"
#elif defined(COMPILE_SSSE3)
    using capability = capability_SSSE3;
    #define TYPE_PREFIX "interpolation - ssse3"
#elif defined(COMPILE_SSE3)
    using capability = capability_SSE3;
    #define TYPE_PREFIX "interpolation - sse3"
#elif defined(COMPILE_SSE2)
    using capability = capability_SSE2;
    #define TYPE_PREFIX "interpolation - sse2"
#else
    using vtest = capability_FPU;
    #define TYPE_PREFIX "interpolation - x87"
#endif

            using vreal = static_dispatcher<capability>::vreal;
            using vint = static_dispatcher<capability>::vint;

            using sint = int;
            using sreal = float;

            TEST_CASE(TYPE_PREFIX" cubic interpolation", "[interpolation]")
            {
                //AreEqual(X, cerp<float>(1, 2, 3, 4, 5));

                for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10
                for (float n3 = -2;		n3 < 2;		n3 += 0.25)			// 16
                for (float n2 = -50;	n2 < 50;	n2 += 15)			// 10
                for (float n1 = -25;	n1 < 25;	n1 += 25)			// 5
                for (float n0 = -1;		n0 < 1;		n0 += 0.25)			// 8
                {
                    gorynych::test::test<vreal, sreal>(
                            [=]() { return cerp<sreal>(n0, n1, n2, n3, a); },
                            [=]() { return cerp<vreal>(n0, n1, n2, n3, a); }
                    );
                }
            }

            TEST_CASE(TYPE_PREFIX" linear interpolation", "[interpolation]")
            {
                for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10
                for (float n1 = -25;	n1 < 25;	n1 += 10)			// 5
                for (float n0 = -1;	n0 < 1;		n0 += 0.25)			// 8
                {
                    gorynych::test::test<vreal, sreal>(
                            [=]() { return lerp<sreal>(n0, n1,  a); },
                            [=]() { return lerp<vreal>(n0, n1,  a); }
                    );
                }
            }

            TEST_CASE(TYPE_PREFIX" scurve3 interpolation", "[interpolation]")
            {
                for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10
                {
                    gorynych::test::test<vreal, sreal>(
                            [=]() { return scurve3<sreal>(a); },
                            [=]() { return scurve3<vreal>(a); }
                    );
                }
            }

            TEST_CASE(TYPE_PREFIX" scurve5 interpolation", "[interpolation]")
            {
                for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10
                {
                    gorynych::test::test<vreal, sreal>(
                            [=]() { return scurve5<sreal>(a); },
                            [=]() { return scurve5<vreal>(a); }
                    );
                }

            }
        }}}