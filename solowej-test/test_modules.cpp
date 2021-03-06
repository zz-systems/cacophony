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

#include "../dependencies/gorynych/gorynych/gorynych.h"
#include "../dependencies/gorynych/gorynych-test/test_extensions.h"


#include "../solowej/modules/all.h"


namespace zzsystems { namespace solowej { namespace  test {
            using namespace gorynych;
            using namespace gorynych::test;
            using namespace zzsystems::solowej::modules;

#define TYPE_PREFIX TEST_PREFIX("modules")


    TEST_CASE(TYPE_PREFIX"perlin", "[modules][generators]")
    {
        mod_perlin<sreal, sint> scalar;
        scalar.seed = 111;
        mod_perlin<vreal, vint> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);
                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }

    }

    TEST_CASE(TYPE_PREFIX"voronoi", "[modules][generators]")
    {
        mod_voronoi<sreal, sint> scalar;
        scalar.seed = 111;
        mod_voronoi<vreal, vint> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);
                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX"billow", "[modules][generators]")
    {
        mod_billow<sreal, sint> scalar;
        scalar.seed = 111;
        mod_billow<vreal, vint> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);
                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX"ridgedmultifrac", "[modules][generators]")
    {
        mod_ridged_multifractal<sreal, sint> scalar;
        scalar.seed = 111;
        mod_ridged_multifractal<vreal, vint> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);
                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX"cylinders", "[modules][generators]")
    {
        mod_cylinders<sreal, sint> scalar;
        mod_cylinders<vreal, vint> vectorized;
        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);
                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX"spheres", "[modules][generators]")
    {
        mod_spheres<sreal, sint> scalar;
        mod_spheres<vreal, vint> vectorized;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);
                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    static mod_perlin<sreal, sint> src_s_a(2.0, 2.0, 0.625, 6, 111);
    static mod_perlin<vreal, vint> src_v_a(2.0, 2.0, 0.625, 6, 111);

    static mod_billow<sreal, sint> src_s_b(2.0, 2.0, 0.625, 6, 111);
    static mod_billow<vreal, vint> src_v_b(2.0, 2.0, 0.625, 6, 111);

    static mod_ridged_multifractal<sreal, sint> src_s_c(3.0, 2.0, 111, 8);
    static mod_ridged_multifractal<vreal, vint> src_v_c(3.0, 2.0, 111, 8);

    TEST_CASE(TYPE_PREFIX"curve", "[modules][modifiers]")
    {
        mod_curve<sreal, sint> scalar(
        {
                {-2,     -1.625},
                {-1,     -1.375},
                {0,      -0.375},
                {0.0625, -0.125},
                {0.125,  0.25},
                {0.25,   1.0},
                {0.5,    0.25},
                {0.75,   0.25},
                {1,      0.5},
                {2,      0.5}
        });
        mod_curve<vreal, vint> vectorized(
        {
                {-2,     -1.625},
                {-1,     -1.375},
                {0,      -0.375},
                {0.0625, -0.125},
                {0.125,  0.25},
                {0.25,   1.0},
                {0.5,    0.25},
                {0.75,   0.25},
                {1,      0.5},
                {2,      0.5}
        });

        scalar.set_source(src_s_a);
        vectorized.set_source(src_v_a);
        
        
        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);

                //cerr << "for [" << x << ", " << y << "] scalar: " << s << " vector: " << e[0] << " " << e[1] << " " << e[2] << " " << e[3] << endl;
                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX"rotate", "[modules][modifiers]")
    {
        mod_rotate<sreal, sint> scalar(vec3<float>(90, 45, 30));
        mod_rotate<vreal, vint> vectorized(vec3<float>(90, 45, 30));

        scalar.set_source(src_s_a);
        vectorized.set_source(src_v_a);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }


    TEST_CASE(TYPE_PREFIX"terrace", "[modules][modifiers]")
    {
        mod_terrace<sreal, sint> scalar(
        {
            -1.625,
            -1.375,
            -0.375,
            -0.125,
             0.25,
             1.0,
             0.25,
             0.25,
             0.5,
             0.5
        });
        mod_terrace<vreal, vint> vectorized(
        {
            -1.625,
            -1.375,
            -0.375,
            -0.125,
            0.25,
            1.0,
            0.25,
            0.25,
            0.5,
            0.5
        });

        scalar.set_source(src_s_a);
        vectorized.set_source(src_v_a);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX"turbulence", "[modules][modifiers]")
    {
        mod_turbulence<sreal, sint> scalar;
        mod_turbulence<vreal, vint> vectorized;

        scalar.set_source(src_s_a);
        vectorized.set_source(src_v_a);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX"select", "[modules][modifiers]")
    {
        mod_select<sreal, sint> scalar;
        mod_select<vreal, vint> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        scalar.set_controller(src_s_c);

        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);
        vectorized.set_controller(src_v_c);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX" add", "[modules][modifiers][primitives]")
    {
        mod_add<sreal, sint> scalar;
        mod_add<vreal, vint> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

    TEST_CASE(TYPE_PREFIX" sub", "[modules][modifiers][primitives]")
    {
        mod_sub<sreal, sint> scalar;
        mod_sub<vreal, vint> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }
    TEST_CASE(TYPE_PREFIX" mul", "[modules][modifiers][primitives]")
    {
        mod_mul<sreal, sint> scalar;
        mod_mul<vreal, vint> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }
    TEST_CASE(TYPE_PREFIX" div", "[modules][modifiers][primitives]")
    {
        mod_div<sreal, sint> scalar;
        mod_div<vreal, vint> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);

        return;
        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto s = scalar({x / d, y / d, 0});
                auto v = vectorized({x / d, y / d, 0});
                ALIGNED float e[dim<vreal>()];
                extract(v, e);


                gorynych::test::test<vreal, sreal>([&]() { return s; }, [&]() { return v; });

                //cout << "scalar: " << s << " vectorized: " << e[0] << endl;
            }
        }
    }

}}}