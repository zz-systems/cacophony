#pragma once
#ifndef PARANOISE_UTIL_H
#define PARANOISE_UTIL_H

#include "CppUnitTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define REDIR_CERR_BEGIN \ 
std::streambuf *cerrbuf = std::cerr.rdbuf(); \
std::stringstream sstream; \
std::cerr.rdbuf(sstream.rdbuf());

#define REDIR_CERR_END \
code \
Logger::WriteMessage(sstream.str().c_str()); \
std::cerr.rdbuf(cerrbuf);

#endif