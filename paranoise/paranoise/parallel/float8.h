#pragma once
#ifndef PARANOISE_INTRINSIC_M256F_H
#define PARANOISE_INTRINSIC_M256F_H

#include "base.h"

namespace paranoise {	namespace parallel {

	union int8;
	union double4;

	ALIGN(32) union float8 {
		__m256 val;
		float values[8];

		float8() = default;
		float8(const float& rhs)	{ val = _mm256_set1_ps(rhs); }

		float8(const uint8& _0, const uint8& _1, const uint8& _2, const uint8& _3, 
			const uint8& _4, const uint8& _5, const uint8& _6, const uint8& _7) { val = _mm256_cvtepi32_ps(_mm256_set_epi32(_7, _6, _5, _4,_3, _2, _1, _0)); }
		float8(const int32& _0, const int32& _1, const int32& _2, const int32& _3, 
			const int32& _4, const int32& _5, const int32& _6, const int32& _7){ val = _mm256_cvtepi32_ps(_mm256_set_epi32(_7, _6, _5, _4, _3, _2, _1, _0)); }
		float8(const float& _0, const float& _1, const float& _2, const float& _3,
			const float& _4, const float& _5, const float& _6, const float& _7) { val = _mm256_set_ps(_7, _6, _5, _4, _3, _2, _1, _0); }

		float8(const __m256& rhs)	{ val = rhs; }
		float8(const __m256i& rhs)	{ val = _mm256_cvtepi32_ps(rhs); }
		float8(const __m256d& rhs)	{ val = _mm256_castps128_ps256(_mm256_cvtpd_ps(rhs)); }

		float8(const float8&	rhs);
		float8(const int8&	rhs);
		float8(const double4&	rhs);
	};

	inline float8 operator +(const float8& a, const float8& b) { return _mm256_add_ps		(a.val, b.val); }
	inline float8 operator -(const float8& a, const float8& b) { return _mm256_sub_ps		(a.val, b.val); }
	inline float8 operator *(const float8& a, const float8& b) { return _mm256_mul_ps		(a.val, b.val); }
	inline float8 operator /(const float8& a, const float8& b) { return _mm256_div_ps		(a.val, b.val); }

	inline float8 operator >(const float8& a, const float8& b) { return _mm256_cmp_ps		(a.val, b.val, _CMP_GT_OQ); }
	inline float8 operator <(const float8& a, const float8& b) { return _mm256_cmp_ps		(a.val, b.val, _CMP_LT_OQ); }
	inline float8 operator==(const float8& a, const float8& b) { return _mm256_cmp_ps		(a.val, b.val, _CMP_EQ_OQ); }

	inline float8 operator -(const float8& a)				   { return _mm256_sub_ps(_mm256_set1_ps(0.0), a.val); }
	inline float8 operator ~(const float8& a)				   { return _mm256_andnot_ps(a.val, a.val); }
	inline float8 operator |(const float8& a, const float8& b) { return _mm256_or_ps		(a.val, b.val); }
	inline float8 operator &(const float8& a, const float8& b) { return _mm256_and_ps		(a.val, b.val); }
	inline float8 operator ^(const float8& a, const float8& b) { return _mm256_xor_ps		(a.val, b.val); }
	

	
	inline float8 min(const float8& a, const float8& b)		{ return _mm256_min_ps		(a.val, b.val); }
	inline float8 max(const float8& a, const float8& b)		{ return _mm256_max_ps		(a.val, b.val); }	

	inline float8 sqrt(const float8& a)						{ return _mm256_sqrt_ps		(a.val); }


	inline float8 floor(const float8& a) {
		auto v0 = _mm256_setzero_si256();
		auto v1 = _mm256_cmpeq_epi32(v0, v0);
		auto ji = _mm256_srli_epi32(v1, 25);
		auto j = *(__m256*)&_mm256_slli_epi32(ji, 23); //create vector 1.0f
		auto i = _mm256_cvttps_epi32(a.val);
		auto fi = _mm256_cvtepi32_ps(i);
		auto igx = _mm256_cmp_ps(fi, a.val, _CMP_GT_OS);
		j = _mm256_and_ps(igx, j);
		return _mm256_sub_ps(fi, j);
	}

	inline float8 ceil(const float8& a) {
		auto v0 = _mm256_setzero_si256();
		auto v1 = _mm256_cmpeq_epi32(v0, v0);
		auto ji = _mm256_srli_epi32(v1, 25);
		auto j = *(__m256*)&_mm256_slli_epi32(ji, 23); //create vector 1.0f
		auto i = _mm256_cvttps_epi32(a.val);
		auto fi = _mm256_cvtepi32_ps(i);
		auto igx = _mm256_cmp_ps(fi, a.val, _CMP_LT_OS);
		j = _mm256_and_ps(igx, j);
		return _mm256_add_ps(fi, j);
	}

	inline float8 round(const float8 &a) {
		auto v0 = _mm256_setzero_ps();             //generate the highest value &lt; 2
		auto v1 = _mm256_cmp_ps(v0, v0, _CMP_EQ_OS);
		auto vNearest2 = *(__m256*)&_mm256_srli_epi32(*(__m256i*)&v1, 2);
		auto i = _mm256_cvttps_epi32(a.val);
		auto aTrunc = _mm256_cvtepi32_ps(i);        // truncate a
		auto rmd = _mm256_sub_ps(a.val, aTrunc);        // get remainder
		auto rmd2 = _mm256_mul_ps(rmd, vNearest2); // mul remainder by near 2 will yield the needed offset
		auto rmd2i = _mm256_cvttps_epi32(rmd2);    // after being truncated of course
		auto rmd2Trunc = _mm256_cvtepi32_ps(rmd2i);
		auto r = _mm256_add_ps(aTrunc, rmd2Trunc);
		return r;
	}

	inline float8 sel(const float8& mask, const float8 &a, const float8 &b)
	{
		return _mm256_blendv_ps(b.val, a.val, mask.val);
	}
}}

#endif