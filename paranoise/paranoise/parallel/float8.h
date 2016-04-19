#pragma once
#ifndef PARANOISE_INTRINSIC_M256F_H
#define PARANOISE_INTRINSIC_M256F_H

#include "dependencies.h"

namespace zzsystems { namespace simdal {

	template<typename featuremask>
	struct int8;
	//struct double4;

#define _float8 float8<featuremask>
#define _int8 int8<featuremask>

	template<typename featuremask>
	struct ALIGN(32) float8 {
		__m256 val;

		float8() = default;

		float8(const int rhs)		: val(_mm256_set1_ps(static_cast<float>(rhs))) {}
		float8(const float rhs)		: val(_mm256_set1_ps(rhs)) {}
		float8(const double rhs)	: val(_mm256_set1_ps(static_cast<float>(rhs))) {}

		float8(const float* rhs)	: val(_mm256_load_ps(rhs)) {}

		float8(VARGS8(uint8_t))		: val(_mm256_cvtepi32_ps(_mm256_set_epi32(VPASS8))) {}
		float8(VARGS8(int32_t))			: val(_mm256_cvtepi32_ps(_mm256_set_epi32(VPASS8))) {}
		float8(VARGS8(float))		: val(_mm256_set_ps(VPASS8)) {}

		float8(const __m256& rhs)	: val(rhs) {}
		float8(const __m256i& rhs)	: val(_mm256_cvtepi32_ps(rhs)) {}
		float8(const __m256d& rhs)	: val(_mm256_castps128_ps256(_mm256_cvtpd_ps(rhs))) {}

		float8(const _float8&	rhs);
		float8(const _int8&	rhs);
		float8(const _int4x2& rhs);
		//float8(const double4&	rhs);

		BIN_OP_STUB(+, _float8, float)
		BIN_OP_STUB(-, _float8, float)
		BIN_OP_STUB(*, _float8, float)
		BIN_OP_STUB(/, _float8, float)

		BIN_OP_STUB(&, _float8, float)
		BIN_OP_STUB(&&, _float8, float)
		BIN_OP_STUB(|, _float8, float)
		BIN_OP_STUB(||, _float8, float)
		BIN_OP_STUB(^, _float8, float)
		
		BIN_OP_STUB(>, _float8, float)
		BIN_OP_STUB(<, _float8, float)
		BIN_OP_STUB(== , _float8, float)


		explicit inline operator bool()
		{
			return _mm256_test_all_ones(_mm256_castps_si256(this->val));
		}

		static inline auto ones()
		{
			auto t = _mm256_setzero_si256();
			return _mm256_cmpeq_epi32(t, t);
		}

		static inline auto one()
		{
			return _mm256_srli_epi32(ones(), 31);
		}

		static inline auto sign1all0()
		{
			return _mm256_slli_epi32(ones(), 31);
		}

		static inline auto sign0all1()
		{
			return _mm256_srli_epi32(ones(), 1);
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
		BODY(_mm256_xor_ps(a.val, _mm256_cmp_ps(_mm256_setzero_ps(), _mm256_setzero_ps(), _CMP_EQ_OQ)));
	}

	FEATURE_BIN_OP(|, _float8, _dispatcher::has_avx)
	{
		BIN_BODY(_mm256_or_ps); 
	}

	FEATURE_BIN_OP(&, _float8, _dispatcher::has_avx)
	{
		BIN_BODY(_mm256_and_ps);
	}

	FEATURE_UN_OP(!, _float8, _dispatcher::has_avx)
	{
		BODY(~a);
	}

	FEATURE_BIN_OP(|| , _float8, _dispatcher::has_avx)
	{
		BIN_BODY(_mm256_or_ps);
	}

	FEATURE_BIN_OP(&&, _float8, _dispatcher::has_avx)
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
		BODY(_mm256_and_ps(a.val, _mm256_castsi256_ps(_float8::sign0all1())));
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
	FEATURE_UN_FUNC(vtrunc, _float8, _dispatcher::has_avx)
	{
		BODY(_mm256_round_ps(a.val, _MM_FROUND_TO_ZERO));
	}

	// Floor value
	FEATURE_UN_FUNC(vfloor, _float8, _dispatcher::has_avx)
	{
		BODY(_mm256_round_ps(a.val, _MM_FROUND_TO_NEG_INF));		
	}

	// Ceil value
	FEATURE_UN_FUNC(vceil, _float8, _dispatcher::has_avx)
	{
		BODY(_mm256_round_ps(a.val, _MM_FROUND_TO_POS_INF));
	}

	// Round value
	FEATURE_UN_FUNC(vround, _float4, _dispatcher::has_avx)
	{
		BODY(_mm256_round_ps(a.val, _MM_FROUND_TO_NEAREST_INT));
	}
}}

#endif