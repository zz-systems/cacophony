//---------------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------------


#pragma once

#include "zacc.hpp"
#include "system/branch.hpp"
#include "math/linear/generic_matrix.hpp"
#include "math/linear/specialized_matrix.hpp"

#include "vectortable.hpp"


namespace cacophony {

    using namespace zacc;
    using namespace zacc::math;

    struct generic_gather    {};
    struct optimized_gather {};


    template<typename T> struct bar { typedef void type; };
    template<> struct bar<float> { typedef bar<float> type; };
    template<> struct bar<double> { typedef bar<double> type; };

    template<typename GatherOptimization>
    struct random_vector_repository
    {};


    template<>
    struct random_vector_repository<generic_gather>//::template impl<generic_gather>
    {
        DISPATCHED struct impl
        {
            vec3<zfloat> at(const zint &index)
            {
                ZTRACE("noise generator", "gather randoms generic");
                return vec3<zfloat>(
                        zfloat::gather(zacc::make_raw(random_vectors.data()),		index),
                        zfloat::gather(zacc::make_raw(random_vectors.data() + 1),	index),
                        zfloat::gather(zacc::make_raw(random_vectors.data() + 2),	index)
                );
            }
        };
    };

    template<>
    struct random_vector_repository<optimized_gather>//::template impl<optimized_gather>
    {
        DISPATCHED struct impl
        {
#if defined(ZACC_SCALAR)
            // Generic case: use zacc gather functions.
            template<typename T = vec3<zfloat>>
            static
            std::enable_if_t<zfloat::dim == 1, vec3<zfloat>>
            at(const zint &index)
            {
                ZTRACE("noise generator", "gather randoms SCALAR");
                return {
                    random_vectors[index.value()],
                    random_vectors[index.value() + 1],
                    random_vectors[index.value() + 2]
                };
            }
#endif

#if defined(ZACC_SSE)

            // For SSE case: No need to gather. Load 4 vectors and transpose them - omitting the last (0 0 0 0) row
            template<typename T = vec3<zfloat>>
            static
            std::enable_if_t<zfloat::dim == 4, vec3<zfloat>>
            at(const zint &index)
            {
                ZTRACE("noise generator", "gather randoms SSE");

                auto extracted = index.data();
                //_mm_prefetch
                auto a0 = random_vectors.data() + extracted[0];
                auto a1 = random_vectors.data() + extracted[1];
                auto a2 = random_vectors.data() + extracted[2];
                auto a3 = random_vectors.data() + extracted[3];

                //auto vvi = extract(index);
                //_mm_prefetch()
                auto rv0 = _mm_loadu_ps(a0);
                auto rv1 = _mm_loadu_ps(a1);
                auto rv2 = _mm_loadu_ps(a2);
                auto rv3 = _mm_loadu_ps(a3);

                //_MM_TRANSPOSE4_PS(rv0, rv1, rv2, rv3)

                __m128 _Tmp3, _Tmp2, _Tmp1, _Tmp0;

                _Tmp0 = _mm_shuffle_ps((rv0), (rv1), 0x44);
                _Tmp2 = _mm_shuffle_ps((rv0), (rv1), 0xEE);
                _Tmp1 = _mm_shuffle_ps((rv2), (rv3), 0x44);
                _Tmp3 = _mm_shuffle_ps((rv2), (rv3), 0xEE);

                return {
                        _mm_shuffle_ps(_Tmp0, _Tmp1, 0x88), //rv0,
                        _mm_shuffle_ps(_Tmp0, _Tmp1, 0xDD), //rv1,
                        _mm_shuffle_ps(_Tmp2, _Tmp3, 0x88) //rv2,
                };
            }

#endif

#if defined(ZACC_AVX) || defined(ZACC_AVX2)
            // For AVX case: Load twice 4 vectors and transpose them - omitting the last (0 0 0 0) row
                template<typename T = vec3<zfloat>>
                static
                std::enable_if_t<zfloat::dim == 8, vec3<zfloat>>
                at(const zint &index)
                {
                    ZTRACE("noise generator", "gather randoms AVX");

                    auto extracted = index.data();
                    //_mm_prefetch
                    auto a0 = random_vectors.data() + extracted[0];
                    auto a1 = random_vectors.data() + extracted[1];
                    auto a2 = random_vectors.data() + extracted[2];
                    auto a3 = random_vectors.data() + extracted[3];
                    auto a4 = random_vectors.data() + extracted[4];
                    auto a5 = random_vectors.data() + extracted[5];
                    auto a6 = random_vectors.data() + extracted[6];
                    auto a7 = random_vectors.data() + extracted[7];

                    ZTRACE("noise generator", "gather randoms AVX, index prepared");

                    auto rv0 = _mm_loadu_ps(a0);
                    auto rv1 = _mm_loadu_ps(a1);
                    auto rv2 = _mm_loadu_ps(a2);
                    auto rv3 = _mm_loadu_ps(a3);

                    ZTRACE("noise generator", "gather randoms AVX, low data loaded");

                    auto rv4 = _mm_loadu_ps(a4);
                    auto rv5 = _mm_loadu_ps(a5);
                    auto rv6 = _mm_loadu_ps(a6);
                    auto rv7 = _mm_loadu_ps(a7);

                    ZTRACE("noise generator", "gather randoms AVX, high data loaded");

                    //_MM_TRANSPOSE4_PS(rv0, rv1, rv2, rv3)

                    __m128 _Tmp7, _Tmp6, _Tmp5, _Tmp4, _Tmp3, _Tmp2, _Tmp1, _Tmp0;

                    _Tmp0 = _mm_shuffle_ps((rv0), (rv1), 0x44);
                    _Tmp2 = _mm_shuffle_ps((rv0), (rv1), 0xEE);
                    _Tmp1 = _mm_shuffle_ps((rv2), (rv3), 0x44);
                    _Tmp3 = _mm_shuffle_ps((rv2), (rv3), 0xEE);

                    auto x_lo = _mm_shuffle_ps(_Tmp0, _Tmp1, 0x88);
                    auto y_lo = _mm_shuffle_ps(_Tmp0, _Tmp1, 0xDD);
                    auto z_lo = _mm_shuffle_ps(_Tmp2, _Tmp3, 0x88);

                    ZTRACE("noise generator", "gather randoms AVX, low data shuffled");

                    _Tmp4 = _mm_shuffle_ps((rv4), (rv5), 0x44);
                    _Tmp6 = _mm_shuffle_ps((rv4), (rv5), 0xEE);
                    _Tmp5 = _mm_shuffle_ps((rv6), (rv7), 0x44);
                    _Tmp7 = _mm_shuffle_ps((rv6), (rv7), 0xEE);

                    auto x_hi = _mm_shuffle_ps(_Tmp4, _Tmp5, 0x88);
                    auto y_hi = _mm_shuffle_ps(_Tmp4, _Tmp5, 0xDD);
                    auto z_hi = _mm_shuffle_ps(_Tmp6, _Tmp7, 0x88);

                    ZTRACE("noise generator", "gather randoms AVX, high data shuffled");

                    return {
                        _mm256_set_m128(x_hi, x_lo),
                        _mm256_set_m128(y_hi, y_lo),
                        _mm256_set_m128(z_hi, z_lo)
                    };
                }
#endif
        };
    };
}