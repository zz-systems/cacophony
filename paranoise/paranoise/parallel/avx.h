#pragma once
#ifndef PARANOISE_AVX_H
#define PARANOISE_AVX_H

#include "base.h"
#include "m256i.h"
#include "m256f.h"
#include "m256d.h"
#include "x87compat.h"

// Traits =========================================================================================================
template<>
struct std::_Is_integral<paranoise::parallel::m256i>		: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::m256f>	: std::true_type {	};

template<>
struct std::_Is_floating_point<paranoise::parallel::m256d>	: std::true_type {	};

namespace paranoise { namespace parallel {

	// Converting constructors ===================================================================================
	inline m256i::m256i(const m256f& rhs) : m256i(rhs.val) { }
	inline m256i::m256i(const m256i& rhs) : m256i(rhs.val) { }
	inline m256i::m256i(const m256d& rhs) : m256i(rhs.val) { }

	inline m256f::m256f(const m256f& rhs) : m256f(rhs.val) { }
	inline m256f::m256f(const m256i& rhs) : m256f(rhs.val) { }
	inline m256f::m256f(const m256d& rhs) : m256f(rhs.val) { }

	inline m256d::m256d(const m256f& rhs) : m256d(rhs.val) { }
	inline m256d::m256d(const m256i& rhs) : m256d(rhs.val) { }
	inline m256d::m256d(const m256d& rhs) : m256d(rhs.val) { }

	// Load/Store ===============================================================================================	
	inline int32* extract(m256i& src)				{ return src.i32; }
	inline int32* extract(m256i& src, m256f& ref)	{ return src.i32; }
	inline int64* extract(m256i& src, m256d& ref)	{ return src.val.m256i_i64; }

	inline float* extract(m256f& src)				{ return src.values; }
	inline double* extract(m256d& src)				{ return src.values; }

	// Vector dimensions  =======================================================================================
	inline size_t dim(const m256f& val) { return 8; }
	inline size_t dim(const m256i& val) { return 8; }
	inline size_t dim(const m256d& val) { return 4; }

	// Integer SQRT =============================================================================================
	inline m256i sqrt(const m256i& a) { return _mm256_sqrt_ps(m256f(a).val); }

	// Integer DIV ==============================================================================================
	inline m256i operator /(const m256i& a, const m256i& b) { return _mm256_div_ps(m256f(a).val, m256f(b).val); }	
}}
#endif