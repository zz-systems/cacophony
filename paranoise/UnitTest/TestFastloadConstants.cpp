#include "stdafx.h"
#include "CppUnitTest.h"

#include "../paranoise/parallel/all.h"
#include "../paranoise/interpolation.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//AreEqual(static_cast<float>	(expected), fastload<float>::name(),	L"scalar float invalid"); 
//AreEqual(static_cast<int32>	(expected), fastload<int32>::name(),		L"scalar int invalid"); 

using namespace paranoise;
using namespace interpolation;
using namespace parallel;
using namespace test;

namespace UnitTest
{

#define VALIDATE_FASTLOAD_VALUE(name, expected) \
	AreEqual(static_cast<float>	(expected), fastload<float>::name(), L"scalar float invalid");\
	AreEqual(static_cast<int>	(expected), fastload<int>::name(), L"scalar int invalid");\
	AreEqual(static_cast<float>	(expected), fastload<sse_real>::name(), L"sse float[4] invalid"); \
	AreEqual(static_cast<int>	(expected), fastload<sse_int>::name(),  L"sse int[4] invalid"); \
	AreEqual(static_cast<float>	(expected), fastload<avx_real>::name(),	L"avx float[8] invalid"); \
	AreEqual(static_cast<int>	(expected), fastload<avx_int>::name(),	L"avx int[8] invalid")


	TEST_CLASS(TestFastLoadCostants)
	{
	public:
		
		TEST_METHOD(Test_Fastload_0)
		{
			VALIDATE_FASTLOAD_VALUE(_0, 0);
		}

		TEST_METHOD(Test_Fastload_1)
		{
			VALIDATE_FASTLOAD_VALUE(_1, 1);
		}

		TEST_METHOD(Test_Fastload_2)
		{
			VALIDATE_FASTLOAD_VALUE(_2, 2);
		}

		TEST_METHOD(Test_Fastload_3)
		{
			VALIDATE_FASTLOAD_VALUE(_3, 3);
		}

		TEST_METHOD(Test_Fastload_4)
		{
			VALIDATE_FASTLOAD_VALUE(_4, 4);
		}

		TEST_METHOD(Test_Fastload_5)
		{
			VALIDATE_FASTLOAD_VALUE(_5, 5);
		}

		TEST_METHOD(Test_Fastload_6)
		{
			VALIDATE_FASTLOAD_VALUE(_6, 6);
		}

		TEST_METHOD(Test_Fastload_7)
		{
			VALIDATE_FASTLOAD_VALUE(_7, 7);
		}

		TEST_METHOD(Test_Fastload_8)
		{
			VALIDATE_FASTLOAD_VALUE(_8, 8);
		}

		TEST_METHOD(Test_Fastload_9)
		{
			VALIDATE_FASTLOAD_VALUE(_9, 9);
		}

		TEST_METHOD(Test_Fastload_10)
		{
			VALIDATE_FASTLOAD_VALUE(_10, 10);
		}

		TEST_METHOD(Test_Fastload_11)
		{
			VALIDATE_FASTLOAD_VALUE(_11, 11);
		}

		TEST_METHOD(Test_Fastload_12)
		{
			VALIDATE_FASTLOAD_VALUE(_12, 12);
		}

		TEST_METHOD(Test_Fastload_13)
		{
			VALIDATE_FASTLOAD_VALUE(_13, 13);
		}

		TEST_METHOD(Test_Fastload_14)
		{
			VALIDATE_FASTLOAD_VALUE(_14, 14);
		}

		TEST_METHOD(Test_Fastload_15)
		{
			VALIDATE_FASTLOAD_VALUE(_15, 15);
		}
	};
}