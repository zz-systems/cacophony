#pragma once
#ifndef PARANOISE_INTRINSIC_M256I_H
#define PARANOISE_INTRINSIC_M256I_H

#include "base.h"
#include "../capabiliy.h"

namespace paranoise {	namespace parallel {

	template<typename featuremask>
	struct float8;
	//struct double4;

#define _float8 float8<featuremask>
#define _int8 int8<featuremask>

	template<typename featuremask>
	struct ALIGN(32) int8 {
		__m256i val;
/*
		int64 i64[4];
		int32 i32[8];
		int16 i16[16];*/

		int8() = default;
		int8(const int32& rhs)		{ val = _mm256_set1_epi32(rhs); }

		int8(const __m256& rhs)		{ val = _mm256_cvtps_epi32(rhs); }
		int8(const __m256i& rhs)	{ val = rhs; }
		int8(const __m256d& rhs)	{ val = _mm256_castsi128_si256(_mm256_cvtpd_epi32(rhs)); }

		int8(const _float8&	rhs);
		int8(const _int8&	rhs);
		//int8(const double4&	rhs);
	};

	FEATURE_BIN_OP(+, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_add_epi32);
	}

	FEATURE_BIN_OP(-, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_sub_epi32);
	}

	FEATURE_BIN_OP(*, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_mul_epi32);
	}	

	FEATURE_UN_OP(-, _int8, _dispatcher::has_avx2)
	{
		BODY(_mm256_sub_epi32(_mm256_set1_epi32(0), a.val));
	}
	

	FEATURE_BIN_OP(>, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_cmpgt_epi32)
	}

	FEATURE_BIN_OP(<, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_cmpgt_epi32)
	}

	FEATURE_BIN_OP(==, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_cmpeq_epi32)
	}

	FEATURE_UN_OP(~, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_andnot_si256);
	}

	FEATURE_BIN_OP(|, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_or_si256);
	}

	FEATURE_BIN_OP(&, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_and_si256);
	}

	FEATURE_BIN_OP(^, _int8, _dispatcher::has_avx2)
	{
		BIN_BODY(_mm256_xor_si256);
	}
	

	FEATURE_SHIFT_OP(>> , _int8, _dispatcher::has_avx2) { return _mm256_srli_epi32(a.val, sa); }

	FEATURE_SHIFT_OP(<< , _int8, _dispatcher::has_avx2) { return _mm256_slli_epi32(a.val, sa); }
	
	
	FEATURE_BIN_FUNC(vmin, _int8, _dispatcher::has_avx2) { BIN_BODY(_mm256_min_epi32); }

	FEATURE_BIN_FUNC(vmax, _int8, _dispatcher::has_avx2) { BIN_BODY(_mm256_max_epi32); }

	// SSE 4.1 branchless select
	FEATURE_TRI_FUNC(vsel, _int8, _dispatcher::has_avx2)
	{
		TRI_BODY_R(_mm256_blendv_epi8);
	}
}}

#endif