#include "stdafx.h"
#include "CppUnitTest.h"

#include "../libnoise++/interpolation.h"

#define TEST_USE_THREADS
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace noisepp::interpolation;
using namespace noisepp::intrinsic;

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
				auto a = _a / 2.0;
#else
			for (double a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif
			for (double n3 = -2;	n3 < 2;		n3 += 0.25)			// 16		
			for (double n2 = -50;	n2 < 50;	n2 += 10)			// 10			
			for (double n1 = -25;	n1 < 25;	n1 += 10)			// 5				
			for (double n0 = -1;	n0 < 1;		n0 += 0.25)			// 8
			{				
				auto d_reference	= InterpolateCubic<double>(n0, n1, n2, n3, a);
				auto f_reference	= (float)d_reference;

				m128f	sse_floats	= InterpolateCubic(m128f(n0),	m128f(n1),	m128f(n2),	m128f(n3),	m128f(a));
				m128d	sse_doubles	= InterpolateCubic(m128d(n0),	m128d(n1),	m128d(n2),	m128d(n3),	m128d(a));

				m256f	avx_floats	= InterpolateCubic(m256f(n0),	m256f(n1),	m256f(n2),	m256f(n3),	m256f(a));
				m256d	avx_doubles	= InterpolateCubic(m256d(n0),	m256d(n1),	m256d(n2),	m256d(n3),	m256d(a));

				Assert::AreEqual(f_reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, sse_doubles.values[0],	std::numeric_limits<double>::epsilon());
				Assert::AreEqual(f_reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, avx_doubles.values[0],	std::numeric_limits<double>::epsilon());
			}	
#ifdef TEST_USE_THREADS
		});
#endif
		}

	TEST_METHOD(Test_InterpolateLinear)
		{
#ifdef TEST_USE_THREADS
			parallel_for(-5, 5, [&](int _a) 
			{
				auto a = _a / 2.0;
#else
			for (double a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif					
			for (double n1 = -25;	n1 < 25;	n1 += 10)			// 5				
			for (double n0 = -1;	n0 < 1;		n0 += 0.25)			// 8
			{
				auto d_reference	= InterpolateLinear<double>(n0, n1, a);
				auto f_reference	= (float)d_reference;

				m128f	sse_floats	= InterpolateLinear(m128f(n0),	m128f(n1),	m128f(a));
				m128d	sse_doubles	= InterpolateLinear(m128d(n0),	m128d(n1),	m128d(a));

				m256f	avx_floats	= InterpolateLinear(m256f(n0),	m256f(n1),	m256f(a));
				m256d	avx_doubles	= InterpolateLinear(m256d(n0),	m256d(n1),	m256d(a));

				Assert::AreEqual(f_reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, sse_doubles.values[0],	std::numeric_limits<double>::epsilon());
				Assert::AreEqual(f_reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, avx_doubles.values[0],	std::numeric_limits<double>::epsilon());
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
				auto a = _a / 2.0;
#else
			for (double a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif		
			{
				auto d_reference	= SCurve3<double>(a);
				auto f_reference	= (float)d_reference;

				m128f	sse_floats	= SCurve3(m128f(a));
				m128d	sse_doubles	= SCurve3(m128d(a));

				m256f	avx_floats	= SCurve3(m256f(a));
				m256d	avx_doubles	= SCurve3(m256d(a));

				Assert::AreEqual(f_reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, sse_doubles.values[0],	std::numeric_limits<double>::epsilon());
				Assert::AreEqual(f_reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, avx_doubles.values[0],	std::numeric_limits<double>::epsilon());
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
				auto a = _a / 2.0;
#else
			for (double a = -2.5;	a < 2.5;	a += 0.5)			// 10		
#endif		
			{
				auto d_reference	= SCurve5<double>(a);
				auto f_reference	= (float)d_reference;

				m128f	sse_floats	= SCurve5(m128f(a));
				m128d	sse_doubles	= SCurve5(m128d(a));

				m256f	avx_floats	= SCurve5(m256f(a));
				m256d	avx_doubles	= SCurve5(m256d(a));

				Assert::AreEqual(f_reference, sse_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, sse_doubles.values[0],	std::numeric_limits<double>::epsilon());
				Assert::AreEqual(f_reference, avx_floats.values[0],		std::numeric_limits<float>::epsilon());
				Assert::AreEqual(d_reference, avx_doubles.values[0],	std::numeric_limits<double>::epsilon());
			}	
#ifdef TEST_USE_THREADS
		});
#endif
		}
	};
}