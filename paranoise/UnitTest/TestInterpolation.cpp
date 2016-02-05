#include "stdafx.h"
#include "CppUnitTest.h"

#include "../paranoise/parallel/all.h"
#include "../paranoise/interpolation.h"

#define TEST_USE_THREADS
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace paranoise::interpolation;
using namespace paranoise::parallel;

#ifdef TEST_USE_THREADS
#include <ppl.h>
using namespace concurrency;
#endif

namespace UnitTest
{	
	TEST_CLASS(TestInterpolation)
	{
	public:
		
		TEST_METHOD(Test_InterpolateCubic)
		{
#ifdef TEST_USE_THREADS
			parallel_for(-5, 5, [&](int _a) 
			{
				auto a = _a / 2.0f;
#else
			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif
			for (float n3 = -2;	n3 < 2;		n3 += 0.25)			// 16		
			for (float n2 = -50;	n2 < 50;	n2 += 10)			// 10			
			for (float n1 = -25;	n1 < 25;	n1 += 10)			// 5				
			for (float n0 = -1;	n0 < 1;		n0 += 0.25)			// 8
			{				
				auto reference	= cerp<float>(n0, n1, n2, n3, a);

				auto	sse_floats	= cerp<float4>(n0, n1, n2, n3, a);

				auto	avx_floats	= cerp<float8>(n0, n1, n2, n3, a);

				Assert::AreEqual(reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
			}	
#ifdef TEST_USE_THREADS
		});
#endif
		}

	TEST_METHOD(Test_lerp)
		{
#ifdef TEST_USE_THREADS
			parallel_for(-5, 5, [&](int _a) 
			{
				auto a = _a / 2.0f;
#else
			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif					
			for (float n1 = -25;	n1 < 25;	n1 += 10)			// 5				
			for (float n0 = -1;	n0 < 1;		n0 += 0.25)			// 8
			{
				auto reference	= lerp<float>(n0, n1, a);

				auto	sse_floats	= lerp<float4>(n0,	n1,	a);

				auto	avx_floats	= lerp<float4>(n0,	n1,	a);

				Assert::AreEqual(reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
			}	
#ifdef TEST_USE_THREADS
		});
#endif
		}

TEST_METHOD(Test_SCurve3)
		{
#ifdef TEST_USE_THREADS
			parallel_for(-5, 5, [&](int _a) 
			{
				auto a = _a / 2.0f;
#else
			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif		
			{
				auto reference	= scurve3<float>(a);

				auto	sse_floats	= scurve3<float4>(a);
				auto	avx_floats	= scurve3<float8>(a);

				Assert::AreEqual(reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
			}	
#ifdef TEST_USE_THREADS
		});
#endif
		}

		TEST_METHOD(Test_SCurve5)
		{
#ifdef TEST_USE_THREADS
			parallel_for(-5, 5, [&](int _a) 
			{
				auto a = _a / 2.0f;
#else
			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif		
			{
				auto reference	= scurve5<float>(a);

				auto	sse_floats	= scurve5<float4>(a);
				auto	avx_floats	= scurve5<float8>(a);

				Assert::AreEqual(reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
			}	
#ifdef TEST_USE_THREADS
		});
#endif
		}
	};
}