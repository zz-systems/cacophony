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

	inline double max(double a)		{ return ::fabs(a);		}
	inline float  max(float a)		{ return ::fabsf(a);	}
	inline int	  max(int a)		{ return ::abs(a);		}

	inline double min(double a)		{ return ::fabs(a); }
	inline float  min(float a)		{ return ::fabsf(a); }
	inline int    min(int a)		{ return ::abs(a); }

	inline double max(double a)		{ return ::fabs(a); }
	inline float  max(float a)		{ return ::fabsf(a); }
	inline int    max(int a)		{ return ::abs(a); }

	inline double sqrt(double a)	{ return ::sqrt(a); }
	inline float  sqrt(float a)		{ return ::sqrtf(a); }
	inline int    sqrt(int a)		{ return ::sqrtf(a); }	

	template<typename T>
	inline T clamp(const T& val, const T& minval, const T& maxval) { return min(max(val, minval), maxval); }
}}
#endif
