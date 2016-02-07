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

	inline float4 operator +(const float4& a, const float4& b) { return _mm_add_ps		(a.val, b.val); }
	inline float4 operator -(const float4& a, const float4& b) { return _mm_sub_ps		(a.val, b.val); }
	inline float4 operator *(const float4& a, const float4& b) { return _mm_mul_ps		(a.val, b.val); }
	inline float4 operator /(const float4& a, const float4& b) { return _mm_div_ps		(a.val, b.val); }

	inline float4 operator >(const float4& a, const float4& b) 
	{
		auto t = _mm_cmpgt_ps(a.val, b.val); 
		return t; 
	}
	inline float4 operator <(const float4& a, const float4& b) { return _mm_cmplt_ps	(a.val, b.val); }	
	inline float4 operator==(const float4& a, const float4& b) { return _mm_cmpeq_ps	(a.val, b.val); }

	inline float4 operator -(const float4& a)				   { return _mm_sub_ps		(_mm_set1_ps(0.0), a.val); }
	inline float4 operator ~(const float4& a) { 
		return _mm_andnot_ps(a.val, _mm_castsi128_ps(_mm_set1_epi32(0xFFFFFFFF)));
	}
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

	inline float4 floor(const float4& a) {
		auto v0 = _mm_setzero_si128();
		auto v1 = _mm_cmpeq_epi32(v0, v0);
		auto ji = _mm_srli_epi32(v1, 25);
		auto j = *(__m128*)&_mm_slli_epi32(ji, 23); //create vector 1.0f
		auto i = _mm_cvttps_epi32(a.val);
		auto fi = _mm_cvtepi32_ps(i);
		auto igx = _mm_cmpgt_ps(fi, a.val);
		j = _mm_and_ps(igx, j);
		return _mm_sub_ps(fi, j);
	}

	inline float4 ceil(const float4& a) {
		auto v0 = _mm_setzero_si128();
		auto v1 = _mm_cmpeq_epi32(v0, v0);
		auto ji = _mm_srli_epi32(v1, 25);
		auto j = *(__m128*)&_mm_slli_epi32(ji, 23); //create vector 1.0f
		auto i = _mm_cvttps_epi32(a.val);
		auto fi = _mm_cvtepi32_ps(i);
		auto igx = _mm_cmplt_ps(fi, a.val);
		j = _mm_and_ps(igx, j);
		return _mm_add_ps(fi, j);
	}

	inline float4 round(const float4 &a) {
		auto v0 = _mm_setzero_ps();             //generate the highest value &lt; 2
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

	inline float4 sel(const float4& mask, const float4 &a, const float4 &b)
	{
		auto t = _mm_blendv_ps(b.val, a.val, mask.val);
		return t;
	}
}}

#endif