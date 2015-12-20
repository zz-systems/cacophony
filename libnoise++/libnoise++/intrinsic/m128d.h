#pragma once
#ifndef NOISEPP_INTRINSIC_M128D_H
#define NOISEPP_INTRINSIC_M128D_H

#include "base.h"

namespace noisepp {	namespace intrinsic {
	union m128f;
	union m128i;

	ALIGN(16) union m128d {
		__m128d val;
		double values[2];

		m128d() = default;
		m128d(const double& rhs)	{ val = _mm_set1_pd(rhs); }

		m128d(const __m128& rhs)	{ val = _mm_cvtps_pd(rhs); }
		m128d(const __m128i& rhs) { val = _mm_cvtepi32_pd(rhs); }
		m128d(const __m128d& rhs) { val = rhs; }

		m128d(const m128f& rhs);
		m128d(const m128i& rhs);
		m128d(const m128d& rhs);

		explicit operator double*() { return values; }
	};

	inline m128d	operator +(const m128d& one, const m128d& other) { return _mm_add_pd(one.val, other.val); }
	inline m128d	operator -(const m128d& one, const m128d& other) { return _mm_sub_pd(one.val, other.val); }
	inline m128d	operator *(const m128d& one, const m128d& other) { return _mm_mul_pd(one.val, other.val); }
	inline m128d	operator /(const m128d& one, const m128d& other) { return _mm_div_pd(one.val, other.val); }

	inline m128d	operator >(const m128d& one, const m128d& other) { return _mm_cmpgt_pd(one.val, other.val); }

	inline m128d	operator &(const m128d& one, const m128d& other) { return _mm_and_pd(one.val, other.val); }	
	inline m128d	operator ~(const m128d& a)						 { return _mm_andnot_pd(a.val, a.val); }

	inline m128d	abs(const m128d& a) {
		static const __m128d sign_mask = _mm_set1_pd(-0.); // -0. = 1 << 63
		return _mm_andnot_pd(sign_mask, a.val); // !sign_mask & x
	}

	inline m128d min(const m128d& a, const m128d& b)
	{
		return _mm_min_pd(a.val, b.val);
	}

	inline m128d max(const m128d& a, const m128d& b)
	{
		return _mm_max_pd(a.val, b.val);
	}

	template<>
	inline m128d clamp(const m128d& val, const m128d& minval, const m128d& maxval)
	{
		// Branchless SSE clamp.
		// return minss( maxss(val,minval), maxval );

		return _mm_min_pd(_mm_max_pd(val.val, minval.val), maxval.val);
	}

	inline m128d sqrt(const m128d& a) { return _mm_sqrt_pd(a.val); }

	//inline m128d	adjustoToInt32(const m128d& a)					 { return _mm_cvtepi32_ps(_mm_cvtps_epi32(a.val)); }
}}

#endif