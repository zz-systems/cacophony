#pragma once
#ifndef PARANOISE_INTRINSIC_M128F_H
#define PARANOISE_INTRINSIC_M128F_H

#include "base.h"

namespace paranoise { namespace parallel {
	struct int4;
	struct double2;
		
	ALIGN(16) struct float4 {
		__m128 val;

		float4() = default;
		float4(const float &rhs) {
			float c[4] = { rhs, rhs, rhs, rhs };
			val = _mm_load_ps(c);
		}	
		

		float4(const float* rhs) : val(_mm_load_ps(rhs)) {}

		float4(VARGS4(uint8))	{ val = _mm_cvtepi32_ps(_mm_set_epi32(VPASS4)); }
		float4(VARGS4(int32))	{ val = _mm_cvtepi32_ps(_mm_set_epi32(VPASS4)); }
		float4(VARGS4(float))	{ val = _mm_set_ps(VPASS4); }

		float4(const __m128& rhs)	{ val = rhs; }
		float4(const __m128i& rhs)	{ val = _mm_cvtepi32_ps(rhs); }
		float4(const __m128d& rhs)	{ val = _mm_cvtpd_ps(rhs); }

		float4(const float4&	rhs);
		float4(const int4&	rhs);
		float4(const double2&	rhs);		
	};
	
	inline auto ones()
	{
		auto t = _mm_setzero_si128();
		return _mm_cmpeq_epi32(t, t);
	}

	inline auto one()
	{
		return _mm_srli_epi32(ones(), 31);
	}

	inline auto sign1all0()
	{
		return _mm_slli_epi32(ones(), 31);
	}

	inline auto sign0all1()
	{
		return _mm_srli_epi32(ones(), 1);
	}

	// Arithmetic =====================================================================================================
	BIN_OP(float4, +) { BIN_BODY(_mm_add_ps); }
	BIN_OP(float4, -) { BIN_BODY(_mm_sub_ps); }
	BIN_OP(float4, *) { BIN_BODY(_mm_mul_ps); }
	BIN_OP(float4, / ) { BODY(_mm_mul_ps(a.val, _mm_rcp_ps(b.val))); }//*/ { BIN_BODY(_mm_div_ps); }

	UN_OP(float4, -) { BODY(_mm_xor_ps(a.val, _mm_castsi128_ps(sign1all0()))); }//*/return _mm_sub_ps(_mm_set1_ps(0.0), a.val); }

	// Comparison =====================================================================================================	
	BIN_OP(float4, > )	{ BIN_BODY(_mm_cmpgt_ps); }
	BIN_OP(float4, < )	{ BIN_BODY(_mm_cmplt_ps); }
	BIN_OP(float4, == ) { BIN_BODY(_mm_cmpeq_ps); }	
	
	// Bitwise ========================================================================================================
	UN_OP(float4, ~) { BODY(_mm_andnot_ps(a.val, _mm_castsi128_ps(ones()))); }

	BIN_OP(float4, &) { BIN_BODY(_mm_and_ps); }
	BIN_OP(float4, |) { BIN_BODY(_mm_or_ps);  }
	BIN_OP(float4, ^) { BIN_BODY(_mm_xor_ps); }
	
	// Special functions ==============================================================================================
	// Branchless select
	TRI_FUNC(float4, vsel) { TRI_BODY_R(_mm_blendv_ps); }
	// Fused multiply-add
	TRI_FUNC(float4, vfmadd) { TRI_BODY(_mm_fmadd_ps); }
	// Fused multipl-subtract
	TRI_FUNC(float4, vfmsub) { TRI_BODY(_mm_fmsub_ps); }

	// Mathematical functions =========================================================================================
	// Absolute
	UN_FUNC(float4, vabs) { BODY(_mm_and_ps(a.val, _mm_castsi128_ps(sign0all1()))); }
	// Minimum
	BIN_FUNC(float4, vmin)	{ BIN_BODY(_mm_min_ps); }
	// Maximum
	BIN_FUNC(float4, vmax)	{ BIN_BODY(_mm_max_ps); }	
	// Square root
	UN_FUNC(float4, vsqrt) { BODY(_mm_mul_ps(a.val, _mm_rsqrt_ps(a.val))); }//*/  { UN_BODY(_mm_sqrt_ps); }

	UN_FUNC(float4, vtrunc) { BODY(static_cast<float4>(static_cast<int4>(a))); }

	// Floor
	UN_FUNC(float4, vfloor)
	{
		auto fi = vtrunc(a);
		return vsel(fi > a, fi - static_cast<float4>(one()), fi);//- (fi > a) & j;
	}

	// Ceil
	UN_FUNC(float4, vceil)
	{
		auto fi = vtrunc(a);
		return vsel(fi < a, fi + static_cast<float4>(one()), fi);
	}

	// Round
	UN_FUNC(float4, vround)
	{
		auto v0 = _mm_setzero_ps();             //generate the highest value < 2
		auto v1 = _mm_cmpeq_ps(v0, v0);
		auto vNearest2 = *(__m128*)&_mm_srli_epi32(*(__m128i*)&v1, 2);
		auto i = _mm_cvttps_epi32(a.val);
		auto aTrunc = _mm_cvtepi32_ps(i);        // truncate a
		auto rmd = _mm_sub_ps(a.val, aTrunc);        // get remainder
		auto rmd2 = _mm_mul_ps(rmd, vNearest2); // mul remainder by near 2 will yield the needed offset
		auto rmd2i = _mm_cvttps_epi32(rmd2);    // after being truncated of course
		auto rmd2Trunc = _mm_cvtepi32_ps(rmd2i);
		auto r = _mm_add_ps(aTrunc, rmd2Trunc);

		return r;
	}	
}}

#endif