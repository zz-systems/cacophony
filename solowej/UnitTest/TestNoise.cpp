#include "stdafx.h"
#include "CppUnitTest.h"

#include "../solowej/vector.h"
#include "../solowej/noisegenerators.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace zzsystems { namespace solowej { namespace tests {
	using namespace solowej;
	using namespace math;
	using namespace gorynych;
	using namespace gorynych::tests;
	
	using namespace math;

	class TestNoise;

	SIMD_ENABLED class noisegen_wrapper : noisegen<SIMD_T>
	{
		friend class TestNoise;
	};


	TEST_CLASS(TestNoise)
	{
	public:

		TEST_METHOD(Test_Noise_GradientCoherentNoise3D)
		{
			for (auto q = 0; q <= 2; q++)
			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)			
			{
				auto ref = noisegen_wrapper<float, int>::gradient_coherent_3d({ x, y, z }, 1010, Quality(q));

				AreEqual(ref, noisegen_wrapper<sse_real, sse_int>::gradient_coherent_3d({ x, y, z }, 1010, Quality(q)));
				AreEqual(ref, noisegen_wrapper<avx_real, avx_int>::gradient_coherent_3d({ x, y, z }, 1010, Quality(q)));
			}
		}

		TEST_METHOD(Test_Noise_GradientNoise3D)
		{	
			for (int zi = -2; zi <= 2; zi += 1)
			for (int yi = -2; yi <= 2; yi += 1)
			for (int xi = -2; xi <= 2; xi += 1)
			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = noisegen_wrapper<float, int>::gradient_3d({ x, y, z }, { xi, zi, yi }, 1010);

				AreEqual(ref, noisegen_wrapper<sse_real, sse_int>::gradient_3d({ x, y, z }, { xi, zi, yi }, 1010));
				AreEqual(ref, noisegen_wrapper<avx_real, avx_int>::gradient_3d({ x, y, z }, { xi, zi, yi }, 1010));
			}
		}

		TEST_METHOD(Test_Noise_ValueCoherentNoise3D)
		{
			for (auto q = 0; q <= 2; q++)
			{
				auto ref = noisegen_wrapper<float, int>::value_coherent_3d({ 5.0f, 2.5f, 4.0f }, 1010, Quality(q));

				AreEqual(ref, noisegen_wrapper<sse_real, sse_int>::value_coherent_3d({ 5.0f, 2.5f, 4.0f }, 1010, Quality(q)));
				AreEqual(ref, noisegen_wrapper<avx_real, avx_int>::value_coherent_3d({ 5.0f, 2.5f, 4.0f }, 1010, Quality(q)));
			}
		}

		TEST_METHOD(Test_Noise_ValueNoise3D)
		{			
			auto ref = noisegen_wrapper<float, int>::realvalue_3d({ 1, 2, 4 }, 1010);

			AreEqual(ref, noisegen_wrapper<sse_real, sse_int>::realvalue_3d({ 1, 2, 4 }, 1010));
			AreEqual(ref, noisegen_wrapper<avx_real, avx_int>::realvalue_3d({ 1, 2, 4 }, 1010));
		}

		TEST_METHOD(Test_Noise_CubeBounds_Single)
		{	
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			for (float z = -1; z <= 1; z += 0.25f)
			for (float y = -1; y <= 1; y += 0.25f)
			for (float x = -1; x <= 1; x += 0.25f)
			{
				auto ref = noisegen_wrapper<float, int>::construct_cube({ x, y, z });

				auto sse = noisegen_wrapper<sse_real, sse_int>::construct_cube({ x, y, z });

				std::cerr << " ";
				for (int dy = 0; dy < 2; dy++)
				{
					for (int dx = 0; dx < 3; dx++)
					{
						AreEqual_NOE(ref.v[dy].v[dx], sse.v[dy].v[dx]);
					}
				}
			}
		}

		TEST_METHOD(Test_Noise_CubeBounds_CAST_SIMD)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			sse_real	zero = 0;
			sse_real	x = 2, y = 0, z = 1;

			auto x0 = static_cast<sse_int>(x - ((x > zero) & 1.0f));
			auto y0 = static_cast<sse_int>(y - ((y > zero) & 1.0f));
			auto z0 = static_cast<sse_int>(z - ((z > zero) & 1.0f));

			auto x1 = x0 + 1;
			auto y1 = y0 + 1;
			auto z1 = z0 + 1;

			AreEqual(2 - 1, x0, L"x0 invalid");
			AreEqual(0 - 0, y0, L"y0 invalid");
			AreEqual(1 - 1, z0, L"z0 invalid");

			AreEqual(2 - 1 + 1, x1, L"x1 invalid");
			AreEqual(0 - 0 + 1, y1, L"y1 invalid");
			AreEqual(1 - 1 + 1, z1, L"z1 invalid");
		}

		TEST_METHOD(Test_Noise_CubeBounds_SIMD)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			sse_int	zero = 0;
			sse_int	x = 2, y = 0, z = 1;

			auto x0 = x - ((x > zero) & 1);
			auto y0 = y - ((y > zero) & 1);
			auto z0 = z - ((z > zero) & 1);

			auto x1 = x0 + 1;
			auto y1 = y0 + 1;
			auto z1 = z0 + 1;

			AreEqual(2 - 1, x0, L"x0 invalid");
			AreEqual(0 - 0, y0, L"y0 invalid");
			AreEqual(1 - 1, z0, L"z0 invalid");

			AreEqual(2 - 1 + 1, x1, L"x1 invalid");
			AreEqual(0 - 0 + 1, y1, L"y1 invalid");
			AreEqual(1 - 1 + 1, z1, L"z1 invalid");
		}
	};
}}}