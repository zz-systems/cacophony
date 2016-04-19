#pragma once
#ifndef PARANOISE_NOISE_GENERATORS
#define PARANOISE_NOISE_GENERATORS

//#include "parallel/x87compat.h"


#include <iostream>

#include "base.h"
#include "parallel/all.h"
#include "interpolation.h"
#include "vectortable.h"
#include "vector.h"
#include "math/matrix.h"

#define NOISE_VERSION 2

namespace zzsystems { namespace paranoise { namespace generators {
	using namespace interpolation;
	using namespace simdal;
	using namespace math;
	//using namespace x87compat;

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
		
	namespace detail {
		using namespace simdal;
		using namespace math;
		// Create a unit-length cube aligned along an integer boundary.  This cube
		// surrounds the input point.
		SIMD_ENABLED
		mat3x2<vint> construct_cube(const vec3<vreal>& c)
		{		
			// Assuming that x > 0 yields either 0 or (+-)1, the result is ANDed with 1
			// => equivalent to (x > 0.0? (int)x: (int)x - 1)
			vec3<vint> lowerEdge
			(
				vfloor(c.x), //(vint)c.x - sel(c.x > 0, consts<vint>::zero(), consts<vint>::one()),
				vfloor(c.y), //(vint)c.y - sel(c.y > 0, consts<vint>::zero(), consts<vint>::one()),
				vfloor(c.z)  //(vint)c.z - sel(c.z > 0, consts<vint>::zero(), consts<vint>::one())
			);

			return mat3x2<vint>(
				lowerEdge,
				lowerEdge + fastload<vint>::_1()
			);
		}

		SIMD_ENABLE(vreal, vint)
		inline vec3<vreal> map_coord_diff(	const vec3<vreal>& c,
											const mat3x2<vint>& cube,
											Quality quality)
		{
			// Map the difference between the coordinates of the input value and the
			// coordinates of the cube's outer-lower-left vertex onto an S-curve.
			auto s = c - static_cast<vec3<vreal>>(cube._0);

			/*auto best = quality == Quality::Best;
			auto fast = quality == Quality::Standard;

			s.x = sel(best, scurve5(s.x), sel(fast, scurve3(s.x), s.x));
			s.y = sel(best, scurve5(s.y), sel(fast, scurve3(s.y), s.y));
			s.z = sel(best, scurve5(s.z), sel(fast, scurve3(s.z), s.z));*/

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
	}
		
	SIMD_ENABLE(vreal, vint)
		 vreal grad(vint &hash, const vec3<vreal> &coords)
	{
		auto h = hash & fastload<vint>::_15();
		auto u = vsel(h < fastload<vint>::_8(), coords.x, coords.y);
		auto v = vsel(h < fastload<vint>::_4(), coords.y, vsel(h == 12 || h == 14, coords.x, coords.z));

		return static_cast<vreal>(
			vsel((h & fastload<vint>::_1()) == fastload<vint>::_0(), u, -u) + 
			vsel((h & fastload<vint>::_2()) == fastload<vint>::_0(), v, -v)
			);
	}	

	template<typename featuremask>
	vec3<_float4> gatherRandoms(_int4 &index)
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
		
		return {			
			_mm_shuffle_ps(_Tmp0, _Tmp1, 0x88), //rv0,
			_mm_shuffle_ps(_Tmp0, _Tmp1, 0xDD), //rv1,
			_mm_shuffle_ps(_Tmp2, _Tmp3, 0x88) //rv2,
		};
	}

	FEATURE_FUNC(gatherRandoms, vec3<_float8>, _dispatcher::has_avx && !_dispatcher::has_avx2)
		(_int4x2 &index)
	{
		auto hi = gatherRandoms(index.hi), 
			 lo = gatherRandoms(index.lo);

		return {
			_mm256_set_m128(hi.x.val, lo.x.val),
			_mm256_set_m128(hi.y.val, lo.y.val),
			_mm256_set_m128(hi.z.val, lo.z.val)
		};
	}

	FEATURE_FUNC(gatherRandoms, vec3<_float8>, _dispatcher::has_avx2)
		 (_int8 &index)
	{
		return {
			_mm256_i32gather_ps(random_vectors<float>::values,		index.val, sizeof(float)),
			_mm256_i32gather_ps(random_vectors<float>::values + 1,	index.val, sizeof(float)),
			_mm256_i32gather_ps(random_vectors<float>::values + 2,	index.val, sizeof(float)),
		};
	}

	inline vec3<float> gatherRandoms(int index)
	{
		return {
			random_vectors<float>::values[index],
			random_vectors<float>::values[index + 1],
			random_vectors<float>::values[index + 2],
		};
	}


