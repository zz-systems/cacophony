#pragma once
#ifndef NOISEPP_INTRINSIC_M256I_H
#define NOISEPP_INTRINSIC_M256I_H

#include "base.h"

namespace noisepp {	namespace intrinsic {

	union m256f;
	union m256d;

	ALIGN(32) union m256i {
		__m256i val;

		int64 i64[8];
		int32 i32[8];
		int16 i16[16];
		int8  i8[32];

		m256i() = default;
		m256i(const int32& rhs)		{ val = _mm256_set1_epi32(rhs); }

		m256i(const __m256& rhs)	{ val = _mm256_cvtps_epi32(rhs); }
		m256i(const __m256i& rhs)	{ val = rhs; }
		m256i(const __m256d& rhs)	{ val = _mm256_castpd_si256(rhs); }

		m256i(const m256f&	rhs);
		m256i(const m256i&	rhs);
		m256i(const m256d&	rhs);

		explicit operator int64*()	{ return i64; }
		explicit operator int32*()	{ return i32; }
		explicit operator int16*()	{ return i16; }
		explicit operator int8*()	{ return i8; }
	};

	inline m256i operator +(const m256i& a, const m256i& b) { return _mm256_add_epi32	(a.val, b.val); }
	inline m256i operator -(const m256i& a, const m256i& b) { return _mm256_sub_epi32	(a.val, b.val); }
	inline m256i operator *(const m256i& a, const m256i& b) { return _mm256_mul_epi32	(a.val, b.val); }
	//inline m256i operator /(const m256i& a, const m256i& b) { return _mm256_div_epi32(a.val, b.val); }

	inline m256i operator >(const m256i& a, const m256i& b) { return _mm256_cmpgt_epi32	(a.val, b.val); }
	inline m256i operator <(const m256i& a, const m256i& b) { return _mm256_cmpgt_epi32	(a.val, b.val); }

	inline m256i operator |(const m256i& a, const m256i& b) { return _mm256_or_si256	(a.val, b.val); }
	inline m256i operator &(const m256i& a, const m256i& b) { return _mm256_and_si256	(a.val, b.val); }
	inline m256i operator ^(const m256i& a, const m256i& b) { return _mm256_xor_si256	(a.val, b.val); }
	inline m256i operator ~(const m256i& a)				    { return _mm256_andnot_si256(a.val, a.val); }

	
	inline m256i min(const m256i& a, const m256i& b)		{ return _mm256_min_epi32	(a.val, b.val); }
	inline m256i max(const m256i& a, const m256i& b)		{ return _mm256_max_epi32	(a.val, b.val); }

	inline m256i sqrt(const m256i& a)						{ return _mm256_sqrt_ps		(m256f(a).val); }
}}

#endif