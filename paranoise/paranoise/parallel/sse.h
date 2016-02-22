#pragma once
#ifndef PARANOISE_SSE_H
#define PARANOISE_SSE_H

#include "base.h"
#include "int4.h"
#include "float4.h"
#include "double2.h"
#include "x87compat.h"

// Traits =========================================================================================================
template<typename featuremask>
struct std::_Is_integral<paranoise::parallel::int4<featuremask>>		: std::true_type {	};

template<typename featuremask>
struct std::_Is_floating_point<paranoise::parallel::float4<featuremask>>	: std::true_type {	};

//template<>
//struct std::_Is_floating_point<paranoise::parallel::double2>	: std::true_type {	};

namespace paranoise { namespace parallel {

	// Converting constructors ===================================================================================
	template<typename featuremask>
	inline _int4::int4(const _float4& rhs) : int4(rhs.val) { }
	template<typename featuremask>
	inline _int4::int4(const _int4& rhs) : int4(rhs.val) { }
	//inline int4::int4(const double2& rhs) : int4(rhs.val) { }
	template<typename featuremask>
	inline _float4::float4(const _float4& rhs) : float4(rhs.val) { }
	template<typename featuremask>
	inline _float4::float4(const _int4& rhs) : float4(rhs.val) { }
	//inline float4::float4(const double2& rhs) : float4(rhs.val) { }

	/*inline double2::double2(const float4& rhs) : double2(rhs.val) { }
	inline double2::double2(const int4& rhs) : double2(rhs.val) { }
	inline double2::double2(const double2& rhs) : double2(rhs.val) { }	*/

	// Integer SQRT =============================================================================================	
	FEATURE_FUNC(vsqrt, _int4, _dispatcher::has_sse)
		(const _int4 a)
	{
		BODY(_mm_sqrt_ps(static_cast<_float4>(a).val));
	}	

	// Integer DIV ==============================================================================================	
		
	FEATURE_BIN_OP(/, _int4, _dispatcher::has_sse)
	{
		BODY(_mm_div_ps(static_cast<_float4>(a).val, static_cast<_float4>(b).val));
	}

	FEATURE_FUNC(vsel, _int4, !_dispatcher::has_sse41 && _dispatcher::has_sse)
		(const _float4 a, const _int4 b, const _int4 c)
	{
		auto mask = _mm_castps_si128(a.val);
		return mask & b | ~mask & c;
	}

	FEATURE_FUNC(vsel, _float4, !_dispatcher::has_sse41 && _dispatcher::has_sse)
		(const _int4 a, const _float4 b, const _float4 c)
	{
		auto mask = _mm_castsi128_ps(a.val);
		return mask & b | ~mask & c;
	}

	FEATURE_FUNC(vsel, _int4, _dispatcher::has_sse41 && _dispatcher::has_sse)
		(const _float4 a, const _int4 b, const _int4 c)
	{
		BODY(_mm_blendv_epi8(c.val, b.val, _mm_castps_si128(a.val)));
	}

	FEATURE_FUNC(vsel, _float4, _dispatcher::has_sse41 && _dispatcher::has_sse)
		(const _int4 a, const _float4 b, const _float4 c)
	{
		BODY(_mm_blendv_ps(c.val, b.val, _mm_castsi128_ps(a.val)));
	}
	
}}
#endif