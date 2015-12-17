#include "stdafx.h"
#include "CppUnitTest.h"
#include "../libnoise++/noisegenerators.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace noisepp::generators;
using namespace noisepp::intrinsic;

namespace UnitTest
{
	TEST_CLASS(TestNoise)
	{
	public:

		TEST_METHOD(Test_GradientCoherentNoise3D)
		{
			auto d_reference = GradientCoherentNoise3D<double>(1.0, 2.0, 4.0, 1000, Quality::Fast);
			auto f_reference = (float)d_reference;

			//__m128	sse_floats = InterpolateCubic(ld1<__m128>(1.0), ld1<__m128>(2.0), ld1<__m128>(4.0), ld1<__m128>(5.0), ld1<__m128>(1.0));
			__m128d sse_doubles = GradientCoherentNoise3D(ld1<__m128d>(1.0), ld1<__m128d>(2.0), ld1<__m128d>(4.0), ld1<__m128i>(1000), Quality::Fast);

			//__m256 avx_floats = InterpolateCubic(ld1<__m256>(1.0), ld1<__m256>(2.0), ld1<__m256>(4.0), ld1<__m256>(5.0), ld1<__m256>(1.0));
			__m256d avx_doubles = GradientCoherentNoise3D(ld1<__m256d>(1.0), ld1<__m256d>(2.0), ld1<__m256d>(4.0), ld1<__m256i>(1000), Quality::Fast);
						
			Assert::AreEqual(d_reference, sse_doubles.m128d_f64[0]);			
			Assert::AreEqual(d_reference, avx_doubles.m256d_f64[0]);
		}

	};
}