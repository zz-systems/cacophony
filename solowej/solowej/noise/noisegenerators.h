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

#pragma once

#include <iostream>

#include "../math/interpolation.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "vectortable.h"

#define NOISE_VERSION 2

namespace zzsystems { namespace solowej {	
	using namespace gorynych;
	using namespace math;

	enum Quality {
		Fast,
		Standard,
		Best
	};


	// These constants control certain parameters that all coherent-noise
	// functions require.
#if (NOISE_VERSION == 1)
	// Constants used by the original version of libnoise.
	// Because X_NOISE_GEN is not relatively prime to the other values, and
	// Z_NOISE_GEN is close to 256 (the number of random gradient vectors),
	// patterns show up in high-frequency coherent noise.
	const int X_NOISE_GEN = 1;
	const int Y_NOISE_GEN = 31337;
	const int Z_NOISE_GEN = 263;
	const int SEED_NOISE_GEN = 1013;
	const int SHIFT_NOISE_GEN = 13;
#else
	// Constants used by the current version of libnoise.
	const int _X_NOISE_GEN = 1619;
	const int _Y_NOISE_GEN = 31337;
	const int _Z_NOISE_GEN = 6971;

	template<typename T>
	constexpr vec3<T> NOISE_GEN() { return vec3<T>(1619, 31337, 6971); }

	const int _SEED_NOISE_GEN = 1013;
	const int SHIFT_NOISE_GEN = 8;

	template<typename T>
	constexpr T SEED_NOISE_GEN() { return (T)1013;	}
	
	template<typename T>
	constexpr T valuenoise_adjust() { return - 1.0f / 1073741824.0f; }
#endif
		
