#pragma once
#ifndef BASETYPES_H
#define BASETYPES_H

#include <functional>
#include "vector.h"

namespace paranoise {
	typedef unsigned	int		uint;

	typedef unsigned	long long	uint64;
	typedef unsigned	int		uint32;
	typedef unsigned	short	uint16;
	typedef unsigned	char	uint8;

	typedef signed		long long	int64;
	typedef signed		int		int32;
	typedef signed		short	int16;
	//typedef signed		char	int8;	


	using namespace paranoise::util;

#define ALIGN(bytes) __declspec(align(bytes))
#define FORCEINLINE __forceinline

// define checked floating and integral type to use on SIMD-enabled functions/types. 
#define SIMD_ENABLE(floatType, intType) \
	template<	typename floatType	= std::enable_if<std::is_floating_point<floatType>::value, floatType>::type, \
				typename intType	= std::enable_if<std::is_integral<intType>::value, intType>::type>

// define checked floating type to use on SIMD-enabled functions/types. 
#define SIMD_ENABLE_F(floatType) \
	template<typename floatType = std::enable_if<std::is_floating_point<floatType>::value, floatType>::type>

// define checked integral type to use on SIMD-enabled functions/types. 
#define SIMD_ENABLE_I(intType) \
	template<typename intType = std::enable_if<std::is_integral<intType>::value, intType>::type>

#define ANY(type) template<typename type>
#define ANY2(type1, type2) template<typename type1, typename type2>

#define UN_OP(op, type) inline type operator op(const type a)				
#define BIN_OP(op, type) inline type operator op(const type a, const type b)
#define SHIFT_OP(op, type) inline type operator op(const type a, const int sa)

#define UN_OP_STUB(op, type, convertable) \
inline friend const type operator op(const convertable a)	{ return op static_cast<type>(a); }		

#define BIN_OP_STUB(op, type, convertable) \
inline friend const type operator op(const type a, const convertable b) { return a op static_cast<type>(b); }

#define FUNC(name, type) inline type name()

#define UN_FUNC(name, type) inline type name(const type a)
#define BIN_FUNC(name, type) inline type name(const type a, const type b)
#define TRI_FUNC(name, type) inline type name(const type a, const type b, const type c)

#define FEATURE(TType, condition) \
template<typename featuremask>\
typename std::enable_if<condition, TType>::type 

#define FEATURE_OP(op, TType, condition) \
FEATURE(TType, condition) inline operator op

#define FEATURE_UN_OP(op, TType, condition) \
FEATURE_OP(op, TType, condition) (const TType a)	

#define FEATURE_BIN_OP(op, TType, condition) \
FEATURE_OP(op, TType, condition) (const TType a, const TType b)

#define FEATURE_SHIFT_OP(op, TType, condition) \
FEATURE_OP(op, TType, condition) (const TType a, const int sa)

#define FEATURE_FUNC(name, TType, condition) \
	FEATURE(TType, condition) inline name

#define FEATURE_UN_FUNC(name, TType, condition) \
FEATURE(TType, condition) name(const TType a)
#define FEATURE_BIN_FUNC(name, TType, condition) \
FEATURE(TType, condition) name(const TType a, const TType b)
#define FEATURE_TRI_FUNC(name, TType, condition) \
FEATURE(TType, condition) name(const TType a, const TType b, const TType c)

#define STDUNP  (a.val)
#define STDBINP (a.val, b.val)
#define STDTRIP(opt) (a.val, b.val, opt)
#define STDDUPP (a.val, a.val)

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

// Constructor arguments (8-vector)
#define VARGS8(type) const type& _0, const type& _1, const type& _2, const type& _3, const type& _4, const type& _5, const type& _6, const type& _7
// Pass 8 constructor arguments
#define VPASS8 _7, _6, _5, _4, _3, _2, _1, _0

//#define SIMD_ENABLE() SIMD_ENABLE(TReal, TInt)

#define CONSTDEF(TType, name, body) constexpr TType name() { return static_cast<TType>(body); }


	SIMD_ENABLE_F(TReal)
		using Module = std::function<TReal(const Vector3<TReal>&)>;

	SIMD_ENABLE_F(TReal)
		using Transformer = std::function<Vector3<TReal>(const Vector3<TReal>&)>;

#define SIMD_FEATURE(condition) \
	template<typename featuremask, typename = std::enable_if<condition, featuremask>::type>

	ANY(TType)
		inline TType id(const TType &a) { return a; }

	SIMD_ENABLE(TReal, TInt)
		using SeededModule = std::function<TReal(const Vector3<TReal>&, const TInt& seed)>;


	template<typename T>
	std::function<typename std::enable_if<std::is_function<T>::value, T>::type> make_function(T *t)
	{
		return{ t };
	}

	template<typename T>
	constexpr size_t dim() { return sizeof(T) >> 2; }

	namespace parallel {

		ANY(TProcess)
			inline TProcess vsel(const bool &condition, const TProcess &choice1, const TProcess &choice2)
		{
			return condition ? choice1 : choice2;
		}

		ANY2(TCondition, TProcess)
			inline Vector3<TProcess> vsel(const Vector3<TCondition> &condition, const Vector3<TProcess> &choice1, const Vector3<TProcess> &choice2)
		{
			return Vector3<TProcess>(
				sel<TCondition, TProcess>(condition.x, choice1.x, choice2.x),
				sel<TCondition, TProcess>(condition.y, choice1.y, choice2.y),
				sel<TCondition, TProcess>(condition.z, choice1.z, choice2.z));
		}

		SIMD_ENABLE_F(TReal)
			inline TReal clamp_int32(const TReal &val)
		{
			return vclamp(val, (TReal)-1073741824.0f, (TReal)1073741824.0f);
		}

		template<typename T, typename U>
		inline Vector3<T> vtrunc(const Vector3<T> &val) { return Vector3<T>(vtrunc<T, U>(val.x), vtrunc<T, U>(val.y), vtrunc<T, U>(val.z)); }

		template<typename T>
		inline Vector3<T> clamp_int32(const Vector3<T> &val)
		{
			return Vector3<T>(clamp_int32<T>(val.x), clamp_int32<T>(val.y), clamp_int32<T>(val.z));
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
		inline int    vsqrt(int a) { return (int)::floor(::sqrt((double)a)); }

		// Fused Multiply-Add		[y = a * b + c]
		ANY(TType) TRI_FUNC(vfmadd, TType) { BODY(a * b + c); }

		// Fused Multiply-Subtract	[y = a * b - c]
		ANY(TType) TRI_FUNC(vfmsub, TType) { BODY(a * b - c); }

		// Clamp: min(max(val, minval), maxval)
		ANY(TType) TRI_FUNC(vclamp, TType) { BODY(vmin(vmax(a, b), c)); }

	};
};
#endif