#pragma once
#ifndef PARANOISE_AVX_H
#define PARANOISE_AVX_H

#include "dependencies.h"

#include "int8_avx1.h"
#include "int8.h"
#include "float8.h"
#include "double4.h"

// Traits =========================================================================================================
template<typename featuremask>
struct std::_Is_integral<zzsystems::simdal::_int8> : std::true_type {	};

template<typename featuremask>
struct std::_Is_integral<zzsystems::simdal::_int4x2> : std::true_type {	};

template<typename featuremask>
struct std::_Is_floating_point<zzsystems::simdal::_float8> : std::true_type {	};

//template<>
//struct std::_Is_floating_point<zzsystems::simdal::double2>	: std::true_type {	};

namespace zzsystems { namespace simdal {

	// Converting constructors ===================================================================================
	ANY(featuremask)
	_int8::int8(const _float8& rhs) : int8(rhs.val) { }
	ANY(featuremask)
	_int8::int8(const _int8& rhs) : int8(rhs.val) { }
	//inline int8::int8(const double4& rhs) : int8(rhs.val) { }

	ANY(featuremask)
		_int4x2::_int4x2(const _float8& rhs) : _int4x2(_mm256_extractf128_ps(rhs.val, 1), _mm256_extractf128_ps(rhs.val, 0)) { }
	ANY(featuremask)
		_int4x2::_int4x2(const _int4x2& rhs) : _int4x2(rhs.hi, rhs.lo) { }
	ANY(featuremask)
		_int4x2::_int4x2(const _int4& rhs_hi, const _int4& rhs_lo) : _int4x2(rhs_hi.val, rhs_lo.val) { }

	ANY(featuremask)
	_float8::float8(const _float8& rhs) : float8(rhs.val) { }
	ANY(featuremask)
	_float8::float8(const _int8& rhs) : float8(rhs.val) { }
	ANY(featuremask)
	_float8::float8(const _int4x2& rhs) 
		: float8(_mm256_set_m128(_mm_cvtepi32_ps(rhs.hi.val), _mm_cvtepi32_ps(rhs.lo.val)))
	{
		_mm256_zeroupper();
	}
	//inline float8::float8(const double4& rhs) : float8(rhs.val) { }

	/*inline double4::double4(const float8& rhs) : double4(rhs.val) { }
	inline double4::double4(const int8& rhs) : double4(rhs.val) { }
	inline double4::double4(const double4& rhs) : double4(rhs.val) { }*/

	ANY(featuremask) int32_t* extract(_int8 &src)
	{
		return src.val.m256i_i32;
	}
	ANY(featuremask)
		const int32_t* extract(const _int8 &src)
	{
		return src.val.m256i_i32;
	}

	ANY(featuremask) int32_t* extract(_int4x2 &src)
	{
		return src.hi.val.m128i_i32;
	}

	ANY(featuremask)
		const int32_t* extract(const _int4x2 &src)
	{
		return src.hi.val.m128i_i32;
	}

	ANY(featuremask) float* extract(_float8 &src)
	{
		return src.val.m256_f32;
	}

	ANY(featuremask)
		const float* extract(const _float8 &src)
	{
		return src.val.m256_f32;
	}

	ANY(featuremask)
		_int8 vgather(int* memloc, _int8 index)
	{
		return _mm256_i32gather_epi32(memloc, index.val, sizeof(int));
	}

	ANY(featuremask)
		_int4x2 vgather(int* memloc, _int4x2 index)
	{
		return{ vgather(memloc, index.hi), vgather(memloc, index.lo) };
	}

	ANY(featuremask)
		_float8 vgather(float* memloc, _int4x2 index)
	{
		return _mm256_set_m128(vgather(memloc, index.hi).val, vgather(memloc, index.lo).val);
	}

	ANY(featuremask)
		_float8 vgather(float* memloc, _int8 index)
	{
		return _mm256_i32gather_ps(memloc, index.val, sizeof(float));
	}


	// Integer SQRT =============================================================================================	
	FEATURE_FUNC(vsqrt, _int8, _dispatcher::has_avx)
		(const _int8 &a)
	{
		BODY(_mm256_sqrt_ps(static_cast<_float8>(a).val));
	}

	FEATURE_FUNC(vsqrt, _int4x2, _dispatcher::has_avx)
		(const _int4x2 &a)
	{
		BODY(_mm256_sqrt_ps(static_cast<_float8>(a).val));
	}
	// Integer DIV ==============================================================================================	

	FEATURE_BIN_OP(/ , _int8, _dispatcher::has_avx2)
	{
		BODY(_mm256_div_ps(static_cast<_float8>(a).val, static_cast<_float8>(b).val));
	}	

	FEATURE_BIN_OP(/ , _int4x2, _dispatcher::has_avx)
	{
		BODY(_mm256_div_ps(static_cast<_float8>(a).val, static_cast<_float8>(b).val));
	}

	FEATURE_FUNC(vsel, _int8, _dispatcher::has_avx2)
		(const _float8 &a, const _int8 &b, const _int8 &c)
	{		
		BODY(vsel(c, b, _int8(a)));
	}

	FEATURE_FUNC(vsel, _float8, _dispatcher::has_avx2)
		(const _int8 &a, const _float8 &b, const _float8 &c)
	{
		BODY(vsel(c, b, _float8(a)));
	}

	FEATURE_FUNC(vsel, _int4x2, _dispatcher::has_avx && !_dispatcher::has_avx2)
		(const _float8 &a, const _int4x2 &b, const _int4x2 &c)
	{
		BODY(vsel(c, b, _int4x2(a.val)));
	}

	FEATURE_FUNC(vsel, _float8, _dispatcher::has_avx && !_dispatcher::has_avx2)
		(const _int4x2 &a, const _float8 &b, const _float8 &c)
	{
		BODY(vsel(c, b, _float8(a)));
	}
}}
#endif