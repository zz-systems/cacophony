#pragma once
#ifndef PARANOISE_INTRINSIC_M128I_H
#define PARANOISE_INTRINSIC_M128I_H

#include "base.h"

namespace paranoise { namespace parallel {

	union m128f;
	union m128d;

	ALIGN(16) union m128i {
		__m128i val;

		int64 i64[2];
		int32 i32[4];
		int16 i16[8];
		int8  i8[16];

		int32 values[4];

		m128i() = default;
		m128i(const int& rhs)		{ val = _mm_set1_epi32(rhs); }

		m128i(const __m128& rhs)	{ val = _mm_cvtps_epi32(rhs); }
		m128i(const __m128i& rhs)	{ val = rhs; }
		m128i(const __m128d& rhs)	{ val = _mm_cvtpd_epi32(rhs); }

		m128i(const m128i& rhs);
		m128i(const m128f& rhs);
		m128i(const m128d& rhs);
	};

	inline m128i operator +(const m128i& a, const m128i& b) { return _mm_add_epi32		(a.val, b.val); }
	inline m128i operator -(const m128i& a, const m128i& b) { return _mm_sub_epi32		(a.val, b.val); }

#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
	inline m128i operator *(const m128i& a, const m128i& b) { return _mm_mullo_epi32	(a.val, b.val); }
#else               // old CPU - use SSE 2
	inline m128i	operator *(const m128i& a, const m128i& b) 
	{
		__m128i tmp1 = _mm_mul_epu32(a.val, b.val); /* mul 2,0*/
		__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(a.val, 4), _mm_srli_si128(b.val, 4)); /* mul 3,1 */
		return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0, 0, 2, 0))); /* shuffle results to [63..0] and pack */	
	}
#endif	
	// yet to implement
	


	inline m128i operator >(const m128i& a, const m128i& b)	{ return _mm_cmpgt_epi32	(a.val, b.val); }
	inline m128i operator <(const m128i& a, const m128i& b)	{ return _mm_cmplt_epi32	(a.val, b.val); }	
	inline m128i operator==(const m128i& a, const m128i& b) { return _mm_cmpeq_epi32	(a.val, b.val); }

	inline m128i operator |(const m128i& a, const m128i& b)	{ return _mm_or_si128		(a.val, b.val); }
	inline m128i operator &(const m128i& a, const m128i& b)	{ return _mm_and_si128		(a.val, b.val); }
	inline m128i operator ^(const m128i& a, const m128i& b)	{ return _mm_xor_si128		(a.val, b.val); }
	inline m128i operator ~(const m128i& a)					{ return _mm_andnot_si128	(a.val, a.val); }

	inline m128i operator >>(const m128i& a, int amount)	{ return _mm_srli_epi32		(a.val, amount); }
	inline m128i operator <<(const m128i& a, int amount)	{ return _mm_slli_epi32		(a.val, amount); }


	inline m128i min(const m128i& a, const m128i& b)		{ return _mm_min_epi32		(a.val, b.val); }
	inline m128i max(const m128i& a, const m128i& b)		{ return _mm_max_epi32		(a.val, b.val); }		
}}

#endif