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

#define TYPE_PREFIX TEST_PREFIX("interpolation")

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