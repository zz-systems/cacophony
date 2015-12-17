#pragma once
#ifndef INTRINSIC_H 
#define INTRINSIC_H

//#include "basetypes.h"
#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

namespace noisepp {	namespace intrinsic {

	// Arithmetic ================================================================================================

	inline __m128i	operator +(__m128i one, __m128i other)		{ return _mm_add_epi32		(one, other); }
	inline __m128	operator +(__m128 one, __m128 other)		{ return _mm_add_ps			(one, other); }
	inline __m128d	operator +(__m128d one, __m128d other)		{ return _mm_add_pd			(one, other); }
	inline __m256i	operator +(__m256i one, __m256i other)		{ return _mm256_add_epi32	(one, other); }
	inline __m256	operator +(__m256 one, __m256 other)		{ return _mm256_add_ps		(one, other); }
	inline __m256d	operator +(__m256d one, __m256d other)		{ return _mm256_add_pd		(one, other); }

	inline __m128i	operator -(__m128i one, __m128i other)		{ return _mm_sub_epi32		(one, other); }
	inline __m128	operator -(__m128 one, __m128 other)		{ return _mm_sub_ps			(one, other); }
	inline __m128d	operator -(__m128d one, __m128d other)		{ return _mm_sub_pd			(one, other); }
	inline __m256i	operator -(__m256i one, __m256i other)		{ return _mm256_sub_epi32	(one, other); }
	inline __m256	operator -(__m256 one, __m256 other)		{ return _mm256_sub_ps		(one, other); }
	inline __m256d	operator -(__m256d one, __m256d other)		{ return _mm256_sub_pd		(one, other); }

	inline __m128i	operator *(__m128i one, __m128i other)		{ return _mm_mul_epi32		(one, other); }
	inline __m128	operator *(__m128 one, __m128 other)		{ return _mm_mul_ps			(one, other); }
	inline __m128d	operator *(__m128d one, __m128d other)		{ return _mm_mul_pd			(one, other); }
	inline __m256i	operator *(__m256i one, __m256i other)		{ return _mm256_mul_epi32	(one, other); }
	inline __m256	operator *(__m256 one, __m256 other)		{ return _mm256_mul_ps		(one, other); }
	inline __m256d	operator *(__m256d one, __m256d other)		{ return _mm256_mul_pd		(one, other); }

	//inline __m128i	operator /(__m128i one, __m128i other)		{ return _mm_di	(one, other); }
	inline __m128	operator /(__m128 one, __m128 other)		{ return _mm_div_ps			(one, other); }
	inline __m128d	operator /(__m128d one, __m128d other)		{ return _mm_div_pd			(one, other); }
	inline __m256	operator /(__m256 one, __m256 other)		{ return _mm256_div_ps		(one, other); }
	inline __m256d	operator /(__m256d one, __m256d other)		{ return _mm256_div_pd		(one, other); }
		
	inline __m128i	operator >(__m128i one, __m128i other)		{ return _mm_cmpgt_epi32	(one, other); }
	inline __m128	operator >(__m128 one, __m128 other)		{ return _mm_cmpgt_ps		(one, other); }
	inline __m128d	operator >(__m128d one, __m128d other)		{ return _mm_cmpgt_pd		(one, other); }
	inline __m256i	operator >(__m256i one, __m256i other)		{ return _mm256_cmpgt_epi32	(one, other); }
	inline __m256	operator >(__m256 one, __m256 other)		{ return _mm256_cmp_ps		(one, other, _CMP_GT_OQ); }
	inline __m256d	operator >(__m256d one, __m256d other)		{ return _mm256_cmp_pd		(one, other, _CMP_GT_OQ); }

	// Bitwise ==================================================================================================

