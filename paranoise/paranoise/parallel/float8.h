#pragma once
#ifndef PARANOISE_INTRINSIC_M256F_H
#define PARANOISE_INTRINSIC_M256F_H

#include "base.h"
#include "../capabiliy.h"

namespace paranoise {	namespace parallel {

	template<typename featuremask>
	struct int8;
	//struct double4;

#define _float8 float8<featuremask>
#define _int8 int8<featuremask>

	template<typename featuremask>
	struct ALIGN(32) float8 {
		__m256 val;

		float8() = default;
		float8(const float& rhs)	{ val = _mm256_set1_ps(rhs); }

		float8(VARGS8(uint8))		{ val = _mm256_cvtepi32_ps(_mm256_set_epi32(VPASS8)); }
		float8(VARGS8(int))			{ val = _mm256_cvtepi32_ps(_mm256_set_epi32(VPASS8)); }
		float8(VARGS8(float))		{ val = _mm256_set_ps(VPASS8); }

		float8(const __m256& rhs)	{ val = rhs; }
		float8(const __m256i& rhs)	{ val = _mm256_cvtepi32_ps(rhs); }
		float8(const __m256d& rhs)	{ val = _mm256_castps128_ps256(_mm256_cvtpd_ps(rhs)); }

		float8(const _float8&	rhs);
		float8(const _int8&	rhs);
		//float8(const double4&	rhs);

		static inline auto ones()
		{
			auto t = _mm_setzero_si128();
			return _mm_cmpeq_epi32(t, t);
		}

		static inline auto one()
		{
			return _mm_srli_epi32(ones(), 31);
		}

		static inline auto sign1all0()
		{
			return _mm_slli_epi32(ones(), 31);
		}

		static inline auto sign0all1()
		{
			return _mm_srli_epi32(ones(), 1);
		}
	};

	

	// Arithmetic =====================================================================================================
	FEATURE_BIN_OP(+, _float8, _dispatcher::has_avx)	
	{
		BIN_BODY(_mm256_add_ps); 
	}

	FEATURE_BIN_OP(-, _float8, _dispatcher::has_avx)	
	{
		BIN_BODY(_mm256_sub_ps);
	}
	FEATURE_BIN_OP(*, _float8, _dispatcher::has_avx)
	{
		BIN_BODY(_mm256_mul_ps); 
	}

	FEATURE_BIN_OP(/, _float8, _dispatcher::has_avx)	
	{
		BIN_BODY(_mm256_div_ps);
	}

	FEATURE_UN_OP(-, _float8, _dispatcher::has_avx)
	{
		BODY(_mm256_sub_ps(_mm256_setzero_ps(), a.val)); 
	}

	// Comparison =====================================================================================================
	FEATURE_BIN_OP(>, _float8, _dispatcher::has_avx)	
	{
		TRI_BODY_O(_mm256_cmp_ps, _CMP_GT_OQ); 
	}

	FEATURE_BIN_OP(< , _float8, _dispatcher::has_avx) 
	{
		TRI_BODY_O(_mm256_cmp_ps, _CMP_LT_OQ); 
	}

	FEATURE_BIN_OP(==, _float8, _dispatcher::has_avx)
	{
		TRI_BODY_O(_mm256_cmp_ps, _CMP_EQ_OQ); 
	}

	// Bitwise ========================================================================================================
	FEATURE_UN_OP(~, _float8, _dispatcher::has_avx)	
	{
		UN_BODY_D(_mm256_andnot_ps);
	}

	FEATURE_BIN_OP(|, _float8, _dispatcher::has_avx)
	{
		BIN_BODY(_mm256_or_ps); 
	}

	FEATURE_BIN_OP(&, _float8, _dispatcher::has_avx)
	{
		BIN_BODY(_mm256_and_ps);
	}

	FEATURE_BIN_OP(^, _float8, _dispatcher::has_avx)
	{
		BIN_BODY(_mm256_xor_ps);
	}
	
	// Special functions ==============================================================================================
	// Branchless select
	FEATURE_TRI_FUNC(vsel, _float8, _dispatcher::has_avx) 
	{
		TRI_BODY_R(_mm256_blendv_ps); 
	}
	// Fused multiply-add
	FEATURE_TRI_FUNC(vfmadd, _float8, _dispatcher::has_avx) 
	{
		TRI_BODY(_mm256_fmadd_ps); 
	}

	// Fused multiply-subtract
	FEATURE_TRI_FUNC(vfmsub, _float8, _dispatcher::has_avx) 
	{
		TRI_BODY(_mm256_fmsub_ps); 
	}

