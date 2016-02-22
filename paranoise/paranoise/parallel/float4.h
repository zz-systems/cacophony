#pragma once
#ifndef PARANOISE_INTRINSIC_M128F_H
#define PARANOISE_INTRINSIC_M128F_H

#include "base.h"
#include "../capabiliy.h"

namespace paranoise { namespace parallel {
	template<typename featuremask>
	struct int4;

	
	//struct double2;
#define _float4 float4<featuremask> 
#define _int4 int4<featuremask>

	template<typename featuremask>
	struct ALIGN(16) float4 {
		__m128 val;		

		float4() = default;
		float4(const float rhs) {
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

		float4(const _float4&	rhs);
		float4(const _int4&	rhs);
		//float4(const double2&	rhs);


		BIN_OP_STUB(+, _float4, float)
		BIN_OP_STUB(-, _float4, float)
		BIN_OP_STUB(*, _float4, float)
		BIN_OP_STUB(/ , _float4, float)		

		BIN_OP_STUB(>, _float4, float)
		BIN_OP_STUB(<, _float4, float)
		BIN_OP_STUB(== , _float4, float)
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
	
	// Add
	FEATURE_BIN_OP(+, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_add_ps);
	}	
	// Sub
	FEATURE_BIN_OP(-, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_sub_ps);
	}
	// Mul
	FEATURE_BIN_OP(*, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_mul_ps);
	}
	// Fast division
	FEATURE_BIN_OP(/, _float4, _dispatcher::has_sse)
	{
		BODY(_mm_mul_ps(a.val, _mm_rcp_ps(b.val))); //*/ { BIN_BODY(_mm_div_ps); }	
	}
	// Negate 
	FEATURE_UN_OP(-, _float4, _dispatcher::has_sse)
	{
		BODY(_mm_xor_ps(a.val, _mm_castsi128_ps(sign1all0())));
	}

	// Comparison =====================================================================================================	
	// Greater than
	FEATURE_BIN_OP(>, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_cmpgt_ps);
	}
	// Less than
	FEATURE_BIN_OP(<, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_cmplt_ps);
	}
	//SIMD_FEATURE(_float4::has_sse)
	FEATURE_BIN_OP(==, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_cmpeq_ps);
	}
	
	// Bitwise ========================================================================================================
	// Bitwise NOT
	FEATURE_UN_OP(~, _float4, _dispatcher::has_sse)
	{
		BODY(_mm_andnot_ps(a.val, _mm_castsi128_ps(ones())));
	}

	// Bitwise AND
	FEATURE_BIN_OP(&, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_and_ps);
	}

	// Bitwise OR
	FEATURE_BIN_OP(|, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_or_ps);
	}

	// Bitwise XOR
	FEATURE_BIN_OP(^, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_xor_ps);
	}
	
	// Special functions ==============================================================================================
	// SSE > 4.1 Branchless select
	FEATURE_TRI_FUNC(vsel, _float4, _dispatcher::has_sse41)
	{
		TRI_BODY_R(_mm_blendv_ps);
	}

	// SSE < 4.1 branchless select
	FEATURE_TRI_FUNC(vsel, _float4, !_dispatcher::has_sse41 && _dispatcher::has_sse)
	{
		BODY(a /* mask */ & b | ~a & c);
	}

	// Fused multiply-add
	FEATURE_TRI_FUNC(vfmadd, _float4, _dispatcher::has_fma)
	{
		TRI_BODY(_mm_fmadd_ps);
	}
	// Fused multipl-subtract
	FEATURE_TRI_FUNC(vfmsub, _float4, _dispatcher::has_fma)
	{
		TRI_BODY(_mm_fmsub_ps);
	}

	// Mathematical functions =========================================================================================
	// Absolute value
	FEATURE_UN_FUNC(vabs, _float4, _dispatcher::has_sse)
	{
		// According to IEEE 754 standard: sign bit is the first bit => set to 0
		BODY(_mm_and_ps(a.val, _mm_castsi128_ps(sign0all1())));
	}

	// Minimum value
	FEATURE_BIN_FUNC(vmin, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_min_ps);
	}

	// Maximum
	FEATURE_BIN_FUNC(vmax, _float4, _dispatcher::has_sse)
	{
		BIN_BODY(_mm_max_ps);
	}

	// Fast square root
	FEATURE_UN_FUNC(vsqrt, _float4, _dispatcher::has_sse)
	{
		BODY(_mm_mul_ps(a.val, _mm_rsqrt_ps(a.val)));
	}//*/  { UN_BODY(_mm_sqrt_ps); }
		
	// Rounding =======================================================================================================

	// Truncate float to *.0
	FEATURE_UN_FUNC(vtrunc, _float4, _dispatcher::has_sse)
	{
		BODY(static_cast<_float4>(static_cast<int4<featuremask>>(a)));
	}

	// Floor value
	FEATURE_UN_FUNC(vfloor, _float4, _dispatcher::has_sse)
	{
		auto fi = vtrunc(a);
		return vsel(fi > a, fi - static_cast<_float4>(one()), fi);
	}

	// Ceil value
	FEATURE_UN_FUNC(vceil, _float4, _dispatcher::has_sse)
	{
		auto fi = vtrunc(a);
		return vsel(fi < a, fi + static_cast<_float4>(one()), fi);
	}

	// Round value
	FEATURE_UN_FUNC(vround, _float4, _dispatcher::has_sse)
	{
		//generate the highest value < 2		
		auto vNearest2 = _mm_castsi128_ps(_mm_srli_epi32(ones(), 2));
		auto aTrunc = vtrunc(a);

		auto rmd = a - aTrunc;        // get remainder
		auto rmd2 = rmd * vNearest2;  // mul remainder by near 2 will yield the needed offset

		auto rmd2Trunc = vtrunc(rmd2); // after being truncated of course

		return aTrunc + rmd2Trunc;
	}	
		
	
}}

#endif