	inline __m128i	operator &(__m128i one, __m128i other)		{ return _mm_andnot_si128	(one, other); }
	inline __m128	operator &(__m128 one, __m128 other)		{ return _mm_and_ps			(one, other); }
	inline __m128d	operator &(__m128d one, __m128d other)		{ return _mm_and_pd			(one, other); }
	inline __m256i	operator &(__m256i one, __m256i other)		{ return _mm256_and_si256	(one, other); }
	inline __m256	operator &(__m256 one, __m256 other)		{ return _mm256_and_ps		(one, other); }
	inline __m256d	operator &(__m256d one, __m256d other)		{ return _mm256_and_pd		(one, other); }

	inline __m128i	operator ^(__m128i one, __m128i other)		{ return _mm_xor_si128		(one, other); }
	inline __m256i	operator ^(__m256i one, __m256i other)		{ return _mm256_xor_si256	(one, other); }

	inline __m128i	operator >>(__m128i one, int amount)		{ return _mm_srli_epi32		(one, amount); }
	inline __m256i	operator >>(__m256i one, int amount)		{ return _mm256_srli_epi32	(one, amount); }

	inline __m128i	operator <<(__m128i one, int amount)		{ return _mm_slli_epi32		(one, amount); }
	inline __m256i	operator <<(__m256i one, int amount)		{ return _mm256_slli_epi32	(one, amount); }

	// Load/Store ===============================================================================================

	template<typename T, typename U>
	inline T ld1(U val);

	template<>
	inline __m128i	ld1(const int val) { return _mm_set1_epi32(val); }

	template<>
	inline __m128	ld1(float val) { return _mm_set_ps1(val); }

	template<>
	inline __m128d	ld1(double val) { return _mm_set1_pd(val); }

	template<>
	inline __m256i	ld1(const int val)	{ return _mm256_set1_epi32(val); }

	template<>
	inline __m256	ld1(float val)	{ return _mm256_set1_ps(val); }

	template<>
	inline __m256d	ld1(double val) { return _mm256_set1_pd(val); }

	/*template<typename T, typename U>
	inline T xtr(U val, int index);

	template<>
	inline int32	xtr(__m128i val, const int index) { return _mm_extract_epi32(val, index); }

	template<>
	inline float	xtr(__m128 val, const int index) { return _mm_extract_ps(val, index); }
*/
	/*template<typename T, typename U>
	inline void		ins(T target, U val, int index);

	template<>
	inline void		ins(__m128i target, int32 value, int index) { _mm_insert_epi32(target, value, index); }

	template<>
	inline void		ins(__m128 target, float value, int index) { _mm_insert_ps(target, value, index); }*/
	
	// Conversion ===============================================================================================

	template<typename TRGT, typename SRC>
	inline TRGT	cast(SRC src);

	/*template<typename T>
	inline T	cast(__m128i src);

	template<typename T>
	inline T	cast(__m256i src);

	template<typename T>
	inline T	cast(__m128 src);

	template<typename T>
	inline T	cast(__m128d src);

	template<typename T>
	inline T	cast(__m256 src);

	template<typename T>
	inline T	cast(__m256d src);*/

	template<>
	inline __m128	cast(__m128i src)	{ return _mm_cvtepi32_ps(src); }
	template<>
	inline __m256	cast(__m256i src)	{ return _mm256_cvtepi32_ps(src); }
	template<>
	inline __m128d	cast(__m128i src)	{ return _mm_cvtepi32_pd(src); }
	template<>
	inline __m256d	cast(__m128i src)	{ return _mm256_cvtepi32_pd(src); }

	template<>
	inline __m128i  cast(__m128 src)	{ return _mm_cvtps_epi32(src); }
	template<>
	inline __m128i  cast(__m128d src)	{ return _mm_cvtpd_epi32(src); }
	template<>
	inline __m256i  cast(__m256 src)	{ return _mm256_cvtps_epi32(src); }
	template<>
	inline __m128i  cast(__m256d src)	{ return _mm256_cvtpd_epi32(src); }

}}

#endif 