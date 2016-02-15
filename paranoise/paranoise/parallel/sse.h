#pragma once
#ifndef PARANOISE_SSE_H
#define PARANOISE_SSE_H

#include "base.h"
#include "int4.h"
#include "float4.h"
#include "double2.h"
#include "x87compat.h"

// Traits =========================================================================================================
template<>
struct std::_Is_integral<paranoise::parallel::int4>		: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::float4>	: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::double2>	: std::true_type {	};

namespace paranoise { namespace parallel {

	// Converting constructors ===================================================================================
	inline int4::int4(const float4& rhs) : int4(rhs.val) { }
	inline int4::int4(const int4& rhs) : int4(rhs.val) { }
	inline int4::int4(const double2& rhs) : int4(rhs.val) { }

	inline float4::float4(const float4& rhs) : float4(rhs.val) { }
	inline float4::float4(const int4& rhs) : float4(rhs.val) { }
	inline float4::float4(const double2& rhs) : float4(rhs.val) { }

	inline double2::double2(const float4& rhs) : double2(rhs.val) { }
	inline double2::double2(const int4& rhs) : double2(rhs.val) { }
	inline double2::double2(const double2& rhs) : double2(rhs.val) { }

	// Load/Store ===============================================================================================
	inline int32* extract(int4& src)				{ return src.i32; }
	inline int32* extract(int4& src, float4& ref)	{ return src.i32; }
	inline int64* extract(int4& src, double2& ref)	{ return src.val.m128i_i64; }
	
	inline float* extract(float4& src)				{ return src.values; }	
	inline double* extract(double2& src)				{ return src.values; }

	// Vector dimensions  =======================================================================================
	inline size_t dim(const float4& val) { return 4; }
	inline size_t dim(const int4& val) { return 4; }
	inline size_t dim(const double2& val) { return 2; }

	// Integer SQRT =============================================================================================	
	inline int4 sqrt(const int4& a) { return _mm_sqrt_ps(float4(a).val); }

	// Integer DIV ==============================================================================================	
	inline int4 operator /(const int4& a, const int4& b) { return _mm_div_ps(float4(a).val, float4(b).val); }

	inline int4 sel(const int4& mask, const int4 &a, const int4 &b)
	{
		return _mm_blendv_epi8(b.val, a.val, mask.val);
	}

	inline int4 sel(const float4& mask, const int4 &a, const int4 &b)
	{
		return _mm_blendv_epi8(b.val, a.val, _mm_castps_si128(mask.val));
	}
}}
#endif