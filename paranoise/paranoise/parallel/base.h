#pragma once

#ifndef PARANOISE_INTRINSIC_BASE_H
#define PARANOISE_INTRINSIC_BASE_H

#include "../basetypes.h"

#include <intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <type_traits>
#include <algorithm>

namespace paranoise { namespace parallel {	

	template<typename T> constexpr T one() { return T(1); }

	ANY(TType) inline TType &operator +=(TType& a, const TType& b) { a = (a + b); return a; }
	ANY(TType) inline TType &operator -=(TType& a, const TType& b) { a = (a - b); return a; }
	ANY(TType) inline TType &operator *=(TType& a, const TType& b) { a = (a * b); return a; }
	ANY(TType) inline TType &operator /=(TType& a, const TType& b) { a = (a / b); return a; }

	ANY2(TType1, TType2) inline TType1 &operator +=(TType1& a, const TType2& b) { a = (a + static_cast<TType1>(b)); return a; }
	ANY2(TType1, TType2) inline TType1 &operator -=(TType1& a, const TType2& b) { a = (a - static_cast<TType1>(b)); return a; }
	ANY2(TType1, TType2) inline TType1 &operator *=(TType1& a, const TType2& b) { a = (a * static_cast<TType1>(b)); return a; }
	ANY2(TType1, TType2) inline TType1 &operator /=(TType1& a, const TType2& b) { a = (a / static_cast<TType1>(b)); return a; }

	ANY(TType) inline TType &operator !=(TType& a, const TType& b) { a = ~(a == b); return a; }
	
	ANY(TType) inline TType &operator &=(TType& a, const TType& b) { a = (a & b); return a; }
	ANY(TType) inline TType &operator |=(TType& a, const TType& b) { a = (a | b); return a; }
	ANY(TType) inline TType &operator ^=(TType& a, const TType& b) { a = (a ^ b); return a; }

	ANY2(TType1, TType2) inline TType1 &operator &=(TType1& a, const TType2& b) { a = (a & static_cast<TType1>(b)); return a; }
	ANY2(TType1, TType2) inline TType1 &operator |=(TType1& a, const TType2& b) { a = (a | static_cast<TType1>(b)); return a; }
	ANY2(TType1, TType2) inline TType1 &operator ^=(TType1& a, const TType2& b) { a = (a ^ static_cast<TType1>(b)); return a; }

	ANY(TType) inline TType operator >>=(TType& a, int amount) { a = a >> amount; return a; }
	ANY(TType) inline TType operator <<=(TType& a, int amount) { a = a << amount; return a; }

	ANY(TType) inline TType operator >=(const TType& a, const TType& b) { return !(a < b); }
	ANY(TType) inline TType operator <=(const TType& a, const TType& b) { return !(a > b); }

	ANY(TType1) inline TType1 create_mask(int mask) { return static_cast<TType1>(mask); }
}}
#endif
