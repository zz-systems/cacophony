#pragma once

#ifndef PARANOISE_INTRINSIC_BASE_H
#define PARANOISE_INTRINSIC_BASE_H

#include "../basetypes.h"

#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <type_traits>
#include <algorithm>

namespace paranoise { namespace parallel {	

	inline double abs(double a)		{ return ::fabs(a);		}
	inline float  abs(float a)		{ return ::fabsf(a);	}
	inline int	  abs(int a)		{ return ::abs(a);		}

	template<typename T>
	inline T min(T a, T b)			{ return std::min(a, b); }

	template<typename T>
	inline T max(T a, T b)			{ return std::max(a, b); }

	inline double sqrt(double a)	{ return ::sqrt(a); }
	inline float  sqrt(float a)		{ return ::sqrtf(a); }
	inline int    sqrt(int a)		{ return (int)::floor(::sqrt((double)a)); }	

	template<typename T>
	inline T clamp(const T& val, const T& minval, const T& maxval) { return min(max(val, minval), maxval); }

	SIMD_ENABLE(TReal, TInt)
		inline TReal truncate(const TReal &val) { return (TReal)(TInt)val; }

#define SIMD_ANY(type) template<typename type>

	SIMD_ANY(TType) inline TType &operator +=(TType& a, const TType& b) { a = (a + b); return a; }
	SIMD_ANY(TType) inline TType &operator -=(TType& a, const TType& b) { a = (a - b); return a; }
	SIMD_ANY(TType) inline TType &operator *=(TType& a, const TType& b) { a = (a * b); return a; }
	SIMD_ANY(TType) inline TType &operator /=(TType& a, const TType& b) { a = (a / b); return a; }

	SIMD_ANY(TType) inline TType &operator !=(TType& a, const TType& b) { a = ~(a == b); return a; }
	SIMD_ANY(TType) inline TType &operator &=(TType& a, const TType& b) { a = (a & b); return a; }
	SIMD_ANY(TType) inline TType &operator |=(TType& a, const TType& b) { a = (a | b); return a; }
	SIMD_ANY(TType) inline TType &operator ^=(TType& a, const TType& b) { a = (a ^ b); return a; }

	SIMD_ANY(TType) inline TType operator >>=(TType& a, int amount) { a = a >> amount; return a; }
	SIMD_ANY(TType) inline TType operator <<=(TType& a, int amount) { a = a << amount; return a; }
}}
#endif
