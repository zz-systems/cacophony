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
}}

#endif