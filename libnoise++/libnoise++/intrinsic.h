#pragma once
#ifndef INTRINSIC_H 
#define INTRINSIC_H

#include "basetypes.h"
#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

namespace noisepp {	namespace intrinsic {
	// Types =====================================================================================================
	union m128;
	union m128i;
	union m128d;
	union m256;
	union m256i;
	union m256d;

	__declspec(align(16)) union m128i {
		__m128i val;
		int64 ints64[2];
		int32 ints32[4];
		int16 ints16[8];
		int8  bytes[16];

		m128i() = default;
		m128i(const int& rhs)		{ val = _mm_set1_epi32(rhs);  }
		
		m128i(const __m128& rhs)	{ val = _mm_cvtps_epi32(rhs); }
		m128i(const __m128i& rhs)	{ val = rhs; }		
		m128i(const __m128d& rhs)	{ val = _mm_cvtpd_epi32(rhs); }

		m128i(const m128& rhs);
		m128i(const m128i& rhs);
		m128i(const m128d& rhs);

		explicit operator int32*() { return ints32; }
		explicit operator int16*() { return ints16; }
		explicit operator int8*()  { return bytes;  }
	};

	
	__declspec(align(16)) union m128 {
		__m128 val;
		float values[4];

		m128() = default;
		m128(const float& rhs)		{ val = _mm_set_ps1(rhs); }

		m128(const __m128& rhs)		{ val = rhs; }
		m128(const __m128i& rhs)	{ val = _mm_cvtepi32_ps(rhs); }
		m128(const __m128d& rhs)	{ val = _mm_cvtpd_ps(rhs); }

		m128(const m128& rhs);
		m128(const m128i& rhs);
		m128(const m128d& rhs);

		explicit operator float*() { return values; }
	};

	__declspec(align(16)) union m128d {
		__m128d val;
		double values[2];

		m128d() = default;
		m128d(const double& rhs)	{ val = _mm_set1_pd(rhs); }

		m128d(const __m128& rhs)	{ val = _mm_cvtps_pd(rhs); }
		m128d(const __m128i& rhs)	{ val = _mm_cvtepi32_pd(rhs); }
		m128d(const __m128d& rhs)	{ val = rhs; }

		m128d(const m128& rhs);
		m128d(const m128i& rhs);
		m128d(const m128d& rhs);

		explicit operator double*() { return values; }
	};

	__declspec(align(32)) union m256i {
		__m256i val;
		int64 ints64[8];
		int32 ints32[8];
		int16 ints16[16];
		int8  bytes[32];

		m256i() = default;
		m256i(const int32& rhs)		{ val = _mm256_set1_epi32(rhs);	}

		m256i(const __m256& rhs)	{ val = _mm256_cvtps_epi32(rhs); }
		m256i(const __m256i& rhs)	{ val = rhs; }
		m256i(const __m256d& rhs)	{ val = _mm256_castpd_si256(rhs); }

		m256i(const m256& rhs);
		m256i(const m256i& rhs);
		m256i(const m256d& rhs);

		explicit operator int32*() { return ints32; }
		explicit operator int16*() { return ints16; }
		explicit operator int8*()  { return bytes;  }
	};

	__declspec(align(32)) union m256 {
		__m256 val;
		float values[8];

		m256() = default;
		m256(const float& rhs)		{ val = _mm256_set1_ps(rhs); }

		m256(const __m256& rhs)		{ val = rhs; }
		m256(const __m256i& rhs)	{ val = _mm256_cvtepi32_ps(rhs); }
		m256(const __m256d& rhs)	{ val = _mm256_castpd_ps(rhs); }

		m256(const m256& rhs);
		m256(const m256i& rhs);
		m256(const m256d& rhs);

		explicit operator float*() { return values; }
	};

	__declspec(align(32)) union m256d {
		__m256d val;
		double values[4];

		m256d() = default;
		m256d(const double& rhs)	{ val = _mm256_set1_pd(rhs); }

		m256d(const __m256& rhs)	{ val = _mm256_castps_pd(rhs); }
		m256d(const __m256i& rhs)	{ val = _mm256_castsi256_pd(rhs); }
		m256d(const __m256d& rhs)	{ val = rhs; }

		m256d(const m256& rhs);
		m256d(const m256i& rhs);
		m256d(const m256d& rhs);

		explicit operator double*() { return values; }
	};

	inline m128i::m128i(const m128& rhs)	: m128i(rhs.val) { }
	inline m128i::m128i(const m128i& rhs)	: m128i(rhs.val) { }
	inline m128i::m128i(const m128d& rhs)	: m128i(rhs.val) { }

