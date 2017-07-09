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

#include "noise/noisegenerators.hpp"

namespace cacophony { namespace  test {
    using namespace zacc;
    using namespace zacc::test;
    using namespace zacc::math;

    using scalar_branch = zacc::scalar::types<branches::scalar>;
    using vector_branch = dispatched_branch::types;

    TEST(noise, gradient_coherent_3d)
    {
            for (auto q = 0; q <= 2; q++)
            for (float z = -1; z <= 1; z += 0.25)
            for (float y = -1; y <= 1; y += 0.25)
            for (float x = -1; x <= 1; x += 0.25)
            {
                auto expected = noisegen<scalar_branch>::gradient_coherent_3d({ x, y, z }, 1010, Quality(q));
                auto actual = noisegen<vector_branch>::gradient_coherent_3d({ x, y, z }, 1010, Quality(q));

                VASSERT_EQ(actual, expected.value());
            }
    }

    TEST(noise, gradient_3d)
    {
            for (int zi = -2; zi <= 2; zi += 1)
            for (int yi = -2; yi <= 2; yi += 1)
            for (int xi = -2; xi <= 2; xi += 1)
            for (float z = -1; z <= 1; z += 0.25)
            for (float y = -1; y <= 1; y += 0.25)
            for (float x = -1; x <= 1; x += 0.25)
            {
                auto expected = noisegen<scalar_branch>::gradient_3d({ x, y, z }, { xi, zi, yi }, 1010);
                auto actual = noisegen<vector_branch>::gradient_3d({ x, y, z }, { xi, zi, yi }, 1010);

                VASSERT_EQ(actual, expected.value());
            }
    }

    TEST(noise, value_coherent_3d)
    {
        for (auto q = 0; q <= 2; q++)
        for (float z = -1; z <= 1; z += 0.25)
        for (float y = -1; y <= 1; y += 0.25)
        for (float x = -1; x <= 1; x += 0.25)
        {
            auto expected = noisegen<scalar_branch>::value_coherent_3d({ x, y, z }, 1010, Quality(q));
            auto actual = noisegen<vector_branch>::value_coherent_3d({ x, y, z }, 1010, Quality(q));

            VASSERT_EQ(actual, expected.value());
        }
    }

    TEST(noise, realvalue_3d)
    {
        for (int zi = -2; zi <= 2; zi += 1)
        for (int yi = -2; yi <= 2; yi += 1)
        for (int xi = -2; xi <= 2; xi += 1)
        {
            auto expected = noisegen<scalar_branch>::realvalue_3d({ xi, yi, zi }, 1010);
            auto actual = noisegen<vector_branch>::realvalue_3d({ xi, yi, zi }, 1010);

            VASSERT_EQ(actual, expected.value());
        }
    }

    TEST(noise, construct_cube)
    {
        for (float z = -1; z <= 1; z += 0.25f)
        for (float y = -1; y <= 1; y += 0.25f)
        for (float x = -1; x <= 1; x += 0.25f)
        {
            auto expected   =  noisegen<scalar_branch>::construct_cube({ x, y, z });
            auto actual     =  noisegen<vector_branch>::construct_cube({ x, y, z });


            VASSERT_EQ(actual(0, 0), expected(0, 0).value());
            VASSERT_EQ(actual(0, 1), expected(0, 1).value());
            VASSERT_EQ(actual(0, 2), expected(0, 2).value());
            VASSERT_EQ(actual(1, 0), expected(1, 0).value());
            VASSERT_EQ(actual(1, 1), expected(1, 1).value());
            VASSERT_EQ(actual(1, 2), expected(1, 2).value());
        }
    }

}}