	SIMD_ENABLED class noisegen
	{
	public:
		
		static vreal gradient_3d(const vec3<vreal>& input, const vec3<vint>& nearby, const vint &seed)
		{
			auto word = dim<vreal>();
			auto vi = (SEED_NOISE_GEN<vint>() * seed + dot(NOISE_GEN<vint>(), nearby)) & static_cast<vint>(0xFFFFFFFF);//ccl<vint>::ones();
			auto diff = input - static_cast<vec3<vreal>>(nearby);

			vi ^= (vi >> SHIFT_NOISE_GEN);

			vi &= static_cast<vint>(0xFF);
			vi <<= 2;

			auto grad = gather_randoms(vi);	
			
			//return dot(gradients, diff) * static_cast<vreal>(2.12f);

			return dot(grad, diff) * 2.12f;
		}

		static vreal gradient_coherent_3d(const vec3<vreal>& c,	const vint &seed, Quality quality)
		{
			mat3x2<vint> cube = construct_cube(c);

			vec3<vreal> s = map_coord_diff(c, cube, quality);

			// Now calculate the noise values at each vertex of the cube.  To generate
			// the coherent-noise value at the input point, interpolate these eight
			// noise values using the S-curve value as the interpolant (trilinear
			// interpolation.)
			vreal n0, n1, ix0, ix1, iy0, iy1;


			n0 = gradient_3d(c, { cube._0.x, cube._0.y, cube._0.z }, seed);
			n1 = gradient_3d(c, { cube._1.x, cube._0.y, cube._0.z }, seed);

			ix0 = lerp(n0, n1, s.x);

			n0 = gradient_3d(c, { cube._0.x, cube._1.y, cube._0.z }, seed);
			n1 = gradient_3d(c, { cube._1.x, cube._1.y, cube._0.z }, seed);

			ix1 = lerp(n0, n1, s.x);
			iy0 = lerp(ix0, ix1, s.y);

			n0 = gradient_3d(c, { cube._0.x, cube._0.y, cube._1.z }, seed);
			n1 = gradient_3d(c, { cube._1.x, cube._0.y, cube._1.z }, seed);

			ix0 = lerp(n0, n1, s.x);

			n0 = gradient_3d(c, { cube._0.x, cube._1.y, cube._1.z }, seed);
			n1 = gradient_3d(c, { cube._1.x, cube._1.y, cube._1.z }, seed);

			ix1 = lerp(n0, n1, s.x);
			iy1 = lerp(ix0, ix1, s.y);

			return lerp(iy0, iy1, s.z);
		}

		static vint intvalue_3d(const vec3<vint>& c, const vint &seed)
		{
			vint n = (SEED_NOISE_GEN<vint>() * seed + dot(NOISE_GEN<vint>(), c)) & numeric_limits<int>::max();//& static_cast<vint>(0x7fffffff);// ccl<vint>::max();

			n ^= (n >> 13);

			//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
			return (n * (n * n * 1087 + 2749) + 3433) & numeric_limits<int>::max();// & static_cast<vint>(0x7FFFFFFF); // & ccl<vint>::max();
		}

		static vreal realvalue_3d(const vec3<vint>& c, const vint &seed)
		{
			// 1 - noise / 1073741824.0 => noise * (-1 / 1073741824.0) + 1

			//return 1 - static_cast<vreal>(IntValueNoise3D(c, seed)) / 1073741824.0f;
			return vfmadd(
				static_cast<vreal>(intvalue_3d(c, seed)),
				//IntValueNoise3D<vreal, vint>(c, seed),
				valuenoise_adjust<vreal>(),
				cfl<vreal, 1>::val());
		}

		static vreal value_coherent_3d(const vec3<vreal>& c, const vint &seed, Quality quality)
		{
			mat3x2<vint> cube = construct_cube(c);

			vec3<vreal> s = map_coord_diff(c, cube, quality);

			vreal n0, n1, ix0, ix1, iy0, iy1;

			n0 = realvalue_3d({ cube._0.x, cube._0.y, cube._0.z }, seed);
			n1 = realvalue_3d({ cube._1.x, cube._0.y, cube._0.z }, seed);

			ix0 = lerp(n0, n1, s.x);

			n0 = realvalue_3d({ cube._0.x, cube._1.y, cube._0.z }, seed);
			n1 = realvalue_3d({ cube._1.x, cube._1.y, cube._0.z }, seed);

			ix1 = lerp(n0, n1, s.x);
			iy0 = lerp(ix0, ix1, s.y);

			n0 = realvalue_3d({ cube._0.x, cube._0.y, cube._1.z }, seed);
			n1 = realvalue_3d({ cube._1.x, cube._0.y, cube._1.z }, seed);

			ix0 = lerp(n0, n1, s.x);

			n0 = realvalue_3d({ cube._0.x, cube._1.y, cube._1.z }, seed);
			n1 = realvalue_3d({ cube._1.x, cube._1.y, cube._1.z }, seed);

			ix1 = lerp(n0, n1, s.y);
			iy1 = lerp(ix0, ix1, s.y);

			return lerp(iy0, iy1, s.z);
		}
	public:
		// Create a unit-length cube aligned along an integer boundary.  This cube
		// surrounds the input point.
		static mat3x2<vint> construct_cube(const vec3<vreal>& c)
		{
			vec3<vint> lowerEdge
			(
				static_cast<vint>(vfloor(c.x)),//*/ (vint)c.x - vsel(c.x > cfl<vreal, 0>::val(), cfl<vint, 0>::val(), cfl<vint, 1>::val()),
				static_cast<vint>(vfloor(c.y)),//*/ (vint)c.y - vsel(c.y > cfl<vreal, 0>::val(), cfl<vint, 0>::val(), cfl<vint, 1>::val()),
				static_cast<vint>(vfloor(c.z))//*/ (vint)c.z - vsel(c.z > cfl<vreal, 0>::val(), cfl<vint, 0>::val(), cfl<vint, 1>::val())
			);

			return mat3x2<vint>(
				lowerEdge,
				lowerEdge + cfl<vint, 1>::val()
				);
		}

		// Map the difference between the coordinates of the input value and the
		// coordinates of the cube's outer-lower-left vertex onto an S-curve.
		static vec3<vreal> map_coord_diff(const vec3<vreal>& c,	const mat3x2<vint>& cube, Quality quality)
		{			
			auto s = c - static_cast<vec3<vreal>>(cube._0);

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

			return s;
		}

		// Generic case: use gorynych gather functions.
		static vec3<vreal> gather_randoms(vint &index)
		{
			return vec3<vreal>(
				vgather(static_cast<const float*>(random_vectors<float>::values),		index),
				vgather(static_cast<const float*>(random_vectors<float>::values + 1),	index),
				vgather(static_cast<const float*>(random_vectors<float>::values + 2),	index)
				);
		}

#if defined(COMPILE_SSE2) || defined(COMPILE_SSE3) || defined(COMPILE_SSE4) || defined(COMPILE_SSE4FMA)
		// For SSE case: No need to gather. Load 4 vectors and transpose them - omitting the last (0 0 0 0) row
		FEATURE
		static vec3<_float4> gather_randoms(_int4 &index)
		{
			//_mm_prefetch
			auto a0 = random_vectors<float>::values + _mm_extract_epi32(index.val, 0);
			auto a1 = random_vectors<float>::values + _mm_extract_epi32(index.val, 1);
			auto a2 = random_vectors<float>::values + _mm_extract_epi32(index.val, 2);
			auto a3 = random_vectors<float>::values + _mm_extract_epi32(index.val, 3);

			//auto vvi = extract(index);
			//_mm_prefetch()
			auto rv0 = _mm_load_ps(a0);
			auto rv1 = _mm_load_ps(a1);
			auto rv2 = _mm_load_ps(a2);
			auto rv3 = _mm_load_ps(a3);

			//_MM_TRANSPOSE4_PS(rv0, rv1, rv2, rv3)

			__m128 _Tmp3, _Tmp2, _Tmp1, _Tmp0;

			_Tmp0 = _mm_shuffle_ps((rv0), (rv1), 0x44);
			_Tmp2 = _mm_shuffle_ps((rv0), (rv1), 0xEE);
			_Tmp1 = _mm_shuffle_ps((rv2), (rv3), 0x44);
			_Tmp3 = _mm_shuffle_ps((rv2), (rv3), 0xEE);

			return{
				_mm_shuffle_ps(_Tmp0, _Tmp1, 0x88), //rv0,
				_mm_shuffle_ps(_Tmp0, _Tmp1, 0xDD), //rv1,
				_mm_shuffle_ps(_Tmp2, _Tmp3, 0x88) //rv2,
			};			
		}
#endif
	};


