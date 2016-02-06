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
//code \
//Logger::WriteMessage(sstream.str().c_str()); \
//std::cerr.rdbuf(cerrbuf);


template<typename TScalar, typename TVec, typename... Args>
void paranoise_assert_implementation_equality(std::function<TScalar(Args...)> expected, std::function<TVec(Args...)> tested, Args&&... args)
{
	auto ref = expected(args);
	auto val = tested(args);

	auto word = sizeof(TVec) >> 2;

	for (int i = 0; i < word; i++)
	{
		Assert::AreEqual(ref, extract(val)[i], std::numeric_limits<TScalar>::epsilon());
	}
}
#endif