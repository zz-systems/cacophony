//
// Vectorized and parallelized version of libnoise using the zacc SIMD toolkit
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

#pragma once

#include <iostream>

#include "math/interpolation.hpp"

#include "math/linear/generic_matrix.hpp"
#include "math/linear/specialized_matrix.hpp"
#include "zacc.hpp"
//#include "random_vector_repository.hpp"
#include "vectortable.hpp"

namespace cacophony {
	using namespace zacc;
	using namespace math;

	enum Quality {
		Fast,
		Standard,
		Best
	};

	DISPATCHED class noisegen
	{
		static constexpr const zfloat valuenoise_adjust()   { return  -1.0f / 1073741824.0f; }
		static constexpr const vec3<zint> noise_gen() 		{ return {1619, 31337, 6971}; }
		static constexpr const zint noise_gen_seed() 		{ return 1013; }
		static constexpr const int noise_gen_shift() 		{ return 8; }
		static constexpr const zint int_max() 				{ return 0x7FFFFFFF; }
	public:
		
		static zfloat gradient_3d(const vec3<zfloat>& input, const vec3<zint>& nearby, const zint &seed)
		{
			auto vi 	= (noise_gen_seed() * seed + noise_gen().dot(nearby)) & int_max();
			auto diff	 = input - static_cast<vec3<zfloat>>(nearby);

			vi ^= (vi >> noise_gen_shift());

			vi &= 0xFF;
			vi <<= 2;

			auto result = gather_randoms(vi).dot(diff) * 2.12f;

			ZTRACE("noise generator", "gradient_3d(" << input << ", " << nearby << ", " << seed << ") => " << result);

			return result;
		}

		static zfloat gradient_coherent_3d(const vec3<zfloat>& c, const zint &seed, Quality quality)
		{
			auto cube = construct_cube(c);

			auto diff = map_coord_diff(c, cube, quality);

			// Now calculate the noise values at each vertex of the cube.  To generate
			// the coherent-noise value at the input point, interpolate these eight
			// noise values using the S-curve value as the interpolant (trilinear
			// interpolation.)
			zfloat n0, n1, ix0, ix1, iy0, iy1;


			n0 = gradient_3d(c, { cube(0, 0), cube(0, 1), cube(0, 2) }, seed);
			n1 = gradient_3d(c, { cube(1, 0), cube(0, 1), cube(0, 2) }, seed);

			ix0 = lerp(n0, n1, diff.x);

			n0 = gradient_3d(c, { cube(0, 0), cube(1, 2), cube(0, 2) }, seed);
			n1 = gradient_3d(c, { cube(1, 0), cube(1, 2), cube(0, 2) }, seed);

			ix1 = lerp(n0, n1, diff.x);
			iy0 = lerp(ix0, ix1, diff.y);

			n0 = gradient_3d(c, { cube(0, 0), cube(0, 1), cube(1, 2) }, seed);
			n1 = gradient_3d(c, { cube(1, 0), cube(0, 1), cube(1, 2) }, seed);

			ix0 = lerp(n0, n1, diff.x);

			n0 = gradient_3d(c, { cube(0, 0), cube(1, 2), cube(1, 2) }, seed);
			n1 = gradient_3d(c, { cube(1, 0), cube(1, 2), cube(1, 2) }, seed);

			ix1 = lerp(n0, n1, diff.x);
			iy1 = lerp(ix0, ix1, diff.y);

			auto result = lerp(iy0, iy1, diff.z);

            ZTRACE("noise generator", "gradient_coherent_3d(" << c << ", " << seed << ", " << quality << ") => " << result);

            return result;
		}

		static zint intvalue_3d(const vec3<zint>& c, const zint &seed)
		{
			zint n = (noise_gen_seed() * seed + noise_gen().dot(c)) & int_max();

			n ^= (n >> 13);

			return (n * (n * n * 1087 + 2749) + 3433) & int_max();
		}

		static zfloat realvalue_3d(const vec3<zint>& c, const zint &seed)
		{
			//return 1 - static_cast<zfloat>(IntValueNoise3D(c, seed)) / 1073741824.0f;
			return vfmadd(
				static_cast<zfloat>(intvalue_3d(c, seed)),
				valuenoise_adjust(),
				1);
		}

		static zfloat value_coherent_3d(const vec3<zfloat>& c, const zint &seed, Quality quality)
		{
			auto cube = construct_cube(c);

			auto diff = map_coord_diff(c, cube, quality);

			zfloat n0, n1, ix0, ix1, iy0, iy1;

			n0 = realvalue_3d({ cube(0, 0), cube(0, 1), cube(0, 2) }, seed);
			n1 = realvalue_3d({ cube(1, 0), cube(0, 1), cube(0, 2) }, seed);

			ix0 = lerp(n0, n1, diff.x);

			n0 = realvalue_3d({ cube(0, 0), cube(1, 2), cube(0, 2) }, seed);
			n1 = realvalue_3d({ cube(1, 0), cube(1, 2), cube(0, 2) }, seed);

			ix1 = lerp(n0, n1, diff.x);
			iy0 = lerp(ix0, ix1, diff.y);

			n0 = realvalue_3d({ cube(0, 0), cube(0, 1), cube(1, 2) }, seed);
			n1 = realvalue_3d({ cube(1, 0), cube(0, 1), cube(1, 2) }, seed);

			ix0 = lerp(n0, n1, diff.x);

			n0 = realvalue_3d({ cube(0, 0), cube(1, 2), cube(1, 2) }, seed);
			n1 = realvalue_3d({ cube(1, 0), cube(1, 2), cube(1, 2) }, seed);

			ix1 = lerp(n0, n1, diff.y);
			iy1 = lerp(ix0, ix1, diff.y);

			auto result = lerp(iy0, iy1, diff.z);

			ZTRACE("noise generator", "value_coherent_3d(" << c << ", " << seed << ", " << quality << ") => " << result);

			return result;
		}
	public:
		/**
		 * @brief Create a unit-length cube aligned along an integer boundary.
		 * This cube surrounds the input point.
		 * @param c input point
		 * @return cube surroinding the input point
		 */
		static mat2x3<zint> construct_cube(const vec3<zfloat>& c)
		{
			vec3<zint> lowerEdge
			(
				c.x.floor(),
				c.y.floor(),
				c.z.floor()
			);

			return {
                lowerEdge,
                lowerEdge + 1
            };
		}

		/**
		 * @brief Map the difference between the coordinates of the input value and the
		 * coordinates of the cube's outer-lower-left vertex onto an S-curve.
		 * @param c input point
		 * @param cube surrounding cube
		 * @param quality interpolation quality
		 * @return
		 */
		static vec3<zfloat> map_coord_diff(const vec3<zfloat>& c,	const mat2x3<zint>& cube, Quality quality)
		{			
			auto s = c - static_cast<vec3<zfloat>>(cube.get_row(0));

			switch (quality) {
			case Quality::Fast:
				break;
			case Quality::Standard:
				s.x = scurve3(s.x);
				s.y = scurve3(s.y);
				s.z = scurve3(s.z);
				break;
			case Quality::Best:
				s.x = scurve5(s.x);
				s.y = scurve5(s.y);
				s.z = scurve5(s.z);
				break;
			}

			ZTRACE("noise generator", "map_coord_diff(" << c << ", " << cube << ", " << quality << ") => " << s);

			return s;
		}

        static vec3<zfloat> gather_randoms(const zint &index)
        {
            ZTRACE("noise generator", "gather randoms generic");
            return vec3<zfloat>(
                    zfloat::gather(zacc::make_raw(random_vectors.data()),		index),
                    zfloat::gather(zacc::make_raw(random_vectors.data() + 1),	index),
                    zfloat::gather(zacc::make_raw(random_vectors.data() + 2),	index)
            );
        }

//        DISPATCHED
//        static vec3<zfloat> gather_randoms(const zint &index)
//        {
//            return random_vector_repository<branch>::template impl<generic_gather>::at(index);
//        }
	};
}
