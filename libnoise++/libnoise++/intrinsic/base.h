#pragma once

#ifndef NOISEPP_INTRINSIC_BASE_H
#define NOISEPP_INTRINSIC_BASE_H

#include "../basetypes.h"

#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <type_traits>

#define ALIGN(bytes) __declspec(align(bytes))

namespace noisepp { namespace intrinsic {	

	inline double abs(double a) { return ::fabs(a); }
	inline float  abs(float a)	{ return ::fabsf(a); }
	inline int    abs(int a)	{ return ::abs(a); }

	template<typename T>
	inline T clamp(const T& a, const T& min, const T& max) { return a < min ? min : a > max ? max : a; }
}}
#endif
