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
	
	const float PI = 3.14159265358979323846;
	const float DEG2RAD = PI / 180.0;
	const float SQRT_3  = sqrtf(3);

#define ALIGN(bytes) __declspec(align(bytes))

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

//#define SIMD_ENABLE() SIMD_ENABLE(TReal, TInt)
	

SIMD_ENABLE_F(TReal)
using Module = std::function<TReal(const Vector3<TReal>&)>;

SIMD_ENABLE_F(TReal)
using Transformer = std::function<Vector3<TReal>(const Vector3<TReal>&)>;

ANY(TType)
inline TType id (const TType &a) { return a; }

SIMD_ENABLE(TReal, TInt)
using SeededModule = std::function<TReal(const Vector3<TReal>&, const TInt& seed)>;

	template<typename T>
	std::function<typename std::enable_if<std::is_function<T>::value, T>::type> make_function(T *t) 
	{
		return{ t };
	}

	namespace parallel {
		ANY(TProcess)
			inline TProcess sel(const bool &condition, const TProcess &choice1, const TProcess &choice2)
		{
			return condition ? choice1 : choice2;
		}

		ANY2(TCondition, TProcess)
			inline Vector3<TProcess> sel(const Vector3<TCondition> &condition, const Vector3<TProcess> &choice1, const Vector3<TProcess> &choice2)
		{
			return Vector3<TProcess>(
				sel<TCondition, TProcess>(condition.x, choice1.x, choice2.x),
				sel<TCondition, TProcess>(condition.y, choice1.y, choice2.y),
				sel<TCondition, TProcess>(condition.z, choice1.z, choice2.z));
		}

		ANY2(TCondition, TProcess)
			inline TProcess sel(const TCondition &condition, const TProcess &choice1, const TProcess &choice2)
		{
			return (condition & choice1) | ((~condition) & choice2);
		}

		//SIMD_ENABLE(TReal, TInt)
		//inline TReal truncate(const TReal &val) { return (TReal)(TInt)val; }

		SIMD_ENABLE_F(TReal)
			inline TReal clamp_int32(const TReal &val)
		{
			return clamp(val, (TReal)-1073741824.0f, (TReal)1073741824.0f);
		}

		inline double abs(double a) { return ::fabs(a); }
		inline float  abs(float a) { return ::fabsf(a); }
		inline int	  abs(int a) { return ::abs(a); }

		ANY(TType)
			inline TType min(TType a, TType b) { return std::min(a, b); }

		ANY(TType)
			inline TType max(TType a, TType b) { return std::max(a, b); }

		inline double floor(double a) { return std::floor(a); }
		inline float floor(float a) { return std::floorf(a); }

		inline double ceil(double a) { return std::ceil(a); }
		inline float ceil(float a) { return std::ceilf(a); }

		inline double round(double a) { return std::round(a); }
		inline float round(float a) { return std::roundf(a); }

		inline double sqrt(double a) { return ::sqrt(a); }
		inline float  sqrt(float a) { return ::sqrtf(a); }
		inline int    sqrt(int a) { return (int)::floor(::sqrt((double)a)); }

		ANY(TType)
			inline TType clamp(const TType& val, const TType& minval, const TType& maxval) { return min(max(val, minval), maxval); }
	};
};
#endif