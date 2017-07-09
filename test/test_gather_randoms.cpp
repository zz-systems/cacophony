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
#include "noise/noisegenerators.hpp"

#include "system/branch.hpp"
#include "backend/scalar/types.hpp"
#include "noise/vectortable.hpp"

namespace cacophony { namespace  test {
        using namespace zacc;
        using namespace zacc::test;

        //using scalar_branch = zacc::scalar::types<branches::scalar>;
        using vector_branch = dispatched_branch::types;

        TEST(gather, gather)
        {
            for(int i = 0; i < random_vectors.size() / zfloat::dim; i++)
            {
                std::array<int, zint::dim> indices;
                for(size_t d = 0; d < zfloat::dim; d++)
                {
                    indices[d] = i + d;
                }

                auto actual = zfloat::gather(random_vectors.data(), zint(indices)).data();

                for(int d = 0; d < zfloat::dim; d++)
                {
                    ASSERT_EQ(random_vectors[i + d], actual[d]);
                }
            }
        }

        TEST(gather, gather_randoms)
        {
            for(int i = 0; i < random_vectors.size() / zfloat::dim; i++)
            {
                std::array<int, zint::dim> indices;
                for(size_t d = 0; d < zfloat::dim; d++)
                {
                    indices[d] = i + d;
                }

                auto ii = zint(indices);
                auto gathered = noisegen<vector_branch>::gather_randoms(ii);

                auto actualx = gathered.x.data();
                auto actualy = gathered.y.data();
                auto actualz = gathered.z.data();

                for(int d = 0; d < zfloat::dim; d++)
                {
                    ASSERT_EQ(random_vectors[i + d], actualx[d]);
                    ASSERT_EQ(random_vectors[i + d + 1], actualy[d]);
                    ASSERT_EQ(random_vectors[i + d + 2], actualz[d]);
                }
            }
        }
}}