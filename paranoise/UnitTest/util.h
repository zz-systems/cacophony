#pragma once
#ifndef PARANOISE_UTIL_H
#define PARANOISE_UTIL_H

#include "CppUnitTest.h"
#include <iostream>
#include <functional>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


//#define REDIR_CERR_BEGIN \ 
//std::streambuf *cerrbuf = std::cerr.rdbuf(); \
//std::stringstream sstream; \
//std::cerr.rdbuf(sstream.rdbuf());
//
//#define REDIR_CERR_END \
//Logger::WriteMessage(sstream.str().c_str()); \
//std::cerr.rdbuf(cerrbuf);

namespace zzsystems { namespace unittest {
	using namespace simdal;

	using capability_avx		= integral_constant<int, CAPABILITY_AVX1 | CAPABILITY_AVX2 >;
	using capability_sse4fma	= integral_constant<int, CAPABILITY_SSE2 | CAPABILITY_SSE3 | CAPABILITY_SSE41 | CAPABILITY_FMA3>;
	using capability_sse4		= integral_constant<int, CAPABILITY_SSE2 | CAPABILITY_SSE3 | CAPABILITY_SSE41>;
	using capability_sse		= integral_constant<int, CAPABILITY_SSE2 | CAPABILITY_SSE3>;
	using capability_fpu		= integral_constant<int, CAPABILITY_NONE>;

	using sse_real				= static_dispatcher<capability_sse4fma>::real_type;
	using sse_int				= static_dispatcher<capability_sse4fma>::integral_type;
	using avx_real				= static_dispatcher<capability_avx>::real_type;
	using avx_int				= static_dispatcher<capability_avx>::integral_type;


	template<typename TScalar>
	inline void AreEqual(const TScalar &expected, TScalar &tested)
	{
		Assert::AreEqual(expected, tested, 0.001f);
	}
	
	template<typename TVec>
	inline void AreEqual(const int &expected, const TVec &tested, const wchar_t* message = NULL)
	{
		auto word = sizeof(TVec) >> 2;
		remove_const_t<TVec>  e = tested;
		auto extracted = extract(e);

		for (int i = 0; i < word; i++)
		{
			Assert::AreEqual(expected, extracted[i],  message);
		}
	}

	template<typename TVec>
	inline void AreEqual(const float &expected, const TVec &tested, const wchar_t* message = NULL)
	{
		auto word = sizeof(TVec) >> 2;

		remove_const_t<TVec> e = tested;	

		auto extracted = extract(e);

		for (int i = 0; i < word; i++)
		{
			Assert::AreEqual(expected, extracted[i], 0.001f, message);			
		}
	}

	template<typename TScalar, typename TVec>
	inline void AreEqual_NOE(const TScalar &expected, const TVec &tested)
	{
		auto word = sizeof(TVec) >> 2;

		remove_const_t<TVec> e = tested;
		auto extracted = extract(e);

		for (int i = 0; i < word; i++)
		{
			Assert::AreEqual(expected, extracted[i]);
		}
	}
}}
#endif