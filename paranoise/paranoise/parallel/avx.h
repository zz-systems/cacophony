#pragma once
#ifndef PARANOISE_AVX_H
#define PARANOISE_AVX_H

#include "dependencies.h"

#include "int8.h"
#include "float8.h"
#include "double4.h"

// Traits =========================================================================================================
template<typename featuremask>
struct std::_Is_integral<zzsystems::simdal::_int8> : std::true_type {	};

template<typename featuremask>
struct std::_Is_floating_point<zzsystems::simdal::_float8> : std::true_type {	};

//template<>
//struct std::_Is_floating_point<zzsystems::simdal::double2>	: std::true_type {	};

namespace zzsystems { namespace simdal {

	// Converting constructors ===================================================================================
	ANY(featuremask)
	_int8::int8(const _float8& rhs) : int8(rhs.val) { }
	ANY(featuremask)
	inline _int8::int8(const _int8& rhs) : int8(rhs.val) { }
	//inline int8::int8(const double4& rhs) : int8(rhs.val) { }

	ANY(featuremask)
	_float8::float8(const _float8& rhs) : float8(rhs.val) { }
	ANY(featuremask)
	_float8::float8(const _int8& rhs) : float8(rhs.val) { }
	//inline float8::float8(const double4& rhs) : float8(rhs.val) { }

	/*inline double4::double4(const float8& rhs) : double4(rhs.val) { }
	inline double4::double4(const int8& rhs) : double4(rhs.val) { }
	inline double4::double4(const double4& rhs) : double4(rhs.val) { }*/

	ANY(featuremask) int32_t* extract(_int8 &src)
	{
		return src.val.m256i_i32;
	}

	ANY(featuremask) float* extract(_float8 &src)
	{
		return src.val.m256_f32;
	}

	// Integer SQRT =============================================================================================	
	FEATURE_FUNC(vsqrt, _int8, _dispatcher::has_sse)
		(const _int4 a)
	{
		BODY(_mm_sqrt_ps(static_cast<_float8>(a).val));
	}

	// Integer DIV ==============================================================================================	

	FEATURE_BIN_OP(/ , _int8, _dispatcher::has_sse)
	{
		BODY(_mm256_div_ps(static_cast<_float8>(a).val, static_cast<_float8>(b).val));
	}	

	FEATURE_FUNC(vsel, _int8, _dispatcher::has_avx2)
		(const _float8 a, const _int8 b, const _int8 c)
	{
		BODY(_mm256_blendv_epi8(c.val, b.val, _mm256_castps_si256(a.val)));
	}

	FEATURE_FUNC(vsel, _float8, _dispatcher::has_avx)
		(const _int8 a, const _float8 b, const _float8 c)
	{
		BODY(_mm256_blendv_ps(c.val, b.val, _mm256_castsi256_ps(a.val)));
	}
}}
#endif