#pragma once
#ifndef NOISEPP_INTRINSIC_M256F_H
#define NOISEPP_INTRINSIC_M256F_H

#include "base.h"

namespace noisepp {	namespace intrinsic {

	union m256i;
	union m256d;

	ALIGN(32) union m256f {
		__m256 val;
		float values[8];

		m256f() = default;
		m256f(const float& rhs)	{ val = _mm256_set1_ps(rhs); }

		m256f(const __m256& rhs)	{ val = rhs; }
		m256f(const __m256i& rhs) { val = _mm256_cvtepi32_ps(rhs); }
		m256f(const __m256d& rhs) { val = _mm256_castpd_ps(rhs); }

		m256f(const m256f&	rhs);
		m256f(const m256i&	rhs);
		m256f(const m256d&	rhs);

		explicit operator float*() { return values; }
	};

	inline m256f		operator +(const m256f& a, const m256f& b) { return _mm256_add_ps	(a.val, b.val); }
	inline m256f		operator -(const m256f& a, const m256f& b) { return _mm256_sub_ps	(a.val, b.val); }
	inline m256f		operator *(const m256f& a, const m256f& b) { return _mm256_mul_ps	(a.val, b.val); }
	inline m256f		operator /(const m256f& a, const m256f& b) { return _mm256_div_ps	(a.val, b.val); }

	inline m256f		operator >(const m256f& a, const m256f& b) { return _mm256_cmp_ps	(a.val, b.val, _CMP_GT_OQ); }
	inline m256f		operator <(const m256f& a, const m256f& b) { return _mm256_cmp_ps	(a.val, b.val, _CMP_LT_OQ); }

	inline m256f		operator |(const m256f& a, const m256f& b) { return _mm256_or_ps	(a.val, b.val); }
	inline m256f		operator &(const m256f& a, const m256f& b) { return _mm256_and_ps	(a.val, b.val); }
	inline m256f		operator ^(const m256f& a, const m256f& b) { return _mm256_xor_ps	(a.val, b.val); }
	inline m256f		operator ~(const m256f& a)				   { return _mm256_andnot_ps(a.val, a.val); }

	
	inline m256f min(const m256f& a, const m256f& b)				{ return _mm256_min_ps	(a.val, b.val); }
	inline m256f max(const m256f& a, const m256f& b)				{ return _mm256_max_ps	(a.val, b.val); }	

	inline m256f sqrt(const m256f& a)								{ return _mm256_sqrt_ps	(a.val); }
}}

#endif