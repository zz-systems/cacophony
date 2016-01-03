#pragma once
#ifndef PARANOISE_SSE_H
#define PARANOISE_SSE_H

#include "base.h"
#include "m128i.h"
#include "m128f.h"
#include "m128d.h"
#include "x87compat.h"

// Traits =========================================================================================================
template<>
struct std::_Is_integral<paranoise::parallel::m128i>		: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::m128f>	: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::m128d>	: std::true_type {	};

namespace paranoise { namespace parallel {

	// Converting constructors ===================================================================================
	inline m128i::m128i(const m128f& rhs) : m128i(rhs.val) { }
	inline m128i::m128i(const m128i& rhs) : m128i(rhs.val) { }
	inline m128i::m128i(const m128d& rhs) : m128i(rhs.val) { }

	inline m128f::m128f(const m128f& rhs) : m128f(rhs.val) { }
	inline m128f::m128f(const m128i& rhs) : m128f(rhs.val) { }
	inline m128f::m128f(const m128d& rhs) : m128f(rhs.val) { }

	inline m128d::m128d(const m128f& rhs) : m128d(rhs.val) { }
	inline m128d::m128d(const m128i& rhs) : m128d(rhs.val) { }
	inline m128d::m128d(const m128d& rhs) : m128d(rhs.val) { }

	// Load/Store ===============================================================================================
	inline int32* extract(m128i& src)				{ return src.i32; }
	inline int32* extract(m128i& src, m128f& ref)	{ return src.i32; }
	inline int64* extract(m128i& src, m128d& ref)	{ return src.val.m128i_i64; }

	inline float* extract(m128f& src)				{ return src.values; }	
	inline double* extract(m128d& src)				{ return src.values; }

	// Vector dimensions  =======================================================================================
	inline size_t dim(const m128f& val) { return 4; }
	inline size_t dim(const m128i& val) { return 4; }
	inline size_t dim(const m128d& val) { return 2; }

	// Integer SQRT =============================================================================================	
	inline m128i sqrt(const m128i& a) { return _mm_sqrt_ps(m128f(a).val); }

	// Integer DIV ==============================================================================================	
	inline m128i operator /(const m128i& a, const m128i& b) { return _mm_div_ps(m128f(a).val, m128f(b).val); }
}}
#endif