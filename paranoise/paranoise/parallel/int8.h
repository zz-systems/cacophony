//#pragma once
//#ifndef PARANOISE_INTRINSIC_M256I_H
//#define PARANOISE_INTRINSIC_M256I_H
//
//#include "base.h"
//
//namespace paranoise {	namespace parallel {
//
//	struct float8;
//	struct double4;
//
//	ALIGN(32) struct int8 {
//		__m256i val;
///*
//		int64 i64[4];
//		int32 i32[8];
//		int16 i16[16];*/
//
//		int8() = default;
//		int8(const int32& rhs)		{ val = _mm256_set1_epi32(rhs); }
//
//		int8(const __m256& rhs)		{ val = _mm256_cvtps_epi32(rhs); }
//		int8(const __m256i& rhs)	{ val = rhs; }
//		int8(const __m256d& rhs)	{ val = _mm256_castsi128_si256(_mm256_cvtpd_epi32(rhs)); }
//
//		int8(const float8&	rhs);
//		int8(const int8&	rhs);
//		int8(const double4&	rhs);
//	};
//
//	inline int8 operator +(const int8& a, const int8& b) { return _mm256_add_epi32	(a.val, b.val); }
//	inline int8 operator -(const int8& a, const int8& b) { return _mm256_sub_epi32	(a.val, b.val); }
//	inline int8 operator *(const int8& a, const int8& b) { return _mm256_mul_epi32	(a.val, b.val); }
//	
//
//	inline int8 operator >(const int8& a, const int8& b) { return _mm256_cmpgt_epi32	(a.val, b.val); }
//	inline int8 operator <(const int8& a, const int8& b) { return _mm256_cmpgt_epi32	(b.val, a.val); }
//	inline int8 operator==(const int8& a, const int8& b) { return _mm256_cmpeq_epi32 (a.val, b.val); }
//
//	inline int8 operator -(const int8& a)				 { return _mm256_sub_epi32(_mm256_set1_epi32(0), a.val); }
//	inline int8 operator ~(const int8& a)				 { return _mm256_andnot_si256(a.val, a.val); }
//	inline int8 operator |(const int8& a, const int8& b) { return _mm256_or_si256	(a.val, b.val); }
//	inline int8 operator &(const int8& a, const int8& b) { return _mm256_and_si256	(a.val, b.val); }
//	inline int8 operator ^(const int8& a, const int8& b) { return _mm256_xor_si256	(a.val, b.val); }
//	
//	inline int8 operator >>(const int8& a, int amount) { return _mm256_srli_epi32(a.val, amount); }
//	inline int8 operator <<(const int8& a, int amount) { return _mm256_slli_epi32(a.val, amount); }
//	
//	inline int8 vmin(const int8& a, const int8& b)		{ return _mm256_min_epi32	(a.val, b.val); }
//	inline int8 vmax(const int8& a, const int8& b)		{ return _mm256_max_epi32	(a.val, b.val); }	
//}}
//
//#endif