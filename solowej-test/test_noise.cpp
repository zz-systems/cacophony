//
// Vectorized and parallelized version of libnoise using the gorynych SIMD toolkit
// Copyright (C) 2015-2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Original libnoise:
// Copyright (C) 2003, 2004 Jason Bevins
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "../dependencies/Catch/single_include/catch.hpp"
#include "../dependencies/gorynych/gorynych/gorynych.h"
#include "../dependencies/gorynych/gorynych-test/test_extensions.h"

#include "../solowej/noise/noisegenerators.h"

namespace zzsystems { namespace solowej { namespace  test {
            using namespace gorynych;
            using namespace gorynych::test;

#define TYPE_PREFIX TEST_PREFIX("noise")

    VECTORIZED class noisegen_wrapper : public noisegen<SIMD_T>
    {
        //friend class TestNoise;
    };

    TEST_CASE(TYPE_PREFIX" gradient coherent 3d", "[noise]")
    {
            for (auto q = 0; q <= 2; q++)
            for (float z = -1; z <= 1; z += 0.25)
            for (float y = -1; y <= 1; y += 0.25)
            for (float x = -1; x <= 1; x += 0.25)
            {
                gorynych::test::test<vreal, sreal>(
                        [=]() { return noisegen_wrapper<sreal, sint>::gradient_coherent_3d({ x, y, z }, 1010, Quality(q)); },
                        [=]() { return noisegen_wrapper<vreal, vint>::gradient_coherent_3d({ x, y, z }, 1010, Quality(q)); }
                );
            }
    }

    TEST_CASE(TYPE_PREFIX" gradient 3d", "[noise]")
    {
            for (int zi = -2; zi <= 2; zi += 1)
            for (int yi = -2; yi <= 2; yi += 1)
            for (int xi = -2; xi <= 2; xi += 1)
            for (float z = -1; z <= 1; z += 0.25)
            for (float y = -1; y <= 1; y += 0.25)
            for (float x = -1; x <= 1; x += 0.25)
            {
                gorynych::test::test<vreal, sreal>(
                        [=]() { return noisegen_wrapper<sreal, sint>::gradient_3d({ x, y, z }, { xi, zi, yi }, 1010);},
                        [=]() { return noisegen_wrapper<vreal, vint>::gradient_3d({ x, y, z }, { xi, zi, yi }, 1010);}
                );
            }
    }

    TEST_CASE(TYPE_PREFIX" value coherent 3d", "[noise]")
    {
        for (auto q = 0; q <= 2; q++)
        for (float z = -1; z <= 1; z += 0.25)
        for (float y = -1; y <= 1; y += 0.25)
        for (float x = -1; x <= 1; x += 0.25)
        {
            gorynych::test::test<vreal, sreal>(
                    [=]() { return noisegen_wrapper<sreal, sint>::value_coherent_3d({ x, y, z }, 1010, Quality(q));},
                    [=]() { return noisegen_wrapper<vreal, vint>::value_coherent_3d({ x, y, z }, 1010, Quality(q));}
            );
        }
    }

