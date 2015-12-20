#pragma once
#ifndef INTRINSIC_H 
#define INTRINSIC_H

#include "basetypes.h"
#include "intrinsic\m128i.h"
#include "intrinsic\m128f.h"
#include "intrinsic\m128d.h"

#include "intrinsic\m256i.h"
#include "intrinsic\m256f.h"
#include "intrinsic\m256d.h"


template<>
struct std::_Is_integral<noisepp::intrinsic::m128i>
	: std::true_type
{	};

template<>
struct std::_Is_integral<noisepp::intrinsic::m256i>
	: std::true_type
{	};

template<>
struct std::_Is_floating_point<noisepp::intrinsic::m128f>
	: std::true_type
{	};

template<>
struct std::_Is_floating_point<noisepp::intrinsic::m128d>
	: std::true_type
{	};

template<>
struct std::_Is_floating_point<noisepp::intrinsic::m256f>
	: std::true_type
{	};

template<>
struct std::_Is_floating_point<noisepp::intrinsic::m256d>
	: std::true_type
{	};

namespace noisepp {	namespace intrinsic {	

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

	//template<typename IGNORE>
	inline int32* extract(int32& src/*, IGNORE& ref*/)	{ return &src; }

	inline int32* extract(m128i& src)				{ return src.i32; }
	inline int32* extract(m128i& src, m128f& ref)	{ return src.i32; }
	inline int64* extract(m128i& src, m128d& ref)	{ return src.val.m128i_i64; }

	inline int32* extract(m256i& src)				{ return src.i32; }
	inline int32* extract(m256i& src, m256f& ref)	{ return src.i32; }
	inline int64* extract(m256i& src, m256d& ref)	{ return src.val.m256i_i64; }

	inline float* extract(float& src)		{ return &src; }
	inline float* extract(m128f& src)		{ return src.values; }
	inline float* extract(m256f& src)		{ return src.values; }

	inline double* extract(double& src)		{ return &src; }
	inline double* extract(m128d& src)		{ return src.values; }
	inline double* extract(m256d& src)		{ return src.values; }

	// Sizes ====================================================================================================
	/*template<typename T>
	inline size_t elem_count(const T& val);*/

	//template<>
	inline size_t elem_count(const int32& val) { return 1; }

	//template<>
	inline size_t elem_count(const float& val) { return 1; }

	//template<>
	inline size_t elem_count(const double& val) { return 1; }

	//template<>
	inline size_t elem_count(const m128f& val) { return 4; }

	//template<>
	inline size_t elem_count(const m128i& val) { return 4; }

	//template<>
	inline size_t elem_count(const m128d& val) { return 2; }

	//template<>
	inline size_t elem_count(const m256f& val) { return 8; }

	//template<>
	inline size_t elem_count(const m256i& val) { return 8; }

	//template<>
	inline size_t elem_count(const m256d& val) { return 4; }
	// Conversion ===============================================================================================
		
}}

#endif 