	template<> inline int noisegen<float, int>::intvalue_3d(const vec3<int>& c, const int& seed)
	{
		auto ng = NOISE_GEN<unsigned>();
		//unsigned cx = c.x > 0 ? c.x :
		unsigned dnc = ng.x * c.x + ng.y * c.y + ng.z * c.z;

		unsigned n = (SEED_NOISE_GEN<unsigned>() * seed + dnc) & 0x7FFFFFFF;// numeric_limits<int>::max();//& static_cast<vint>(0x7fffffff);// ccl<vint>::max();

		n ^= (n >> 13);

		//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
		n = (n * (n * n * 1087 + 2749) + 3433);// & static_cast<vint>(0x7FFFFFFF); // & ccl<vint>::max();

		int nn = 0;

		if (n <= 0x7FFFFFFF)
			nn = static_cast<int>(n);
		else
		if (n >= 0x80000000)
			nn = static_cast<int>(n - 0x80000000) +  0x80000000;//0x7FFFFFFF + 1;//numeric_limits<int>::max();

		return nn  & 0x7FFFFFFF;//numeric_limits<int>::max();
	}

	template <> inline float noisegen<float, int>::gradient_3d(const vec3<float>& c, const vec3<int>& nearby, const int &seed)
	{
		auto ng = NOISE_GEN<unsigned>();
		unsigned dnc = ng.x * nearby.x + ng.y * nearby.y + ng.z * nearby.z;

		unsigned vi = (SEED_NOISE_GEN<unsigned>() * seed + dnc) & static_cast<int>(0xFFFFFFFF);//ccl<vint>::ones();


		vi ^= (vi >> SHIFT_NOISE_GEN);

		vi &= (0xFF);
		vi <<= 2;

		int vvi  = static_cast<int>(vi);
		auto grad = gather_randoms(vvi);

		auto diff = c - static_cast<vec3<float>>(nearby);

		//return dot(gradients, diff) * static_cast<vreal>(2.12f);

		return dot(grad, diff) * 2.12f;
	}
}}