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
		float4(const float& rhs)		{ val = _mm_set_ps1(rhs); }

		float4(const float& _0, const float& _1, const float& _2, const float& _3) { val = _mm_set_ps(_0, _1, _2, _3); }

		float4(const __m128& rhs)	{ val = rhs; }
		float4(const __m128i& rhs)	{ val = _mm_cvtepi32_ps(rhs); }
		float4(const __m128d& rhs)	{ val = _mm_cvtpd_ps(rhs); }

		float4(const float4&	rhs);
		float4(const int4&	rhs);
		float4(const double2&	rhs);		
	};

	inline float4 operator +(const float4& a, const float4& b) { return _mm_add_ps		(a.val, b.val); }
	inline float4 operator -(const float4& a, const float4& b) { return _mm_sub_ps		(a.val, b.val); }
	inline float4 operator *(const float4& a, const float4& b) { return _mm_mul_ps		(a.val, b.val); }
	inline float4 operator /(const float4& a, const float4& b) { return _mm_div_ps		(a.val, b.val); }

	inline float4 operator >(const float4& a, const float4& b) { return _mm_cmpgt_ps	(a.val, b.val); }
	inline float4 operator <(const float4& a, const float4& b) { return _mm_cmplt_ps	(a.val, b.val); }	
	inline float4 operator==(const float4& a, const float4& b) { return _mm_cmpeq_ps	(a.val, b.val); }

	inline float4 operator ~(const float4& a)				   { return _mm_andnot_ps	(a.val, a.val); }
	inline float4 operator &(const float4& a, const float4& b) { return _mm_and_ps		(a.val, b.val); }
	inline float4 operator |(const float4& a, const float4& b) { return _mm_or_ps		(a.val, b.val); }
	inline float4 operator ^(const float4& a, const float4& b) { return _mm_xor_ps		(a.val, b.val); }
	

	inline float4		abs(const float4& a) {
		static const __m128 sign_mask = _mm_set_ps1(-0.f); // -0.f = 1 << 31
		return _mm_andnot_ps(sign_mask, a.val);
	}

	inline float4 min(const float4& a, const float4& b)		{ return _mm_min_ps		(a.val, b.val); }
	inline float4 max(const float4& a, const float4& b)		{ return _mm_max_ps		(a.val, b.val); }	

	inline float4 sqrt(const float4& a)						{ return _mm_sqrt_ps	(a.val); }

	
}}

#endif