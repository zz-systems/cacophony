//#pragma once
//#ifndef PARANOISE_INTRINSIC_M256D_H
//#define PARANOISE_INTRINSIC_M256D_H
//
//#include "base.h"
//
//namespace paranoise {	namespace parallel {
//
//	struct float8;
//	struct int8;
//
//	ALIGN(32) struct double4 {
//		__m256d val;
//		/*double values[4];*/
//
//		double4() = default;
//		double4(const float& rhs)		{ val = _mm256_set1_pd(rhs); }
//
//		double4(const __m256d& rhs)	{ val = rhs; }
//		double4(const __m256i& rhs)	{ val = _mm256_cvtepi32_pd(_mm256_castsi256_si128(rhs)); }
//		double4(const __m256& rhs)	{ val = _mm256_cvtps_pd(_mm256_castps256_ps128(rhs)); }
//
//		double4(const float8&	rhs);
//		double4(const int8&	rhs);
//		double4(const double4&	rhs);		
//	};
//
//	inline double4 operator +(const double4& a, const double4& b)	{ return _mm256_add_pd		(a.val, b.val); }
//	inline double4 operator -(const double4& a, const double4& b)	{ return _mm256_sub_pd		(a.val, b.val); }
//	inline double4 operator *(const double4& a, const double4& b)	{ return _mm256_mul_pd		(a.val, b.val); }
//	inline double4 operator /(const double4& a, const double4& b)	{ return _mm256_div_pd		(a.val, b.val); }
//
//	inline double4 operator >(const double4& a, const double4& b)	{ return _mm256_cmp_pd		(a.val, b.val, _CMP_GT_OQ); }
//	inline double4 operator <(const double4& a, const double4& b)	{ return _mm256_cmp_pd		(a.val, b.val, _CMP_LT_OQ); }	
//	inline double4 operator==(const double4& a, const double4& b)	{ return _mm256_cmp_pd		(a.val, b.val, _CMP_EQ_OQ); }
//
//	inline double4 operator |(const double4& a, const double4& b)	{ return _mm256_or_pd		(a.val, b.val); }
//	inline double4 operator &(const double4& a, const double4& b)	{ return _mm256_and_pd		(a.val, b.val); }
//	inline double4 operator ^(const double4& a, const double4& b)	{ return _mm256_xor_pd		(a.val, b.val); }
//	inline double4 operator ~(const double4& a)						{ return _mm256_andnot_pd	(a.val, a.val); }
//
//
//	inline double4 vmin(const double4& a, const double4& b)			{ return _mm256_min_pd		(a.val, b.val); }
//	inline double4 vmax(const double4& a, const double4& b)			{ return _mm256_max_pd		(a.val, b.val); }
//
//	inline double4 vsqrt(const double4& a)						{	 return _mm256_sqrt_pd		(a.val); }
//}}
//
//#endif