	inline m128::m128(const m128& rhs)		: m128(rhs.val) { }
	inline m128::m128(const m128i& rhs)		: m128(rhs.val) { }
	inline m128::m128(const m128d& rhs)		: m128(rhs.val) { }

	inline m128d::m128d(const m128& rhs)	: m128d(rhs.val) { }
	inline m128d::m128d(const m128i& rhs)	: m128d(rhs.val) { }
	inline m128d::m128d(const m128d& rhs)	: m128d(rhs.val) { }

	inline m256i::m256i(const m256& rhs)	: m256i(rhs.val) { }
	inline m256i::m256i(const m256i& rhs)	: m256i(rhs.val) { }
	inline m256i::m256i(const m256d& rhs)	: m256i(rhs.val) { }

	inline m256::m256(const m256& rhs)		: m256(rhs.val) { }
	inline m256::m256(const m256i& rhs)		: m256(rhs.val) { }
	inline m256::m256(const m256d& rhs)		: m256(rhs.val) { }

	inline m256d::m256d(const m256& rhs)	: m256d(rhs.val) { }
	inline m256d::m256d(const m256i& rhs)	: m256d(rhs.val) { }
	inline m256d::m256d(const m256d& rhs)	: m256d(rhs.val) { }

	// Arithmetic ================================================================================================

	inline m128i	operator +(const m128i& one, const m128i& other)		{ return _mm_add_epi32		(one.val, other.val); }
	inline m128		operator +(const m128& one, const m128& other)			{ return _mm_add_ps			(one.val, other.val); }
	inline m128d	operator +(const m128d& one, const m128d& other)		{ return _mm_add_pd			(one.val, other.val); }
	inline m256i	operator +(const m256i& one, const m256i& other)		{ return _mm256_add_epi32	(one.val, other.val); }
	inline m256		operator +(const m256& one, const m256& other)			{ return _mm256_add_ps		(one.val, other.val); }
	inline m256d	operator +(const m256d& one, const m256d& other)		{ return _mm256_add_pd		(one.val, other.val); }

	inline m128i	operator -(const m128i& one, const m128i& other)		{ return _mm_sub_epi32		(one.val, other.val); }
	inline m128		operator -(const m128& one, const m128& other)			{ return _mm_sub_ps			(one.val, other.val); }
	inline m128d	operator -(const m128d& one, const m128d& other)		{ return _mm_sub_pd			(one.val, other.val); }
	inline m256i	operator -(const m256i& one, const m256i& other)		{ return _mm256_sub_epi32	(one.val, other.val); }
	inline m256		operator -(const m256& one, const m256& other)			{ return _mm256_sub_ps		(one.val, other.val); }
	inline m256d	operator -(const m256d& one, const m256d& other)		{ return _mm256_sub_pd		(one.val, other.val); }

	inline m128i	operator *(const m128i& one, const m128i& other)
	{ 
#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
			return _mm_mullo_epi32(one.val, other.val);
#else               // old CPU - use SSE 2
			__m128i tmp1 = _mm_mul_epu32(one.val, other.val); /* mul 2,0*/
			__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(one.val, 4), _mm_srli_si128(other.val, 4)); /* mul 3,1 */
			return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0, 0, 2, 0))); /* shuffle results to [63..0] and pack */
