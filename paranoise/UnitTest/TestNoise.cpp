#include "stdafx.h"
#include "CppUnitTest.h"

#include "../paranoise/parallel/all.h"
#include "../paranoise/basetypes.h"
#include "../paranoise/noisegenerators.h"
#include <iostream>

#include "util.h"

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
			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)			
			{
				auto ref	= GradientCoherentNoise3D(Vector3<float>(x, y, z), 1010, Quality(q));

				AreEqual(ref, GradientCoherentNoise3D<float4, int4>(Vector3<float4>(x, y, z), 1010, Quality(q)));
			}
		}

		TEST_METHOD(Test_GradientNoise3D)
		{	
			for (int zi = -2; zi <= 2; zi += 1)
			for (int yi = -2; yi <= 2; yi += 1)
			for (int xi = -2; xi <= 2; xi += 1)
			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = GradientNoise3D<float, int>({ x, y, z }, { xi, zi, yi }, 1010);

				AreEqual(ref, GradientNoise3D<float4, int4>({ x, y, z }, { xi, zi, yi }, 1010));
			}
		}

		TEST_METHOD(Test_ValueCoherentNoise3D)
		{
			for (auto q = 0; q <= 2; q++)
			{
				auto reference = ValueCoherentNoise3D<float>(Vector3<float>(5.0, 2.5, 4.0), 1010, Quality(q));
				

				//m128	sse_floats = cerp(ld1<m128>(1.0), ld1<m128>(2.0), ld1<m128>(4.0), ld1<m128>(5.0), ld1<m128>(1.0));
				float4 sse_floats = ValueCoherentNoise3D(Vector3<float4>(5.0, 2.5, 4.0), int4(1010), Quality(q));

				//m256 avx_floats = cerp(ld1<m256>(1.0), ld1<m256>(2.0), ld1<m256>(4.0), ld1<m256>(5.0), ld1<m256>(1.0));
				//float4 avx_floats = GradientCoherentNoise3D(ld1<float4>(1.0), ld1<float4>(2.0), ld1<float4>(4.0), ld1<int8>(1000), Quality::Fast);	

				Assert::AreEqual(reference, sse_floats.values[0], std::numeric_limits<float>::epsilon());
				Assert::AreEqual(reference, sse_floats.values[1], std::numeric_limits<float>::epsilon());
			}
			//Assert::AreEqual(reference, avx_floats.floats[0]);
		}

		TEST_METHOD(Test_ValueNoise3D)
		{			
			auto reference = ValueNoise3D<float, int32>(Vector3<int32>(1, 2, 4), 1010);
			

			//m128	sse_floats = cerp(ld1<m128>(1.0), ld1<m128>(2.0), ld1<m128>(4.0), ld1<m128>(5.0), ld1<m128>(1.0));
			float4 sse_floats = ValueNoise3D<float4>(Vector3<int4>(1, 2, 4), int4(1010));

			//m256 avx_floats = cerp(ld1<m256>(1.0), ld1<m256>(2.0), ld1<m256>(4.0), ld1<m256>(5.0), ld1<m256>(1.0));
			//float4 avx_floats = GradientCoherentNoise3D(ld1<float4>(1.0), ld1<float4>(2.0), ld1<float4>(4.0), ld1<int8>(1000), Quality::Fast);


			Assert::AreEqual(reference, sse_floats.values[0], std::numeric_limits<float>::epsilon());
			Assert::AreEqual(reference, sse_floats.values[1], std::numeric_limits<float>::epsilon());
			//Assert::AreEqual(reference, avx_floats.floats[0]);
		}

		TEST_METHOD(Test_CubeBounds_Single)
		{	
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			for (float z = -1; z <= 1; z += 0.25f)
			for (float y = -1; y <= 1; y += 0.25f)
			for (float x = -1; x <= 1; x += 0.25f)
			{
				Matrix3x2<int> ref;
				detail::construct_cube<float, int>({ x, y, z }, ref);

				Matrix3x2<int4> sse;
				detail::construct_cube<float4, int4>({ x, y, z }, sse);

				std::cerr << " ";
				for (int dy = 0; dy < 2; dy++)
				{
					for (int dx = 0; dx < 3; dx++)
					{
						AreEqual_NOE(ref.v[dy].v[dx], sse.v[dy].v[dx]);
					}
				}
			}

			/*int		one = 1;
			float	zero = 0;
			float	x = 2, y = 0, z = 1;

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
			Assert::AreEqual(z1, 1 - 1 + 1);*/
		}

		TEST_METHOD(Test_CubeBounds_CAST_SIMD)
		{
			float4	zero = 0;
			float4	x = 2, y = 0, z = 1;

			auto x0 = (int4)(x - ((x > zero) & 1));
			auto y0 = (int4)(y - ((y > zero) & 1));
			auto z0 = (int4)(z - ((z > zero) & 1));

			auto x1 = x0 + 1;
			auto y1 = y0 + 1;
			auto z1 = z0 + 1;

			Assert::AreEqual(x0.values[0], 2 - 1);
			Assert::AreEqual(y0.values[0], 0 - 0);
			Assert::AreEqual(z0.values[0], 1 - 1);

			Assert::AreEqual(x1.values[0], 2 - 1 + 1);
			Assert::AreEqual(y1.values[0], 0 - 0 + 1);
			Assert::AreEqual(z1.values[0], 1 - 1 + 1);
		}

		TEST_METHOD(Test_CubeBounds_SIMD)
		{
			int4	zero = 0;
			int4	x = 2, y = 0, z = 1;

			auto x0 = x - ((x > zero) & 1);
			auto y0 = y - ((y > zero) & 1);
			auto z0 = z - ((z > zero) & 1);

			auto x1 = x0 + 1;
			auto y1 = y0 + 1;
			auto z1 = z0 + 1;

			Assert::AreEqual(x0.values[0], 2 - 1);
			Assert::AreEqual(y0.values[0], 0 - 0);
			Assert::AreEqual(z0.values[0], 1 - 1);

			Assert::AreEqual(x1.values[0], 2 - 1 + 1);
			Assert::AreEqual(y1.values[0], 0 - 0 + 1);
			Assert::AreEqual(z1.values[0], 1 - 1 + 1);
		}
	};
}