	// Mathematical functions =========================================================================================
	FEATURE_UN_FUNC(vabs, _float8, _dispatcher::has_avx)  
	{
		BODY(a & _float8::sign0all1());
	}
	
	FEATURE_BIN_FUNC(vmin, _float8, _dispatcher::has_avx)  
	{
		BIN_BODY(_mm256_min_ps); 
	}

	FEATURE_BIN_FUNC(vmax, _float8, _dispatcher::has_avx) 
	{
		BIN_BODY(_mm256_max_ps);
	}

	FEATURE_UN_FUNC(vsqrt, _float8, _dispatcher::has_avx) 
	{
		UN_BODY(_mm256_sqrt_ps); 
	}

	// Truncate float to *.0
	FEATURE_UN_FUNC(vtrunc, _float8, _dispatcher::has_avx && _dispatcher::has_avx2)
	{
		BODY(static_cast<_float8>(static_cast<_int8>(a)));
	}

	// Floor value
	FEATURE_UN_FUNC(vfloor, _float8, _dispatcher::has_avx && _dispatcher::has_avx2)
	{
		auto fi = vtrunc(a);
		return vsel(fi > a, fi - static_cast<_float8>(_float8::one()), fi);
	}

	// Ceil value
	FEATURE_UN_FUNC(vceil, _float8, _dispatcher::has_avx && _dispatcher::has_avx2)
	{
		auto fi = vtrunc(a);
		return vsel(fi < a, fi + static_cast<_float8>(_float8::one()), fi);
	}

	// Round value
	FEATURE_UN_FUNC(vround, _float4, _dispatcher::has_avx)
	{
		//generate the highest value < 2		
		auto vNearest2 = _mm256_castsi256_ps(_mm256_srli_epi32(_float4::ones(), 2));
		auto aTrunc = vtrunc(a);

		auto rmd = a - aTrunc;        // get remainder
		auto rmd2 = rmd * vNearest2;  // mul remainder by near 2 will yield the needed offset

		auto rmd2Trunc = vtrunc(rmd2); // after being truncated of course

		return aTrunc + rmd2Trunc;
	}


	//inline float8 vfloor(const float8& a) {
	//	auto v0 = _mm256_setzero_si256();
	//	auto v1 = _mm256_cmpeq_epi32(v0, v0);
	//	auto ji = _mm256_srli_epi32(v1, 25);
	//	auto j = *(__m256*)&_mm256_slli_epi32(ji, 23); //create vector 1.0f
	//	auto i = _mm256_cvttps_epi32(a.val);
	//	auto fi = _mm256_cvtepi32_ps(i);
	//	auto igx = _mm256_cmp_ps(fi, a.val, _CMP_GT_OS);
	//	j = _mm256_and_ps(igx, j);
	//	return _mm256_sub_ps(fi, j);
	//}

	//inline float8 vceil(const float8& a) {
	//	auto v0 = _mm256_setzero_si256();
	//	auto v1 = _mm256_cmpeq_epi32(v0, v0);
	//	auto ji = _mm256_srli_epi32(v1, 25);
	//	auto j = *(__m256*)&_mm256_slli_epi32(ji, 23); //create vector 1.0f
	//	auto i = _mm256_cvttps_epi32(a.val);
	//	auto fi = _mm256_cvtepi32_ps(i);
	//	auto igx = _mm256_cmp_ps(fi, a.val, _CMP_LT_OS);
	//	j = _mm256_and_ps(igx, j);
	//	return _mm256_add_ps(fi, j);
	//}

	//inline float8 vround(const float8 &a) {
	//	auto v0 = _mm256_setzero_ps();             //generate the highest value &lt; 2
	//	auto v1 = _mm256_cmp_ps(v0, v0, _CMP_EQ_OS);
	//	auto vNearest2 = *(__m256*)&_mm256_srli_epi32(*(__m256i*)&v1, 2);
	//	auto i = _mm256_cvttps_epi32(a.val);
	//	auto aTrunc = _mm256_cvtepi32_ps(i);        // truncate a
	//	auto rmd = _mm256_sub_ps(a.val, aTrunc);        // get remainder
	//	auto rmd2 = _mm256_mul_ps(rmd, vNearest2); // mul remainder by near 2 will yield the needed offset
	//	auto rmd2i = _mm256_cvttps_epi32(rmd2);    // after being truncated of course
	//	auto rmd2Trunc = _mm256_cvtepi32_ps(rmd2i);
	//	auto r = _mm256_add_ps(aTrunc, rmd2Trunc);
	//	return r;
	//}

	

}}

#endif