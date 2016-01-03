#pragma once
#ifndef PARANOISE_INTRINSIC_M128D_H
#define PARANOISE_INTRINSIC_M128D_H

#include "base.h"

namespace paranoise { namespace parallel {
	union m128f;
	union m128i;

	ALIGN(16) union m128d {
		__m128d val;
		double values[2];

		m128d() = default;
		m128d(const double& rhs)	{ val = _mm_set1_pd(rhs); }

		m128d(const __m128& rhs)	{ val = _mm_cvtps_pd(rhs); }
		m128d(const __m128i& rhs)	{ val = _mm_cvtepi32_pd(rhs); }
		m128d(const __m128d& rhs)	{ val = rhs; }

		m128d(const m128f& rhs);
		m128d(const m128i& rhs);
		m128d(const m128d& rhs);
	};

	inline m128d operator +(const m128d& a, const m128d& b) { return _mm_add_pd		(a.val, b.val); }
	inline m128d operator -(const m128d& a, const m128d& b) { return _mm_sub_pd		(a.val, b.val); }
	inline m128d operator *(const m128d& a, const m128d& b) { return _mm_mul_pd		(a.val, b.val); }
	inline m128d operator /(const m128d& a, const m128d& b) { return _mm_div_pd		(a.val, b.val); }

	inline m128d operator >(const m128d& a, const m128d& b) { return _mm_cmpgt_pd	(a.val, b.val); }
	inline m128d operator <(const m128d& a, const m128d& b) { return _mm_cmplt_pd	(a.val, b.val); }
	inline m128d operator==(const m128d& a, const m128d& b) { return _mm_cmpeq_pd	(a.val, b.val); }

	inline m128d operator &(const m128d& a, const m128d& b) { return _mm_and_pd		(a.val, b.val); }	
	inline m128d operator ~(const m128d& a)					{ return _mm_andnot_pd	(a.val, a.val); }

	inline m128d abs(const m128d& a) {
		static const __m128d sign_mask = _mm_set1_pd(-0.); // -0. = 1 << 63
		return _mm_andnot_pd(sign_mask, a.val); // !sign_mask & x
	}

	inline m128d min(const m128d& a, const m128d& b)		{ return _mm_min_pd		(a.val, b.val);	}
	inline m128d max(const m128d& a, const m128d& b)		{ return _mm_max_pd		(a.val, b.val);	}	

	inline m128d sqrt(const m128d& a)						{ return _mm_sqrt_pd	(a.val); }

	//inline m128d	adjustoToInt32(const m128d& a)					 { return _mm_cvtepi32_ps(_mm_cvtps_epi32(a.val)); }
}}

#endif