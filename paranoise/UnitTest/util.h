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


template<typename TScalar, typename TVec>
inline void AreEqual(const TScalar &expected, const TVec &tested)
{
	auto word = sizeof(TVec) >> 2;
	TVec e = tested;
	auto extracted = extract(e);

	for (int i = 0; i < word; i++)
	{
		Assert::AreEqual(expected, extracted[i], std::numeric_limits<TScalar>::epsilon());		
	}
}

template<typename TScalar, typename TVec>
inline void AreEqual_NOE(const TScalar &expected, const TVec &tested)
{
	auto word = sizeof(TVec) >> 2;
	TVec e = tested;
	auto extracted = extract(e);

	for (int i = 0; i < word; i++)
	{
		Assert::AreEqual(expected, extracted[i]);
	}
}

template<typename TScalar>
inline void AreEqual(const TScalar &expected, const TScalar &tested)
{
	Assert::AreEqual(expected, tested, std::numeric_limits<TScalar>::epsilon());
}
#endif