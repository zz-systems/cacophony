#include "stdafx.h"
#include "CppUnitTest.h"
#include "../libnoise++/interpolation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace noisepp::interpolation;
using namespace noisepp::intrinsic;

namespace UnitTest
{	
	TEST_CLASS(TestInterpolation)
	{
	public:
		
		TEST_METHOD(Test_InterpolateCubic)
		{
			auto d_reference	= InterpolateCubic<double>(1.0, 2.0, 4.0, 5.0, 1.0);
			auto f_reference	= (float)d_reference;
				
			__m128	sse_floats	= InterpolateCubic(ld1<__m128>(1.0),	ld1<__m128>(2.0),	ld1<__m128>(4.0),	ld1<__m128>(5.0),	ld1<__m128>(1.0));
			__m128d sse_doubles = InterpolateCubic(ld1<__m128d>(1.0),	ld1<__m128d>(2.0),	ld1<__m128d>(4.0),	ld1<__m128d>(5.0),	ld1<__m128d>(1.0));

			__m256 avx_floats	= InterpolateCubic(ld1<__m256>(1.0),	ld1<__m256>(2.0),	ld1<__m256>(4.0),	ld1<__m256>(5.0),	ld1<__m256>(1.0));
			__m256d avx_doubles = InterpolateCubic(ld1<__m256d>(1.0),	ld1<__m256d>(2.0),	ld1<__m256d>(4.0),	ld1<__m256d>(5.0),	ld1<__m256d>(1.0));

			Assert::AreEqual(f_reference, sse_floats.m128_f32[0]);
			Assert::AreEqual(d_reference, sse_doubles.m128d_f64[0]);
			Assert::AreEqual(f_reference, avx_floats.m256_f32[0]);
			Assert::AreEqual(d_reference, avx_doubles.m256d_f64[0]);
		}

	};
}