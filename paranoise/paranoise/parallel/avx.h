#pragma once
#ifndef PARANOISE_AVX_H
#define PARANOISE_AVX_H

#include "base.h"
#include "int8.h"
#include "float8.h"
#include "double4.h"
#include "x87compat.h"

// Traits =========================================================================================================
template<>
struct std::_Is_integral<paranoise::parallel::int8>		: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::float8>	: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::double4>	: std::true_type {	};

namespace paranoise { namespace parallel {

	// Converting constructors ===================================================================================
	inline int8::int8(const float8& rhs) : int8(rhs.val) { }
	inline int8::int8(const int8& rhs) : int8(rhs.val) { }
	inline int8::int8(const double4& rhs) : int8(rhs.val) { }

	inline float8::float8(const float8& rhs) : float8(rhs.val) { }
	inline float8::float8(const int8& rhs) : float8(rhs.val) { }
	inline float8::float8(const double4& rhs) : float8(rhs.val) { }

	inline double4::double4(const float8& rhs) : double4(rhs.val) { }
	inline double4::double4(const int8& rhs) : double4(rhs.val) { }
	inline double4::double4(const double4& rhs) : double4(rhs.val) { }

	// Integer SQRT =============================================================================================
	inline int8 vsqrt(const int8& a) { return _mm256_sqrt_ps(float8(a).val); }

	// Integer DIV ==============================================================================================
	inline int8 operator /(const int8& a, const int8& b) { return _mm256_div_ps(float8(a).val, float8(b).val); }	

	inline int8 vsel(const int8& mask, const int8 &a, const int8 &b)
	{
		return _mm256_blendv_epi8(b.val, a.val, mask.val);
	}

	inline int8 vsel(const float8& mask, const int8 &a, const int8 &b)
	{
		return _mm256_blendv_epi8(b.val, a.val, _mm256_castps_si256(mask.val));
	}
}}
#endif