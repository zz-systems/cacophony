#pragma once
#ifndef BASETYPES_H
#define BASETYPES_H

#include <functional>
//#include "vector.h"

namespace zzsystems { namespace paranoise {


#ifdef _MSC_VER
// prevent 'decorated name length exceeded, name was truncated'
#pragma warning(disable : 4503)
// prevent 'found Intel(R) Advanced Vector Extensions; consider using / arch:AVX'
#pragma warning(disable : 4752)	
#endif


#if _MSC_VER < 1900  
#define ALIGN(bytes) __declspec(align(bytes))
#else 
#define ALIGN(bytes) alignas(bytes)
#endif

#define FORCEINLINE __forceinline

#define SIMD_T vreal, vint
// define checked floating and integral type to use on SIMD-enabled functions/types. 
#define SIMD_ENABLE(floatType, intType) \
	template<	typename floatType	= std::enable_if<std::is_floating_point<floatType>::value, floatType>::type, \
				typename intType	= std::enable_if<std::is_integral<intType>::value, intType>::type>

#define SIMD_ENABLED \
	template<	typename vreal	= std::enable_if<std::is_floating_point<vreal>::value, vreal>::type, \
				typename vint	= std::enable_if<std::is_integral<vint>::value, vint>::type>
// define checked floating type to use on SIMD-enabled functions/types. 
#define SIMD_ENABLE_F(floatType) \
	template<typename floatType = std::enable_if<std::is_floating_point<floatType>::value, floatType>::type>

// define checked integral type to use on SIMD-enabled functions/types. 
#define SIMD_ENABLE_I(intType) \
	template<typename intType = std::enable_if<std::is_integral<intType>::value, intType>::type>

#define ANY(type) template<typename type>
#define ANY2(type1, type2) template<typename type1, typename type2>

#define ASSIGN_OP(op, type) inline type& operator op(type &a, const type &b)
#define ASSIGN_OP2(op, type1, type2) inline type1& operator op(type1 &a, const type2 &b)

#define UN_OP(op, type) inline type operator op(const type &a)				
#define BIN_OP(op, type) inline type operator op(const type &a, const type &b)
#define SHIFT_OP(op, type) inline type operator op(const type &a, const int sa)

#define UN_OP_STUB(op, type, convertable) \
inline friend const type operator op(const convertable &a)	{ return op static_cast<type>(a); }		

#define BIN_OP_STUB_AB(op, type, convertable) \
inline friend type operator op(const type &a, const convertable &b) { return a op static_cast<type>(b); }

#define BIN_OP_STUB_BA(op, type, convertable) \
inline friend type operator op(const convertable &a, const type &b) { return static_cast<type>(a) op b; }

#define BIN_OP_STUB(op, type, convertable) \
BIN_OP_STUB_AB(op, type, convertable) \
BIN_OP_STUB_BA(op, type, convertable)

#define BIN_OP_STUB_ANY(op, type) \
ANY(convertable) BIN_OP_STUB_AB(op, type, convertable) \
ANY(convertable) BIN_OP_STUB_BA(op, type, convertable)

#define FUNC(name, type) inline type name()

#define UN_FUNC(name, type) inline type name(const type &a)
#define BIN_FUNC(name, type) inline type name(const type &a, const type &b)
#define TRI_FUNC(name, type) inline type name(const type &a, const type &b, const type &c)

#define FEATURE(TType, condition) \
template<typename featuremask>\
typename std::enable_if<condition, TType>::type 

#define FEATURE_OP(op, TType, condition) \
FEATURE(TType, condition) inline operator op

#define FEATURE_UN_OP(op, TType, condition) \
FEATURE_OP(op, TType, condition) (const TType &a)	

#define FEATURE_BIN_OP(op, TType, condition) \
FEATURE_OP(op, TType, condition) (const TType &a, const TType &b)

#define FEATURE_SHIFT_OP(op, TType, condition) \
FEATURE_OP(op, TType, condition) (const TType &a, const int sa)

#define FEATURE_FUNC(name, TType, condition) \
	FEATURE(TType, condition) inline name

#define FEATURE_UN_FUNC(name, TType, condition) \
FEATURE(TType, condition) name(const TType &a)
#define FEATURE_BIN_FUNC(name, TType, condition) \
FEATURE(TType, condition) name(const TType &a, const TType &b)
#define FEATURE_TRI_FUNC(name, TType, condition) \
FEATURE(TType, condition) name(const TType &a, const TType &b, const TType &c)

#define STDUNP  (a.val)
#define STDBINP (a.val, b.val)
#define STDTRIP(opt) (a.val, b.val, opt)
#define STDDUPP (a.val, a.val)

//#define ENABLE_DEBUG_FUNC_INSTANCE

#if defined(ENABLE_DEBUG_FUNC_INSTANCE) //&& defined(__DEBUG)
#define DEBUG_FUNC_INSTANCE(name) cout << "for " << static_dispatcher<featuremask>::unit_name() << " using: " << name << endl
#else 
#define DEBUG_FUNC_INSTANCE {}
#endif

// cosmetic...
#define BODY(expr) return expr

// body for 1 argument
#define UN_BODY(intrin) return intrin STDUNP
// body for 1 argument, duplicated
#define UN_BODY_D(intrin) return intrin STDDUPP

// body for 2 arguments
#define BIN_BODY(intrin) return intrin STDBINP
// body for 2 arguments in reversed order
#define BIN_BODY_R(intrin) return intrin STDBINP

// body for 3 arguments
#define TRI_BODY(intrin) return intrin (a.val, b.val, c.val)
// body for 3 arguments in reversed order
#define TRI_BODY_R(intrin) return intrin (c.val, b.val, a.val)

// body for 2 standard and one special argument
#define TRI_BODY_O(intrin, opt) return intrin STDTRIP(opt)

// Constructor arguments (4-vector)
#define VARGS4(type) const type& _0, const type& _1, const type& _2, const type& _3
// Pass 4 constructor arguments
#define VPASS4 _3, _2, _1, _0
#define VPASS4_HI _7, _6, _5, _4

// Constructor arguments (8-vector)
#define VARGS8(type) const type& _0, const type& _1, const type& _2, const type& _3, const type& _4, const type& _5, const type& _6, const type& _7
// Pass 8 constructor arguments
#define VPASS8 _7, _6, _5, _4, _3, _2, _1, _0

//#define SIMD_ENABLE() SIMD_ENABLE(vreal, vint)

#define CONSTDEF(TType, name, body) constexpr TType name() { return static_cast<TType>(body); }


#define SIMD_FEATURE(condition) \
	template<typename featuremask, typename = std::enable_if<condition, featuremask>::type>