    TEST_CASE(TYPE_PREFIX" real value 3d", "[noise]")
    {
        for (int zi = -2; zi <= 2; zi += 1)
        for (int yi = -2; yi <= 2; yi += 1)
        for (int xi = -2; xi <= 2; xi += 1)
        {
            gorynych::test::test<vreal, sreal>(
                    [=]() { return noisegen_wrapper<sreal, sint>::realvalue_3d({ xi, yi, zi }, 1010); },
                    [=]() { return noisegen_wrapper<vreal, vint>::realvalue_3d({ xi, yi, zi }, 1010); }
            );
        }
    }
//
//    TEST_CASE(TYPE_PREFIX" noise seeder", "[noise]")
//    {
//        sint seed = 1010;
//
//        for (int zi = -2; zi <= 2; zi += 1)
//        for (int yi = -2; yi <= 2; yi += 1)
//        for (int xi = -2; xi <= 2; xi += 1)
//        {
//            gorynych::test::test<vint, sint>(
//                    [=]() { return  (SEED_NOISE_GEN<sint>() * seed + dot(NOISE_GEN<sint>(), { xi, yi, zi })) & 0x7FFFFFFF; },
//                    [=]() { return  (SEED_NOISE_GEN<vint>() * seed + dot(NOISE_GEN<vint>(), { xi, yi, zi })) & 0x7FFFFFFF; }
//            );
//        }
//    }
//
//    TEST_CASE(TYPE_PREFIX" shifted noise seeder", "[noise]")
//    {
//        sint seed = 1010;
//
//        for (int zi = -2; zi <= 2; zi += 1)
//        for (int yi = -2; yi <= 2; yi += 1)
//        for (int xi = -2; xi <= 2; xi += 1)
//        {
//            gorynych::test::test<vint, sint>(
//                    [=]() { auto n = (SEED_NOISE_GEN<sint>() * seed + dot(NOISE_GEN<sint>(), { xi, yi, zi })) & 0x7FFFFFFF; return n ^= (n >> 13);},
//                    [=]() { auto n = (SEED_NOISE_GEN<vint>() * seed + dot(NOISE_GEN<vint>(), { xi, yi, zi })) & 0x7FFFFFFF; return n ^= (n >> 13);}
//            );
//        }
//    }

//    TEST_CASE(TYPE_PREFIX" 0x7FFFFFFF", "[constants]")
//    {
//        gorynych::test::test<vint, sint>(
//                [=]() { return numeric_limits<sint>::max(); },// 2147483647;}, // 0x7FFFFFFF
//                [=]() { return ccl<vint>::ones() >> 1;}
//        );
//    }
//
//    TEST_CASE(TYPE_PREFIX" 0x80000000", "[constants]")
//    {
//        gorynych::test::test<vint, sint>(
//                [=]() { return numeric_limits<sint>::min();},//-2147483648;},
//                [=]() { return ccl<vint>::ones() << 31;}
//        );
//    }
/*
    TEST_CASE(TYPE_PREFIX" intnoise3d impl", "[noise]")
    {
        sint seed = 111;

        //cout << " imax: " <<  numeric_limits<int>::max() << endl;
        //int xi = 1, yi = -1, zi = -1;
        for (int zi = -2; zi <= 2; zi += 1)
        for (int yi = -2; yi <= 2; yi += 1)
        for (int xi = -2; xi <= 2; xi += 1)
        {
            gorynych::test::test<vint, sint>(
                    [=]() {
                        long long n = (SEED_NOISE_GEN<sint>() * seed + dot(NOISE_GEN<sint>(), { xi, yi, zi }));
                        n &= static_cast<int>(0x7FFFFFFF);// & numeric_limits<sint>::max();
                        n ^= (n >> 13);
                        long long r = (n * (n * n * 1087 + 2749) + 3433);
                        sint r2 = r & static_cast<int>(0x7FFFFFFF);//0x7FFFFFFF);// & numeric_limits<sint>::max();

//                        auto ng = NOISE_GEN<unsigned>();
//                        unsigned dnc = ng.x * xi + ng.y * yi + ng.z * zi;
//
//                        unsigned n = (SEED_NOISE_GEN<unsigned>() * seed + dnc) & numeric_limits<int>::max();//& static_cast<vint>(0x7fffffff);// ccl<vint>::max();
//
//                        n ^= (n >> 13);
//
//                        //return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
//                        n = (n * (n * n * 1087 + 2749) + 3433);// & static_cast<vint>(0x7FFFFFFF); // & ccl<vint>::max();
//
//                        auto r =  *reinterpret_cast<int*>(&n);
//
//                        auto r2 = r & numeric_limits<int>::max();
                        cout << "sisd: " << r <<  " | " << r2 << endl;
                        return r;
                    },
                    [=]()
                    {
                        auto n = (SEED_NOISE_GEN<vint>() * seed + dot(NOISE_GEN<vint>(), { xi, yi, zi }));
                        n &= (ccl<vint>::ones() >> 1);// & (ccl<vint>::ones() >> 1);//numeric_limits<sint>::max();
                        n ^= (n >> 13);
                        auto r = (n * (n * n * 1087 + 2749) + 3433);// & (ccl<vint>::ones() >> 1);;
                        sint e[dim<vint>()];
                        extract(r, e);

                        auto r2 = r & (ccl<vint>::ones() >> 1);

                        sint e2[dim<vint>()];
                        extract(r2, e2);

                        cout << "simd: " << e[0] << " | "<< e2[0] << endl;
                        return r;}// & numeric_limits<sint>::max();}
            );
        }
    }*/
    TEST_CASE(TYPE_PREFIX" intvalue 3d", "[noise]")
    {
//        for (int zi = -2; zi <= 2; zi += 1)
//        for (int yi = -2; yi <= 2; yi += 1)
//        for (int xi = -2; xi <= 2; xi += 1)
//        {
//            gorynych::test::test<vint, sint>(
//                    [=]() { return noisegen_wrapper<sreal, sint>::intvalue_3d({ xi, yi, zi }, 1010); },
//                    [=]() { return noisegen_wrapper<vreal, vint>::intvalue_3d({ xi, yi, zi }, 1010); }
//            );
//        }
    }

    TEST_CASE(TYPE_PREFIX" construct cube", "[noise]")
    {
        for (float z = -1; z <= 1; z += 0.25f)
        for (float y = -1; y <= 1; y += 0.25f)
        for (float x = -1; x <= 1; x += 0.25f)
        {
            auto expected   = noisegen_wrapper<sreal, sint>::construct_cube({ x, y, z });
            auto tested     = noisegen_wrapper<vreal, vint>::construct_cube({ x, y, z });

            gorynych::test::test<vreal, sreal>(
                    [&]() { return expected(0, 0); },
                    [&]() { return tested(0, 0); }
            );

            gorynych::test::test<vreal, sreal>(
                    [&]() { return expected(0, 1); },
                    [&]() { return tested(0, 1); }
            );

            gorynych::test::test<vreal, sreal>(
                    [&]() { return expected(0, 2); },
                    [&]() { return tested(0, 2); }
            );


            gorynych::test::test<vreal, sreal>(
                    [&]() { return expected(1, 0); },
                    [&]() { return tested(1, 0); }
            );

            gorynych::test::test<vreal, sreal>(
                    [&]() { return expected(1, 2); },
                    [&]() { return tested(1, 2); }
            );

            gorynych::test::test<vreal, sreal>(
                    [&]() { return expected(1, 2); },
                    [&]() { return tested(1, 2); }
            );
        }
    }

}}}