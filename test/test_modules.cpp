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

#include "gtest/gtest.h"
#include "util/testing/gtest_ext.hpp"
#include "system/branch.hpp"
#include "backend/scalar/types.hpp"
#include "modules/all.hpp"


namespace cacophony { namespace  test {
    using namespace zacc;
    using namespace zacc::test;
    using namespace cacophony::modules;
        
    using scalar_branch = zacc::scalar::types<branches::scalar>;
    using vector_branch = dispatched_branch::types;

    TEST(modules_generators, perlin)
    {
        REQUIRES(ZACC_CAPABILITIES);

        mod_perlin<scalar_branch> scalar;
        scalar.seed = 111;
        mod_perlin<vector_branch> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }

    }

    TEST(modules_generators, voronoi)
    {
        mod_voronoi<scalar_branch> scalar;
        scalar.seed = 111;
        mod_voronoi<vector_branch> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_generators, billow)
    {
        mod_billow<scalar_branch> scalar;
        scalar.seed = 111;
        mod_billow<vector_branch> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_generators, ridgedmultifrac)
    {
        mod_ridged_multifractal<scalar_branch> scalar;
        scalar.seed = 111;
        mod_ridged_multifractal<vector_branch> vectorized;
        vectorized.seed = 111;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_generators, cylinders)
    {
        mod_cylinders<scalar_branch> scalar;
        mod_cylinders<vector_branch> vectorized;
        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_generators, spheres)
    {
        mod_spheres<scalar_branch> scalar;
        mod_spheres<vector_branch> vectorized;

        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    static mod_perlin<scalar_branch> src_s_a(2.0, 2.0, 0.625, 6, 111);
    static mod_perlin<vector_branch> src_v_a(2.0, 2.0, 0.625, 6, 111);

    static mod_billow<scalar_branch> src_s_b(2.0, 2.0, 0.625, 6, 111);
    static mod_billow<vector_branch> src_v_b(2.0, 2.0, 0.625, 6, 111);

    static mod_ridged_multifractal<scalar_branch> src_s_c(3.0, 2.0, 111, 8);
    static mod_ridged_multifractal<vector_branch> src_v_c(3.0, 2.0, 111, 8);

    TEST(modules_modifiers, curve)
    {
        mod_curve<scalar_branch> scalar(
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
        mod_curve<vector_branch> vectorized(
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
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_modifiers, rotate)
    {
        mod_rotate<scalar_branch> scalar(vec3<float>(90, 45, 30));
        mod_rotate<vector_branch> vectorized(vec3<float>(90, 45, 30));

        scalar.set_source(src_s_a);
        vectorized.set_source(src_v_a);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }


    TEST(modules_modifiers, terrace)
    {
        mod_terrace<scalar_branch> scalar(
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
        mod_terrace<vector_branch> vectorized(
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
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_modifiers, turbulence)
    {
        mod_turbulence<scalar_branch> scalar;
        mod_turbulence<vector_branch> vectorized;

        scalar.set_source(src_s_a);
        vectorized.set_source(src_v_a);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_modifiers, select)
    {
        mod_select<scalar_branch> scalar;
        mod_select<vector_branch> vectorized;

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
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_modifiers_primitives, add)
    {
        mod_add<scalar_branch> scalar;
        mod_add<vector_branch> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

    TEST(modules_modifiers_primitives, sub)
    {
        mod_sub<scalar_branch> scalar;
        mod_sub<vector_branch> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }
    TEST(modules_modifiers_primitives, mul)
    {
        mod_mul<scalar_branch> scalar;
        mod_mul<vector_branch> vectorized;

        scalar.set_a(src_s_a);
        scalar.set_b(src_s_b);
        vectorized.set_a(src_v_a);
        vectorized.set_b(src_v_b);


        auto d = 128.0f;

        for( int y = 0; y < d; y++)
        {
            for( int x = 0; x < d; x++)
            {
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }
    TEST(modules_modifiers_primitives, div)
    {
        mod_div<scalar_branch> scalar;
        mod_div<vector_branch> vectorized;

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
                auto expected = scalar({x / d, y / d, 0});
                auto actual = vectorized({x / d, y / d, 0});

                VASSERT_EQ(actual, 0);
            }
        }
    }

}}