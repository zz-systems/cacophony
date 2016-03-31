#pragma once
#ifndef PARANOISE_ALL_H
#define PARANOISE_ALL_H

#include "dependencies.h"
#include "sse.h"
#include "avx.h"
#include "opencl.h"
#include "SISD.h"

#include "../vector.h"
namespace std
{
	template <class T>
	inline void hash_combine(size_t& seed, const T& v)
	{
		hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<typename featuremask>
	struct hash<zzsystems::simdal::_float4>
	{
		typedef zzsystems::simdal::_float4 argument_type;
		typedef size_t result_type;

		result_type operator()(const argument_type &arg) const
		{
			float *v = zzsystems::simdal::extract(arg);

			result_type result = 0;

			for (int i = 0; i < zzsystems::simdal::dim<argument_type>(); i++)
				hash_combine<float>(result, v[i]);

			return result;
		}
	};

	template<typename featuremask>
	struct hash<zzsystems::simdal::_float8>
	{
		typedef zzsystems::simdal::_float8 argument_type;
		typedef size_t result_type;

		result_type operator()(const argument_type &arg) const
		{
			float *v = extract(arg);

			result_type result = 0;

			for (int i = 0; i < zzsystems::simdal::dim<argument_type>(); i++)
				hash_combine<float>(result, v[i]);

			return result;
		}
	};

	template<typename T>
	struct hash<zzsystems::paranoise::util::Vector3<T>>
	{
		typedef zzsystems::paranoise::util::Vector3<T> argument_type;
		typedef size_t result_type;

		result_type operator()(const argument_type &arg) const
		{
			result_type result = 0;
			
			hash_combine<T>(result, arg.x);
			hash_combine<T>(result, arg.y);
			hash_combine<T>(result, arg.z);

			return result;
		}
	};
}

namespace zzsystems { namespace simdal {
	
	template<typename, typename enable = void>
	struct static_dispatcher
	{
		static_assert(!is_same<enable, void>::value, "not expected");

		typedef int	integral_type;
		typedef float	real_type;
		static constexpr const char* unit_name = "default FPU";
	};

	template<typename featuremask>
	struct static_dispatcher<featuremask, 
		typename enable_if<!_dispatcher::has_sse && !_dispatcher::has_avx2>::type>
	{
		typedef int		integral_type;
		typedef float	real_type;
		static constexpr const char* unit_name = "FPU";
	};

	template<typename featuremask>
	struct static_dispatcher<featuremask, 
		typename enable_if<_dispatcher::has_sse && !_dispatcher::has_avx2>::type>
	{
		typedef _int4 integral_type;
		typedef _float4 real_type;
		static constexpr const char* unit_name = "SSE";
	};

	template<typename featuremask>
	struct static_dispatcher<featuremask, 
		typename enable_if<_dispatcher::has_avx && _dispatcher::has_avx2>::type>
	{
		typedef _int8 integral_type;
		typedef _float8 real_type;
		static constexpr const char* unit_name = "AVX";
	};	

	template<typename capability>
	struct simd_traits
	{
		typedef typename static_dispatcher<capability>::real_type		vreal;
		typedef typename static_dispatcher<capability>::integral_type	vint;
	};

#define SIMD_DISPATCH_TYPES \
	using vreal = static_dispatcher<capability>::real_type;\
	using vint = static_dispatcher<capability>::integral_type

	using capability_avx		= integral_constant<int, CAPABILITY_FASTFLOAT | CAPABILITY_AVX1 | CAPABILITY_AVX2 >; 
	using capability_sse4fma	= integral_constant<int, CAPABILITY_FASTFLOAT | CAPABILITY_SSE2 | CAPABILITY_SSE3 | CAPABILITY_SSE41 | CAPABILITY_FMA3>;
	using capability_sse4		= integral_constant<int, CAPABILITY_FASTFLOAT | CAPABILITY_SSE2 | CAPABILITY_SSE3 | CAPABILITY_SSE41>;
	using capability_sse		= integral_constant<int, CAPABILITY_FASTFLOAT | CAPABILITY_SSE2 | CAPABILITY_SSE3>;
	using capability_fpu		= integral_constant<int, CAPABILITY_NONE>;

#define AVX_BRANCH \
	using capability = capability_avx; \
	SIMD_DISPATCH_TYPES

#define SSE4FMA_BRANCH \
	using capability = capability_sse4fma; \
	SIMD_DISPATCH_TYPES

#define SSE4_BRANCH \
	using capability = capability_sse4;\
	SIMD_DISPATCH_TYPES

#define SSE_BRANCH \
	using capability = capability_sse;\
	SIMD_DISPATCH_TYPES

#define FPU_BRANCH \
	using capability = capability_fpu;\
	SIMD_DISPATCH_TYPES

#define SIMD_DISPATCH(sysinfo, body) \
	do { \
		if (sysinfo.hasAVX2()) \
		{ \
			AVX_BRANCH;\
			body;\
		} \
		else if (sysinfo.hasSSE4FMA()) \
		{ \
			SSE4FMA_BRANCH;\
			body;\
		} \
		else if (sysinfo.hasSSE4()) \
		{ \
			SSE4_BRANCH;\
			body;\
		} \
		else if (sysinfo.hasSSE()) \
		{ \
			SSE_BRANCH;\
			body;\
		} \
		else \
		{ \
			FPU_BRANCH; \
			body;\
		} \
	} while(false)

}}
#endif