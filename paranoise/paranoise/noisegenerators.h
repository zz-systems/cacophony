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

namespace zzsystems { namespace paranoise { namespace generators {
	using namespace interpolation;
	using namespace simdal;
	using namespace util;
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
		using namespace simdal;

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
	SIMD_ENABLE(TReal, TInt)
		FORCEINLINE TReal grad(TInt hash, const Vector3<TReal> &coords)
	{
		auto h = hash & fastload<TInt>::_15();
		auto u = vsel(h < fastload<TInt>::_8(), coords.x, coords.y);
		auto v = vsel(h < fastload<TInt>::_4(), coords.y, vsel(h == 12 || h == 14, coords.x, coords.z));

		return static_cast<TReal>(
			vsel((h & fastload<TInt>::_1()) == fastload<TInt>::_0(), u, -u) + 
			vsel((h & fastload<TInt>::_2()) == fastload<TInt>::_0(), v, -v)
			);
	}	

	template<typename featuremask>
	FORCEINLINE Vector3<_float4> gatherRandoms(_int4 index)
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

	FEATURE_FUNC(gatherRandoms, Vector3<_float8>, _dispatcher::has_avx)
		 (_int8 index)
	{		
		//index.val = _mm256_permute2f128_si256(index.val, index.val, 1);
		//index.val = _mm256_permutevar8x32_epi32(index.val, _int8(7, 6, 5, 4, 3, 2, 1, 0).val);
		////index.val = _mm256_permute2x128_si256(x, 5);
		auto ix = index;
		auto iy = index + 1;
		auto iz = index + 2;
		
		return {
			_mm256_i32gather_ps(random_vectors<float>::values, ix.val, sizeof(float)),
			_mm256_i32gather_ps(random_vectors<float>::values, iy.val, sizeof(float)),
			_mm256_i32gather_ps(random_vectors<float>::values, iz.val, sizeof(float)),
		};

		//int i[8];

		//_mm256_store_si256(reinterpret_cast<__m256i*>(i), index.val);
		////auto i = _mm256_packus_epi32(index.val, index.val);
		//
		////_mm_prefetch
		//auto a0 = random_vectors<float>::values + i[7];//_mm_extract_epi16(i, 0);
		//auto a1 = random_vectors<float>::values + i[6];//_mm_extract_epi16(i, 1);
		//auto a2 = random_vectors<float>::values + i[5];//_mm_extract_epi16(i, 2);
		//auto a3 = random_vectors<float>::values + i[4];//_mm_extract_epi16(i, 3);

		//auto a4 = random_vectors<float>::values + i[3];//_mm_extract_epi16(i, 4);
		//auto a5 = random_vectors<float>::values + i[2];//_mm_extract_epi16(i, 5);
		//auto a6 = random_vectors<float>::values + i[1];//_mm_extract_epi16(i, 6);
		//auto a7 = random_vectors<float>::values + i[0];//_mm_extract_epi16(i, 7);

		//////auto vvi = extract(index);
		//////_mm_prefetch()
		//auto rv0 = _mm_load_ps(a0);
		//auto rv1 = _mm_load_ps(a1);
		//auto rv2 = _mm_load_ps(a2);
		//auto rv3 = _mm_load_ps(a3);

		//_MM_TRANSPOSE4_PS(rv0, rv1, rv2, rv3)

		//auto rv4 = _mm_load_ps(a4);
		//auto rv5 = _mm_load_ps(a5);
		//auto rv6 = _mm_load_ps(a6);
		//auto rv7 = _mm_load_ps(a7);

		//_MM_TRANSPOSE4_PS(rv4, rv5, rv6, rv7)

		//return{
		//	_mm256_set_m128(rv0, rv4),
		//	_mm256_set_m128(rv1, rv5),
		//	_mm256_set_m128(rv2, rv6)
		//};

		//auto __t0 = _mm256_unpacklo_ps(rv0, rv1);
		//auto __t1 = _mm256_unpackhi_ps(rv0, rv1);
		//auto __t2 = _mm256_unpacklo_ps(rv2, rv3);
		//auto __t3 = _mm256_unpackhi_ps(rv2, rv3);
		//auto __t4 = _mm256_unpacklo_ps(rv4, rv5);
		//auto __t5 = _mm256_unpackhi_ps(rv4, rv5);
		//auto __t6 = _mm256_unpacklo_ps(rv6, rv7);
		//auto __t7 = _mm256_unpackhi_ps(rv6, rv7);

		//auto __tt0 = _mm256_shuffle_ps(__t0, __t2, _MM_SHUFFLE(1, 0, 1, 0));
		//auto __tt1 = _mm256_shuffle_ps(__t0, __t2, _MM_SHUFFLE(3, 2, 3, 2));
		//auto __tt2 = _mm256_shuffle_ps(__t1, __t3, _MM_SHUFFLE(1, 0, 1, 0));
		//auto __tt3 = _mm256_shuffle_ps(__t1, __t3, _MM_SHUFFLE(3, 2, 3, 2));
		//auto __tt4 = _mm256_shuffle_ps(__t4, __t6, _MM_SHUFFLE(1, 0, 1, 0));
		//auto __tt5 = _mm256_shuffle_ps(__t4, __t6, _MM_SHUFFLE(3, 2, 3, 2));
		//auto __tt6 = _mm256_shuffle_ps(__t5, __t7, _MM_SHUFFLE(1, 0, 1, 0));
		//auto __tt7 = _mm256_shuffle_ps(__t5, __t7, _MM_SHUFFLE(3, 2, 3, 2));

		//rv0 = _mm256_permute2f128_ps(__tt0, __tt4, 0x20);
		//rv1 = _mm256_permute2f128_ps(__tt1, __tt5, 0x20);
		//rv2 = _mm256_permute2f128_ps(__tt2, __tt6, 0x20);
		////rv3 = _mm256_permute2f128_ps(__tt3, __tt7, 0x20);
		//rv4 = _mm256_permute2f128_ps(__tt0, __tt4, 0x31);
		/*rv2 = _mm256_permute2f128_ps(__tt1, __tt5, 0x31);
		rv1 = _mm256_permute2f128_ps(__tt2, __tt6, 0x31);
		rv0 = _mm256_permute2f128_ps(__tt3, __tt7, 0x31);*/

		/*return{
			rv0,
			rv1,
			rv2,
		};*/
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

		auto gradients = gatherRandoms(vi);	
		

		return dot(gradients, diff) * static_cast<TReal>(2.12f);

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
	TReal ValueNoise3D(const Vector3<TInt>& c, const TInt seed)
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
		
}}}

#endif