#endif		
	}
	inline m128		operator *(const m128& one, const m128& other)			{ return _mm_mul_ps			(one.val, other.val); }
	inline m128d	operator *(const m128d& one, const m128d& other)		{ return _mm_mul_pd			(one.val, other.val); }
	inline m256i	operator *(const m256i& one, const m256i& other)		{ return _mm256_mul_epi32	(one.val, other.val); }
	inline m256		operator *(const m256& one, const m256& other)			{ return _mm256_mul_ps		(one.val, other.val); }
	inline m256d	operator *(const m256d& one, const m256d& other)		{ return _mm256_mul_pd		(one.val, other.val); }

	//inline m128i	operator /(const m128i& one, const m128i& other)		{ return _mm_di	(one.val, other.val); }
	inline m128		operator /(const m128& one, const m128& other)			{ return _mm_div_ps			(one.val, other.val); }
	inline m128d	operator /(const m128d& one, const m128d& other)		{ return _mm_div_pd			(one.val, other.val); }
	inline m256		operator /(const m256& one, const m256& other)			{ return _mm256_div_ps		(one.val, other.val); }
	inline m256d	operator /(const m256d& one, const m256d& other)		{ return _mm256_div_pd		(one.val, other.val); }
		
	inline m128i	operator >(const m128i& one, const m128i& other)		{ return _mm_cmpgt_epi32	(one.val, other.val); }
	inline m128		operator >(const m128& one, const m128& other)			{ return _mm_cmpgt_ps		(one.val, other.val); }
	inline m128d	operator >(const m128d& one, const m128d& other)		{ return _mm_cmpgt_pd		(one.val, other.val); }
	inline m256i	operator >(const m256i& one, const m256i& other)		{ return _mm256_cmpgt_epi32	(one.val, other.val); }
	inline m256		operator >(const m256& one, const m256& other)			{ return _mm256_cmp_ps		(one.val, other.val, _CMP_GT_OQ); }
	inline m256d	operator >(const m256d& one, const m256d& other)		{ return _mm256_cmp_pd		(one.val, other.val, _CMP_GT_OQ); }

	// Bitwise ==================================================================================================

	inline m128i	operator &(const m128i& one, const m128i& other)		{ return _mm_and_si128	(one.val, other.val); }
	inline m128		operator &(const m128& one, const m128& other)			{ return _mm_and_ps			(one.val, other.val); }
	inline m128d	operator &(const m128d& one, const m128d& other)		{ return _mm_and_pd			(one.val, other.val); }
	inline m256i	operator &(const m256i& one, const m256i& other)		{ return _mm256_and_si256	(one.val, other.val); }
	inline m256		operator &(const m256& one, const m256& other)			{ return _mm256_and_ps		(one.val, other.val); }
	inline m256d	operator &(const m256d& one, const m256d& other)		{ return _mm256_and_pd		(one.val, other.val); }

	inline m128i	operator ^(const m128i& one, const m128i& other)		{ return _mm_xor_si128		(one.val, other.val); }
	inline m256i	operator ^(const m256i& one, const m256i& other)		{ return _mm256_xor_si256	(one.val, other.val); }

	inline m128i	operator >>(const m128i& one, int amount)				{ return _mm_srli_epi32		(one.val, amount); }
	inline m256i	operator >>(const m256i& one, int amount)				{ return _mm256_srli_epi32	(one.val, amount); }

	inline m128i	operator <<(const m128i& one, int amount)				{ return _mm_slli_epi32		(one.val, amount); }
	inline m256i	operator <<(const m256i& one, int amount)				{ return _mm256_slli_epi32	(one.val, amount); }

	// Assignment ===============================================================================================

	/*inline m128i	operator (const m128i& rhs, const m128i& other) { return _mm_andnot_si128(one.val, other.val); }
	inline m128		operator &(const m128& one, const m128& other) { return _mm_and_ps(one.val, other.val); }
	inline m128d	operator &(const m128d& one, const m128d& other) { return _mm_and_pd(one.val, other.val); }
	inline m256i	operator &(const m256i& one, const m256i& other) { return _mm256_and_si256(one.val, other.val); }
	inline m256		operator &(const m256& one, const m256& other) { return _mm256_and_ps(one.val, other.val); }
	inline m256d	operator &(const m256d& one, const m256d& other) { return _mm256_and_pd(one.val, other.val); }*/

	// Load/Store ===============================================================================================

	//template<typename IGNORE>
	inline int32* extract(int32& src/*, IGNORE& ref*/)	{ return &src; }

	inline int32* extract(m128i& src)				{ return src.ints32; }
	inline int32* extract(m128i& src, m128& ref)	{ return src.ints32; }
	inline int64* extract(m128i& src, m128d& ref)	{ return src.val.m128i_i64; }

	inline int32* extract(m256i& src)				{ return src.ints32; }
	inline int32* extract(m256i& src, m256& ref)	{ return src.ints32; }
	inline int64* extract(m256i& src, m256d& ref)	{ return src.val.m256i_i64; }

	inline float* extract(float& src)		{ return &src; }
	inline float* extract(m128& src)		{ return src.values; }
	inline float* extract(m256& src)		{ return src.values; }

	inline double* extract(double& src)		{ return &src; }
	inline double* extract(m128d& src)		{ return src.values; }
	inline double* extract(m256d& src)		{ return src.values; }

	// Sizes ====================================================================================================
	/*template<typename T>
	inline size_t elem_count(const T& val);*/

	//template<>
	inline size_t elem_count(const int32& val) { return 1; }

	//template<>
	inline size_t elem_count(const float& val) { return 1; }

	//template<>
	inline size_t elem_count(const double& val) { return 1; }

	//template<>
	inline size_t elem_count(const m128& val) { return 4; }

	//template<>
	inline size_t elem_count(const m128i& val) { return 4; }

	//template<>
	inline size_t elem_count(const m128d& val) { return 2; }

	//template<>
	inline size_t elem_count(const m256& val) { return 8; }

	//template<>
	inline size_t elem_count(const m256i& val) { return 8; }

	//template<>
	inline size_t elem_count(const m256d& val) { return 4; }
	// Conversion ===============================================================================================
		
}}

#endif 