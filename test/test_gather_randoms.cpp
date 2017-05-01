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

#include "../solowej/noise/noisegenerators.h"

namespace zzsystems { namespace solowej { namespace  test {
            using namespace gorynych;
            using namespace gorynych::test;

#define TYPE_PREFIX TEST_PREFIX("gather randoms")


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