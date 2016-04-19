#pragma once
#ifndef PARANOISE_INTRINSIC_M128Ix2_H
#define PARANOISE_INTRINSIC_M128Ix2_H

#include "dependencies.h"
#include "int4.h"

namespace zzsystems { namespace simdal {

	template<typename featuremask>
	struct float8;
	//struct double4;

#define _float8 float8<featuremask>
#define _int4x2 int4x2<featuremask>

	template<typename featuremask>
	struct ALIGN(32) int4x2 {
		_int4 lo, hi;

		int4x2() = default;
		int4x2(const int32_t rhs)		: lo(rhs), hi(rhs) {}

		int4x2(VARGS8(uint32_t))		: lo(VPASS4), hi(VPASS4_HI) {}
		int4x2(VARGS8(int))				: lo(VPASS4), hi(VPASS4_HI) {}
		int4x2(VARGS8(float))			: lo(VPASS4), 
										  hi(VPASS4_HI) {}

		int4x2(const __m256& rhs)		: lo(_mm256_extractf128_ps(rhs, 0)),
										  hi(_mm256_extractf128_ps(rhs, 1)) 
		{}

		int4x2(const __m128i& rhs_hi, const __m128i& rhs_lo)	: lo(rhs_lo), hi(rhs_hi) {}
		int4x2(const __m256d& rhs)	:	lo(_mm256_extractf128_pd(rhs, 0)),
										hi(_mm256_extractf128_pd(rhs, 1)) {}

		int4x2(const _float8&	rhs);
		int4x2(const _int4x2&	rhs);
		int4x2(const _int4&	rhs_hi, const _int4& rhs_lo);

		//int4x2(const double4&	rhs);

		BIN_OP_STUB(+, _int4x2, int)
		BIN_OP_STUB(-, _int4x2, int)
		BIN_OP_STUB(*, _int4x2, int)
		BIN_OP_STUB(/ , _int4x2, int)

		BIN_OP_STUB(&, _int4x2, int)
		BIN_OP_STUB(&&, _int4x2, int)
		BIN_OP_STUB(|, _int4x2, int)
		BIN_OP_STUB(||, _int4x2, int)
		BIN_OP_STUB(^, _int4x2, int)

		BIN_OP_STUB(>, _int4x2, int)
		BIN_OP_STUB(<, _int4x2, int)
		BIN_OP_STUB(== , _int4x2, int)

		explicit inline operator bool() const
		{
			return static_cast<bool>(hi) && static_cast<bool>(lo);
		}
	};	

	FEATURE_BIN_OP(+, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return { a.hi + b.hi, a.lo + b.lo };
	}

	FEATURE_BIN_OP(-, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return { a.hi - b.hi, a.lo - b.lo };
	}

	FEATURE_BIN_OP(*, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return { a.hi * b.hi, a.lo * b.lo };
	}	

	FEATURE_UN_OP(-, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return { -a.hi, -a.lo };
	}
	
	FEATURE_BIN_OP(>, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ a.hi > b.hi, a.lo > b.lo };
	}

	FEATURE_BIN_OP(<, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ a.hi < b.hi, a.lo < b.lo };
	}

	FEATURE_BIN_OP(==, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ a.hi == b.hi, a.lo == b.lo };
	}

	FEATURE_UN_OP(~, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ ~a.hi, ~a.lo };
	}

	FEATURE_BIN_OP(|, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return { a.hi | b.hi, a.lo | b.lo };
	}

	FEATURE_BIN_OP(&, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return { a.hi & b.hi, a.lo & b.lo };
	}

	FEATURE_UN_OP(!, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		BODY(~a);
	}

	FEATURE_BIN_OP(|| , _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return a | b;
	}

	FEATURE_BIN_OP(&&, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return a & b;
	}

	FEATURE_BIN_OP(^, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ a.hi ^ b.hi, a.lo ^ b.lo };
	}
	

	FEATURE_SHIFT_OP(>> , _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return { a.hi >> sa, a.lo >> sa };
	}

	FEATURE_SHIFT_OP(<< , _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ a.hi << sa, a.lo << sa };
	}	
	
	FEATURE_BIN_FUNC(vmin, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ vmin(a.hi, b.hi), vmin(a.lo, b.lo) };
	}

	FEATURE_BIN_FUNC(vmax, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{
		return{ vmax(a.hi, b.hi), vmax(a.lo, b.lo) };
	}

	// AVX2 branchless select
	FEATURE_TRI_FUNC(vsel, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
	{		
		return{ vsel(a.hi, b.hi, c.hi), vsel(a.lo, b.lo, c.lo) };
	}
}}

#endif