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
	typedef signed		char	int8;
	
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

//#define SIMD_ENABLE() SIMD_ENABLE(TReal, TInt)
	

SIMD_ENABLE_F(TReal)
using Module = std::function<TReal(const Vector3<TReal>&)>;

SIMD_ENABLE(TReal, TInt)
using SeededModule = std::function<TReal(const Vector3<TReal>&, const TInt& seed)>;

	template<typename T>
	std::function<typename std::enable_if<std::is_function<T>::value, T>::type> make_function(T *t) 
	{
		return{ t };
	}
};

#endif