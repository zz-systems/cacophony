#pragma once
#ifndef PARANOISE_NOISE_GENERATORS
#define PARANOISE_NOISE_GENERATORS

//#include "parallel/x87compat.h"


#include <iostream>


#include "parallel/all.h"
#include "interpolation.h"
#include "vectortable.h"
#include "vector.h"

#define NOISE_VERSION 2

namespace paranoise {	namespace generators {
	using namespace interpolation;
	using namespace parallel;

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
	constexpr Vector3<T> NOISE_GEN() { return Vector3<T>(1619, 31337, 6971); }

	const int _SEED_NOISE_GEN = 1013;
	const int SHIFT_NOISE_GEN = 8;

	template<typename T>
	constexpr T SEED_NOISE_GEN() { return (T)1013;	}
	
	template<typename T>
	constexpr T valuenoise_adjust() { return - 1.0f / 1073741824.0f; }
#endif
		
	namespace detail {
		// Create a unit-length cube aligned along an integer boundary.  This cube
		// surrounds the input point.
		SIMD_ENABLE(TReal, TInt)
		inline Matrix3x2<TInt> construct_cube(const Vector3<TReal>& c)
		{		
			// Assuming that x > 0 yields either 0 or (+-)1, the result is ANDed with 1
			// => equivalent to (x > 0.0? (int)x: (int)x - 1)
			Vector3<TInt> lowerEdge
			(
				vfloor(c.x), //(TInt)c.x - sel(c.x > 0, consts<TInt>::zero(), consts<TInt>::one()),
				vfloor(c.y), //(TInt)c.y - sel(c.y > 0, consts<TInt>::zero(), consts<TInt>::one()),
				vfloor(c.z)  //(TInt)c.z - sel(c.z > 0, consts<TInt>::zero(), consts<TInt>::one())
			);

			return Matrix3x2<TInt>(
				lowerEdge,
				lowerEdge +	static_cast<Vector3<TInt>>(1)
			);
		}

		SIMD_ENABLE(TReal, TInt)
		inline Vector3<TReal> map_coord_diff(	const Vector3<TReal>& c,
											const Matrix3x2<TInt>& cube,
											Quality quality)
		{
			// Map the difference between the coordinates of the input value and the
			// coordinates of the cube's outer-lower-left vertex onto an S-curve.
			auto s = c - static_cast<Vector3<TReal>>(cube._0);

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
	template<typename featuremask>
	FORCEINLINE Vector3<float4<featuremask>> gatherRandoms(int4<featuremask> index)
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
		
		_MM_TRANSPOSE4_PS(rv0, rv1, rv2, rv3)

		return {			
			rv0,
			rv1,
			rv2,
		};
	}

	FORCEINLINE Vector3<float> gatherRandoms(int index)
	{
		return {
			random_vectors<float>::values[index],
			random_vectors<float>::values[index + 1],
			random_vectors<float>::values[index + 2],
		};
	}


	SIMD_ENABLE(TReal, TInt)
	FORCEINLINE TReal GradientNoise3D(	const Vector3<TReal>& input,
							const Vector3<TInt>& nearby, 
							const TInt seed)
	{
		auto word	= dim<TReal>();
		auto vi		= (SEED_NOISE_GEN<TInt>() * seed + dot(NOISE_GEN<TInt>(), nearby)) & static_cast<TInt>(0xFFFF'FFFF);		
		auto diff	= input - static_cast<Vector3<TReal>>(nearby);

		vi ^= (vi >> SHIFT_NOISE_GEN);		
		
		vi &= (TInt) 0xFF;	
		vi <<= 2;

		auto grad = gatherRandoms(vi);	

		return dot(grad, diff) * static_cast<TReal>(2.12f);

		//return dot(grad, diff) * 2.12f;
	}
	

	SIMD_ENABLE(TReal, TInt)
	TReal GradientCoherentNoise3D(	const Vector3<TReal>& c,
									const TInt seed, 
									Quality quality)
	{		
		Matrix3x2<TInt> cube = detail::construct_cube<TReal, TInt>(c);

		Vector3<TReal> s = detail::map_coord_diff<TReal, TInt>(c, cube, quality);	

		// Now calculate the noise values at each vertex of the cube.  To generate
		// the coherent-noise value at the input point, interpolate these eight
		// noise values using the S-curve value as the interpolant (trilinear
		// interpolation.)
		TReal n0, n1, ix0, ix1, iy0, iy1;

		
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

	SIMD_ENABLE_I(TInt)
	FORCEINLINE TInt IntValueNoise3D(const Vector3<TInt>& c, const TInt seed)
	{
		TInt n = (SEED_NOISE_GEN<TInt>() * seed + dot(NOISE_GEN<TInt>(), c)) & 0x7fffffff;

		n ^= (n >> 13);

		//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
		return (n * (n * n * 1087 + 2749) + 3433) & 0x7fffffff;
	}

	SIMD_ENABLE(TReal, TInt)
	TReal IntValueNoise3D(const Vector3<TInt>& c, const TInt seed)
	{
		TInt n = (SEED_NOISE_GEN<TInt>() * seed + dot(NOISE_GEN<TInt>(), c)) & static_cast<TInt>(0x7fffffff);

		auto k = static_cast<TReal>(n ^ (n >> 13));

		//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
		auto kk = k * k;
		auto m = vfmadd(kk, static_cast<TReal>(1087.0f), static_cast<TReal>(2749.0f));
		return clamp_int32(vfmadd(k, m, static_cast<TReal>(3433.0f)));
	}

	SIMD_ENABLE(TReal, TInt)
	FORCEINLINE TReal ValueNoise3D(const Vector3<TInt>& c, const TInt seed)
	{
		// 1 - noise / 1073741824.0 => noise * (-1 / 1073741824.0) + 1

		//return 1 - static_cast<TReal>(IntValueNoise3D(c, seed)) / 1073741824.0f;
		return vfmadd(
				//static_cast<TReal>(IntValueNoise3D(c, seed)), 
				IntValueNoise3D<TReal, TInt>(c, seed),
				valuenoise_adjust<TReal>(), 
				fastload<TReal>::_1());
	}

	SIMD_ENABLE(TReal, TInt)
	TReal ValueCoherentNoise3D(const Vector3<TReal>& c, const TInt seed, Quality quality)
	{
		Matrix3x2<TInt> cube = detail::construct_cube<TReal, TInt>(c);

		Vector3<TReal> s = detail::map_coord_diff<TReal, TInt>(c, cube, quality);

		TReal n0, n1, ix0, ix1, iy0, iy1;

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._0.y, cube._0.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._0.y, cube._0.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._1.y, cube._0.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._1.y, cube._0.z }, seed);

		ix1 = lerp(n0, n1, s.x);
		iy0 = lerp(ix0, ix1, s.y);

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._0.y, cube._1.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._0.y, cube._1.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._1.y, cube._1.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._1.y, cube._1.z }, seed);

		ix1 = lerp(n0, n1, s.y);
		iy1 = lerp(ix0, ix1, s.y);

		return lerp(iy0, iy1, s.z);
	}
		
}}

#endif