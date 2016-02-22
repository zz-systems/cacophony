//#pragma once
//#ifndef PARANOISE_INTRINSIC_M256F_H
//#define PARANOISE_INTRINSIC_M256F_H
//
//#include "base.h"
//
//namespace paranoise {	namespace parallel {
//
//	struct int8;
//	struct double4;
//
//	ALIGN(32) struct float8 {
//		__m256 val;
//
//		float8() = default;
//		float8(const float& rhs)	{ val = _mm256_set1_ps(rhs); }
//
//		float8(VARGS8(uint8))		{ val = _mm256_cvtepi32_ps(_mm256_set_epi32(VPASS8)); }
//		float8(VARGS8(int))			{ val = _mm256_cvtepi32_ps(_mm256_set_epi32(VPASS8)); }
//		float8(VARGS8(float))		{ val = _mm256_set_ps(VPASS8); }
//
//		float8(const __m256& rhs)	{ val = rhs; }
//		float8(const __m256i& rhs)	{ val = _mm256_cvtepi32_ps(rhs); }
//		float8(const __m256d& rhs)	{ val = _mm256_castps128_ps256(_mm256_cvtpd_ps(rhs)); }
//
//		float8(const float8&	rhs);
//		float8(const int8&	rhs);
//		float8(const double4&	rhs);
//	};
//
//	template<> inline float8 create_mask(int mask) { return  _mm256_castsi256_ps(_mm256_set1_epi32(mask)); }
//
//	const auto ones8 = create_mask<float8>(0xFFFF'FFFF);
//	const auto sign1all08 = create_mask<float8>(0x8000'0000);
//	const auto sign0all18 = create_mask<float8>(0x7FFF'FFFF);
//
//	// Arithmetic =====================================================================================================
//	BIN_OP(float8, +)	{ BIN_BODY(_mm256_add_ps); }
//	BIN_OP(float8, -)	{ BIN_BODY(_mm256_sub_ps); }
//	BIN_OP(float8, *)	{ BIN_BODY(_mm256_mul_ps); }
//	BIN_OP(float8, /)	{ BIN_BODY(_mm256_div_ps); }
//	UN_OP(float8, -)	{ BODY(_mm256_sub_ps(_mm256_setzero_ps(), a.val)); }
//
//	// Comparison =====================================================================================================
//	BIN_OP(float8, >)	{ TRI_BODY_O(_mm256_cmp_ps, _CMP_GT_OQ); }
//	BIN_OP(float8, <)	{ TRI_BODY_O(_mm256_cmp_ps, _CMP_LT_OQ); }
//	BIN_OP(float8,==)	{ TRI_BODY_O(_mm256_cmp_ps, _CMP_EQ_OQ); }
//
//	// Bitwise ========================================================================================================
//	UN_OP(float8, ~)	{ UN_BODY_D(_mm256_andnot_ps); }
//	BIN_OP(float8, |)	{ BIN_BODY(_mm256_or_ps); }
//	BIN_OP(float8, &)	{ BIN_BODY(_mm256_and_ps); }
//	BIN_OP(float8, ^)	{ BIN_BODY(_mm256_xor_ps); }
//	
//	// Special functions ==============================================================================================
//	// Branchless select
//	TRI_FUNC(float8, vsel) { TRI_BODY_R(_mm256_blendv_ps); }
//	// Fused multiply-add
//	TRI_FUNC(float8, vfmadd) { TRI_BODY(_mm256_fmadd_ps); }
//	// Fused multiply-subtract
//	TRI_FUNC(float8, vfmsub) { TRI_BODY(_mm256_fmsub_ps); }
//
//	// Mathematical functions =========================================================================================
//	UN_FUNC(float8,  vabs)  { BODY(a & sign0all18); }
//	
//	BIN_FUNC(float8, vmin)  { BIN_BODY(_mm256_min_ps); }
//	BIN_FUNC(float8, vmax)  { BIN_BODY(_mm256_max_ps); }	
//	UN_FUNC(float8,  vsqrt) { UN_BODY(_mm256_sqrt_ps); }
//
//
//	inline float8 vfloor(const float8& a) {
//		auto v0 = _mm256_setzero_si256();
//		auto v1 = _mm256_cmpeq_epi32(v0, v0);
//		auto ji = _mm256_srli_epi32(v1, 25);
//		auto j = *(__m256*)&_mm256_slli_epi32(ji, 23); //create vector 1.0f
//		auto i = _mm256_cvttps_epi32(a.val);
//		auto fi = _mm256_cvtepi32_ps(i);
//		auto igx = _mm256_cmp_ps(fi, a.val, _CMP_GT_OS);
//		j = _mm256_and_ps(igx, j);
//		return _mm256_sub_ps(fi, j);
//	}
//
//	inline float8 vceil(const float8& a) {
//		auto v0 = _mm256_setzero_si256();
//		auto v1 = _mm256_cmpeq_epi32(v0, v0);
//		auto ji = _mm256_srli_epi32(v1, 25);
//		auto j = *(__m256*)&_mm256_slli_epi32(ji, 23); //create vector 1.0f
//		auto i = _mm256_cvttps_epi32(a.val);
//		auto fi = _mm256_cvtepi32_ps(i);
//		auto igx = _mm256_cmp_ps(fi, a.val, _CMP_LT_OS);
//		j = _mm256_and_ps(igx, j);
//		return _mm256_add_ps(fi, j);
//	}
//
//	inline float8 vround(const float8 &a) {
//		auto v0 = _mm256_setzero_ps();             //generate the highest value &lt; 2
//		auto v1 = _mm256_cmp_ps(v0, v0, _CMP_EQ_OS);
//		auto vNearest2 = *(__m256*)&_mm256_srli_epi32(*(__m256i*)&v1, 2);
//		auto i = _mm256_cvttps_epi32(a.val);
//		auto aTrunc = _mm256_cvtepi32_ps(i);        // truncate a
//		auto rmd = _mm256_sub_ps(a.val, aTrunc);        // get remainder
//		auto rmd2 = _mm256_mul_ps(rmd, vNearest2); // mul remainder by near 2 will yield the needed offset
//		auto rmd2i = _mm256_cvttps_epi32(rmd2);    // after being truncated of course
//		auto rmd2Trunc = _mm256_cvtepi32_ps(rmd2i);
//		auto r = _mm256_add_ps(aTrunc, rmd2Trunc);
//		return r;
//	}
//
//	
//
//}}
//
//#endif