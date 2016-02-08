#pragma once
#ifndef PARANOISE_INTRINSIC_M128F_H
#define PARANOISE_INTRINSIC_M128F_H

#include "base.h"

namespace paranoise { namespace parallel {
	union int4;
	union double2;

	ALIGN(16) union float4 {
		__m128 val;
		float values[4];

		float4() = default;
		float4(const float& rhs) : val(_mm_set1_ps(rhs)) {} //val(_mm_set_ps1(rhs)) {}

		float4(const uint8& _0, const uint8& _1, const uint8& _2, const uint8& _3)	{ val = _mm_cvtepi32_ps(_mm_set_epi32(_3, _2, _1, _0)); }
		float4(const int32& _0, const int32& _1, const int32& _2, const int32& _3)	{ val = _mm_cvtepi32_ps(_mm_set_epi32(_3, _2, _1, _0)); }
		float4(const float& _0, const float& _1, const float& _2, const float& _3)	{ val = _mm_set_ps(_3, _2, _1, _0); }

		float4(const __m128& rhs)	{ val = rhs; }
		float4(const __m128i& rhs)	{ val = _mm_cvtepi32_ps(rhs); }
		float4(const __m128d& rhs)	{ val = _mm_cvtpd_ps(rhs); }

		float4(const float4&	rhs);
		float4(const int4&	rhs);
		float4(const double2&	rhs);		
	};
	

	BINOP(float4, +) { STDBDY(_mm_add_ps); }
	BINOP(float4, -) { STDBDY(_mm_sub_ps); }
	BINOP(float4, *) { STDBDY(_mm_mul_ps); }
	BINOP(float4, /) { STDBDY(_mm_div_ps); }

	BINOP(float4, > ) {	STDBDY(_mm_cmpgt_ps); }
	BINOP(float4, < ) {	STDBDY(_mm_cmplt_ps); }
	BINOP(float4, == ) { STDBDY(_mm_cmpeq_ps); }

	UNOP(float4, -) { return _mm_sub_ps(_mm_set1_ps(0.0), a.val); }
	
	UNOP(float4, ~) { return _mm_andnot_ps(a.val, _mm_castsi128_ps(_mm_set1_epi32(0xFFFFFFFF))); }

	BINOP(float4, &) { STDBDY(_mm_and_ps); }
	BINOP(float4, |) { STDBDY(_mm_or_ps);  }
	BINOP(float4, ^) { STDBDY(_mm_xor_ps); }
	
	UNFUNC(float4, abs) {
		static const __m128 sign_mask = _mm_set_ps1(-0.f); // -0.f = 1 << 31
		return _mm_andnot_ps(sign_mask, a.val);
	}

	BINFUNC(float4, min)	{ STDBDY(_mm_min_ps); }
	BINFUNC(float4, max)	{ STDBDY(_mm_max_ps); }	

	UNFUNC(float4, sqrt)	{ STDBDY1(_mm_sqrt_ps); }

	inline float4 sel(const float4& mask, const float4 &a, const float4 &b)	{ return _mm_blendv_ps(b.val, a.val, mask.val);	}
	inline float4 fmadd(const float4 &a, const float4 &b, const float4 &c) { return _mm_fmadd_ps(a.val, b.val, c.val); }	
	inline float4 fmsub(const float4 &a, const float4 &b, const float4 &c) { return _mm_fmsub_ps(a.val, b.val, c.val); }

	UNFUNC(float4, floor) 
	{
		auto fi = (float4)(int4)a;

		return sel(fi > a, fi - one<float4>(), fi);//- (fi > a) & j;
	}

	UNFUNC(float4, ceil)
	{
		auto fi = (float4)(int4)a;

		return sel(fi < a, fi + one<float4>(), fi);
	}

	UNFUNC(float4, round)
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