	SIMD_ENABLE(vreal, vint)
	vreal GradientNoise3D(	const vec3<vreal>& input,
							const vec3<vint>& nearby, 
							const vint &seed)
	{
		auto word	= dim<vreal>();
		auto vi		= (SEED_NOISE_GEN<vint>() * seed + dot(NOISE_GEN<vint>(), nearby)) & static_cast<vint>(0xFFFF'FFFF);		
		auto diff	= input - static_cast<vec3<vreal>>(nearby);

		vi ^= (vi >> SHIFT_NOISE_GEN);		
		
		vi &= (vint) 0xFF;	
		vi <<= 2;

		auto gradients = gatherRandoms(vi);	
		


		return dot(gradients, diff) * static_cast<vreal>(2.12f);

		//return dot(grad, diff) * 2.12f;
	}
	

	SIMD_ENABLE(vreal, vint)
	vreal GradientCoherentNoise3D(	const vec3<vreal>& c,
									const vint &seed, 
									Quality quality)
	{		
		mat3x2<vint> cube = detail::construct_cube<vreal, vint>(c);

		vec3<vreal> s = detail::map_coord_diff<vreal, vint>(c, cube, quality);	

		// Now calculate the noise values at each vertex of the cube.  To generate
		// the coherent-noise value at the input point, interpolate these eight
		// noise values using the S-curve value as the interpolant (trilinear
		// interpolation.)
		vreal n0, n1, ix0, ix1, iy0, iy1;

		
		n0 = GradientNoise3D(c, { cube._0.x, cube._0.y, cube._0.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._0.y, cube._0.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = GradientNoise3D(c, { cube._0.x, cube._1.y, cube._0.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._1.y, cube._0.z }, seed);

		ix1 = lerp(n0, n1, s.x);
		iy0 = lerp(ix0, ix1, s.y);

		n0 = GradientNoise3D(c, { cube._0.x, cube._0.y, cube._1.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._0.y, cube._1.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = GradientNoise3D(c, { cube._0.x, cube._1.y, cube._1.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._1.y, cube._1.z }, seed);

		ix1 = lerp(n0, n1, s.x);
		iy1 = lerp(ix0, ix1, s.y);

		return lerp(iy0, iy1, s.z);
	}	

	SIMD_ENABLE_I(vint)
	vint IntValueNoise3D(const vec3<vint>& c, const vint &seed)
	{
		vint n = (SEED_NOISE_GEN<vint>() * seed + dot(NOISE_GEN<vint>(), c)) & 0x7fffffff;

		n ^= (n >> 13);

		//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
		return (n * (n * n * 1087 + 2749) + 3433) & 0x7fffffff;
	}

	//SIMD_ENABLE(vreal, vint)
	//vreal IntValueNoise3D(const vec3<vint>& c, const vint &seed)
	//{
	//	vint n = vfmadd(SEED_NOISE_GEN<vint>(), seed, dot(NOISE_GEN<vint>(), c)) 
	//			& static_cast<vint>(0x7fffffff);

	//	auto k = static_cast<vreal>(n ^ (n >> 13));

	//	//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	//	auto kk = k * k;
	//	auto m = vfmadd(kk, static_cast<vreal>(1087.0f), static_cast<vreal>(2749.0f));
	//	return clamp_int32(vfmadd(k, m, static_cast<vreal>(3433.0f)));
	//}

	SIMD_ENABLE(vreal, vint)
	vreal ValueNoise3D(const vec3<vint>& c, const vint &seed)
	{
		// 1 - noise / 1073741824.0 => noise * (-1 / 1073741824.0) + 1

		//return 1 - static_cast<vreal>(IntValueNoise3D(c, seed)) / 1073741824.0f;
		return vfmadd(
				static_cast<vreal>(IntValueNoise3D(c, seed)), 
				//IntValueNoise3D<vreal, vint>(c, seed),
				valuenoise_adjust<vreal>(), 
				fastload<vreal>::_1());
	}

	SIMD_ENABLE(vreal, vint)
	vreal ValueCoherentNoise3D(const vec3<vreal>& c, const vint &seed, Quality quality)
	{
		mat3x2<vint> cube = detail::construct_cube<vreal, vint>(c);

		vec3<vreal> s = detail::map_coord_diff<vreal, vint>(c, cube, quality);

		vreal n0, n1, ix0, ix1, iy0, iy1;

		n0 = ValueNoise3D<vreal, vint>({ cube._0.x, cube._0.y, cube._0.z }, seed);
		n1 = ValueNoise3D<vreal, vint>({ cube._1.x, cube._0.y, cube._0.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = ValueNoise3D<vreal, vint>({ cube._0.x, cube._1.y, cube._0.z }, seed);
		n1 = ValueNoise3D<vreal, vint>({ cube._1.x, cube._1.y, cube._0.z }, seed);

		ix1 = lerp(n0, n1, s.x);
		iy0 = lerp(ix0, ix1, s.y);

		n0 = ValueNoise3D<vreal, vint>({ cube._0.x, cube._0.y, cube._1.z }, seed);
		n1 = ValueNoise3D<vreal, vint>({ cube._1.x, cube._0.y, cube._1.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = ValueNoise3D<vreal, vint>({ cube._0.x, cube._1.y, cube._1.z }, seed);
		n1 = ValueNoise3D<vreal, vint>({ cube._1.x, cube._1.y, cube._1.z }, seed);

		ix1 = lerp(n0, n1, s.y);
		iy1 = lerp(ix0, ix1, s.y);

		return lerp(iy0, iy1, s.z);
	}
		
}}}

#endif