#pragma once
#ifndef PARANOISE_INTRINSIC_M256D_H
#define PARANOISE_INTRINSIC_M256D_H

#include "base.h"

namespace paranoise {	namespace intrinsic {

	union m256f;
	union m256i;

	ALIGN(32) union m256d {
		__m256d val;
		double values[4];

		m256d() = default;
		m256d(const float& rhs)		{ val = _mm256_set1_pd(rhs); }

		m256d(const __m256d& rhs)	{ val = rhs; }
		m256d(const __m256i& rhs)	{ val = _mm256_castsi256_pd(rhs); }
		m256d(const __m256& rhs)	{ val = _mm256_castps_pd(rhs); }

		m256d(const m256f&	rhs);
		m256d(const m256i&	rhs);
		m256d(const m256d&	rhs);

		/*m256d &operator =(const m256i &rhs);
		m256d &operator =(const m256f &rhs);
		m256d &operator =(const m256d &rhs);*/

		explicit operator double*() { return values; }
	};

	inline m256d operator +(const m256d& a, const m256d& b) { return _mm256_add_pd		(a.val, b.val); }
	inline m256d operator -(const m256d& a, const m256d& b) { return _mm256_sub_pd		(a.val, b.val); }
	inline m256d operator *(const m256d& a, const m256d& b) { return _mm256_mul_pd		(a.val, b.val); }
	inline m256d operator /(const m256d& a, const m256d& b) { return _mm256_div_pd		(a.val, b.val); }

	inline m256d operator >(const m256d& a, const m256d& b) { return _mm256_cmp_pd		(a.val, b.val, _CMP_GT_OQ); }
	inline m256d operator <(const m256d& a, const m256d& b) { return _mm256_cmp_pd		(a.val, b.val, _CMP_LT_OQ); }	
	inline m256d operator==(const m256d& a, const m256d& b) { return _mm256_cmp_pd		(a.val, b.val, _CMP_EQ_OQ); }

	inline m256d operator |(const m256d& a, const m256d& b) { return _mm256_or_pd		(a.val, b.val); }
	inline m256d operator &(const m256d& a, const m256d& b) { return _mm256_and_pd		(a.val, b.val); }
	inline m256d operator ^(const m256d& a, const m256d& b) { return _mm256_xor_pd		(a.val, b.val); }
	inline m256d operator ~(const m256d& a)				    { return _mm256_andnot_pd	(a.val, a.val); }


	inline m256d min(const m256d& a, const m256d& b)		{ return _mm256_min_pd		(a.val, b.val); }
	inline m256d max(const m256d& a, const m256d& b)		{ return _mm256_max_pd		(a.val, b.val); }

	inline m256d sqrt(const m256d& a)						{ return _mm256_sqrt_pd		(a.val); }
}}

#endif