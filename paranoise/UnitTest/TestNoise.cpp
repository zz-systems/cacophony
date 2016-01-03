#include "stdafx.h"
#include "CppUnitTest.h"

#include "../paranoise/basetypes.h"
#include "../paranoise/noisegenerators.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace paranoise;
using namespace paranoise::generators;
using namespace paranoise::parallel;

namespace UnitTest
{
	TEST_CLASS(TestNoise)
	{
	public:

		TEST_METHOD(Test_GradientCoherentNoise3D)
		{
			for (auto q = 0; q <= 2; q++)
			{
				auto d_reference	= GradientCoherentNoise3D<double>(Vector3<double>(5.0, 2.5, 4.0), 1010, Quality(q));
				auto f_reference	= (float)d_reference;

				//m128	sse_floats = InterpolateCubic(ld1<m128>(1.0), ld1<m128>(2.0), ld1<m128>(4.0), ld1<m128>(5.0), ld1<m128>(1.0));
				m128d sse_doubles = GradientCoherentNoise3D(Vector3<m128d>(5.0, 2.5, 4.0), m128i(1010), Quality(q));

				//m256 avx_floats = InterpolateCubic(ld1<m256>(1.0), ld1<m256>(2.0), ld1<m256>(4.0), ld1<m256>(5.0), ld1<m256>(1.0));
				//m256d avx_doubles = GradientCoherentNoise3D(ld1<m256d>(1.0), ld1<m256d>(2.0), ld1<m256d>(4.0), ld1<m256i>(1000), Quality::Fast);	

				Assert::AreEqual(d_reference, sse_doubles.values[0], std::numeric_limits<double>::epsilon());
				Assert::AreEqual(d_reference, sse_doubles.values[1], std::numeric_limits<double>::epsilon());
				//Assert::AreEqual(d_reference, avx_doubles.doubles[0]);
			}
		}

		TEST_METHOD(Test_GradientNoise3D)
		{
			auto d_reference = GradientNoise3D<double, int32>(Vector3<double>(5.0, 2.5, 4.0), Vector3<int32>(1, 2, 4), 1010);
			auto f_reference = (float)d_reference;

			//m128	sse_floats = InterpolateCubic(ld1<m128>(1.0), ld1<m128>(2.0), ld1<m128>(4.0), ld1<m128>(5.0), ld1<m128>(1.0));
			m128d sse_doubles = GradientNoise3D(Vector3<m128d>(5.0, 2.5, 4.0), Vector3<m128i>(1, 2, 4), m128i(1010));

			//m256 avx_floats = InterpolateCubic(ld1<m256>(1.0), ld1<m256>(2.0), ld1<m256>(4.0), ld1<m256>(5.0), ld1<m256>(1.0));
			//m256d avx_doubles = GradientCoherentNoise3D(ld1<m256d>(1.0), ld1<m256d>(2.0), ld1<m256d>(4.0), ld1<m256i>(1000), Quality::Fast);
			
			
			Assert::AreEqual(d_reference, sse_doubles.values[0], std::numeric_limits<double>::epsilon());
			Assert::AreEqual(d_reference, sse_doubles.values[1], std::numeric_limits<double>::epsilon());
			//Assert::AreEqual(d_reference, avx_doubles.doubles[0]);
		}

		TEST_METHOD(Test_ValueCoherentNoise3D)
		{
			for (auto q = 0; q <= 2; q++)
			{
				auto d_reference = ValueCoherentNoise3D<double>(Vector3<double>(5.0, 2.5, 4.0), 1010, Quality(q));
				auto f_reference = (float)d_reference;

				//m128	sse_floats = InterpolateCubic(ld1<m128>(1.0), ld1<m128>(2.0), ld1<m128>(4.0), ld1<m128>(5.0), ld1<m128>(1.0));
				m128d sse_doubles = ValueCoherentNoise3D(Vector3<m128d>(5.0, 2.5, 4.0), m128i(1010), Quality(q));

				//m256 avx_floats = InterpolateCubic(ld1<m256>(1.0), ld1<m256>(2.0), ld1<m256>(4.0), ld1<m256>(5.0), ld1<m256>(1.0));
				//m256d avx_doubles = GradientCoherentNoise3D(ld1<m256d>(1.0), ld1<m256d>(2.0), ld1<m256d>(4.0), ld1<m256i>(1000), Quality::Fast);	

				Assert::AreEqual(d_reference, sse_doubles.values[0], std::numeric_limits<double>::epsilon());
				Assert::AreEqual(d_reference, sse_doubles.values[1], std::numeric_limits<double>::epsilon());
			}
			//Assert::AreEqual(d_reference, avx_doubles.doubles[0]);
		}

		TEST_METHOD(Test_ValueNoise3D)
		{			
			auto d_reference = ValueNoise3D<double, int32>(Vector3<int32>(1, 2, 4), 1010);
			auto f_reference = (float)d_reference;

			//m128	sse_floats = InterpolateCubic(ld1<m128>(1.0), ld1<m128>(2.0), ld1<m128>(4.0), ld1<m128>(5.0), ld1<m128>(1.0));
			m128d sse_doubles = ValueNoise3D<m128d>(Vector3<m128i>(1, 2, 4), m128i(1010));

			//m256 avx_floats = InterpolateCubic(ld1<m256>(1.0), ld1<m256>(2.0), ld1<m256>(4.0), ld1<m256>(5.0), ld1<m256>(1.0));
			//m256d avx_doubles = GradientCoherentNoise3D(ld1<m256d>(1.0), ld1<m256d>(2.0), ld1<m256d>(4.0), ld1<m256i>(1000), Quality::Fast);


			Assert::AreEqual(d_reference, sse_doubles.values[0], std::numeric_limits<double>::epsilon());
			Assert::AreEqual(d_reference, sse_doubles.values[1], std::numeric_limits<double>::epsilon());
			//Assert::AreEqual(d_reference, avx_doubles.doubles[0]);
		}

		TEST_METHOD(Test_CubeBounds_Single)
		{			
			int		one = 1;
			double	zero = 0;
			double	x = 2, y = 0, z = 1;

			auto x0 = int(x) - (int(x > zero) & one);
			auto y0 = int(y) - (int(y > zero) & one);
			auto z0 = int(z) - (int(z > zero) & one);

			auto x1 = x0 + one;
			auto y1 = y0 + one;
			auto z1 = z0 + one;

			Assert::AreEqual(x0, 2 - 1);
			Assert::AreEqual(y0, 0 - 0);
			Assert::AreEqual(z0, 1 - 1);

			Assert::AreEqual(x1, 2 - 1 + 1);
			Assert::AreEqual(y1, 0 - 0 + 1);
			Assert::AreEqual(z1, 1 - 1 + 1);
		}

		TEST_METHOD(Test_CubeBounds_SIMD)
		{
			m128d	zero = 0;
			m128d	x = 2, y = 0, z = 1;

			auto x0 = m128i(x) - (m128i((x > zero) & 1));
			auto y0 = m128i(y) - (m128i((y > zero) & 1));
			auto z0 = m128i(z) - (m128i((z > zero) & 1));

			auto x1 = x0 + 1;
			auto y1 = y0 + 1;
			auto z1 = z0 + 1;

			Assert::AreEqual(x0.i32[0], 2 - 1);
			Assert::AreEqual(y0.i32[0], 0 - 0);
			Assert::AreEqual(z0.i32[0], 1 - 1);

			Assert::AreEqual(x1.i32[0], 2 - 1 + 1);
			Assert::AreEqual(y1.i32[0], 0 - 0 + 1);
			Assert::AreEqual(z1.i32[0], 1 - 1 + 1);
		}
	};
}