		ANY(TType)
			inline TType id(const TType &a) { return a; }



		ANY(T)
			std::function<typename std::enable_if<std::is_function<T>::value, T>::type> make_function(T *t)
		{
			return{ t };
		}
	}
};

namespace zzsystems { namespace simdal
	{
		ANY(T)
			constexpr size_t dim() { return sizeof(T) >> 2; }


		ANY(TProcess)
			inline TProcess vsel(const bool condition, const TProcess &choice1, const TProcess &choice2)
		{
			//cout << "default bool vsel";
			return vsel<bool, TProcess>(condition, choice1, choice2);
		}

		ANY2(TCondition, TProcess)
			inline TProcess vsel(const TCondition condition, const TProcess &choice1, const TProcess &choice2)
		{
			//cout << "default vsel";
			return condition ? choice1 : choice2;
		}	

		// Fused Multiply-Add		[y = a * b + c]
		ANY(TType) TRI_FUNC(vfmadd, TType) { BODY(a * b + c); }

		// Fused Multiply-Subtract	[y = a * b - c]
		ANY(TType) TRI_FUNC(vfmsub, TType) { BODY(a * b - c); }

		// Clamp: min(max(val, minval), maxval)
		ANY(TType) TRI_FUNC(vclamp, TType) { BODY(vmin(vmax(a, b), c)); }

		SIMD_ENABLE_F(vreal)
			inline vreal clamp_int32(const vreal &val)
		{
			return vclamp<vreal>(val, -1073741824.0, 1073741824.0);
		}


		inline double vabs(double a) { return ::fabs(a); }
		inline float  vabs(float a) { return ::fabsf(a); }
		inline int	  vabs(int a) { return ::abs(a); }

		ANY(TType) BIN_FUNC(vmin, TType) { BODY(std::min(a, b)); }

		ANY(TType) BIN_FUNC(vmax, TType) { BODY(std::max<TType>(a, b)); }

		inline double vfloor(double a) { return std::floor(a); }
		inline float vfloor(float a) { return std::floorf(a); }

		inline double vceil(double a) { return std::ceil(a); }
		inline float vceil(float a) { return std::ceilf(a); }

		inline double vround(double a) { return std::round(a); }
		inline float vround(float a) { return std::roundf(a); }

		inline double vsqrt(double a) { return ::sqrt(a); }
		inline float  vsqrt(float a) { return ::sqrtf(a); }
		inline int    vsqrt(int a) { return static_cast<int>(::floor(::sqrt(static_cast<double>(a)))); }
	}
}
#endif