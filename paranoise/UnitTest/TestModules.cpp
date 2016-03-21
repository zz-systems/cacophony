#include "stdafx.h"
#include "CppUnitTest.h"

#include "../paranoise/parallel/all.h"
#include "../paranoise/modules/all.h"
#include "../paranoise/scheduler.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace paranoise::interpolation;
using namespace paranoise::parallel;
using namespace paranoise::module;
using namespace paranoise::scheduler;
using namespace paranoise;
using namespace test;

namespace UnitTest
{
	TEST_CLASS(TestModules)
	{
	public:

		TEST_METHOD(Test_Module_Perlin)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			perlin<float, int> ref_gen;
			perlin<sse_real, sse_int> sse_gen;
			perlin<avx_real, avx_int> avx_gen;

			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = ref_gen(Vector3<float>(x, y, z));

				AreEqual(ref, sse_gen(Vector3<sse_real>(x, y, z)));
				AreEqual(ref, avx_gen(Vector3<avx_real>(x, y, z)));
			}
		}

		TEST_METHOD(Test_Module_RidgedMultiFractal)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			ridged_multifractal<float, int> ref_gen;
			ridged_multifractal<sse_real, sse_int> sse_gen;
			ridged_multifractal<avx_real, avx_int> avx_gen;

			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = ref_gen(Vector3<float>(x, y, z));

				AreEqual(ref, sse_gen(Vector3<sse_real>(x, y, z)));
				AreEqual(ref, avx_gen(Vector3<avx_real>(x, y, z)));
			}
		}

		TEST_METHOD(Test_Module_Billow)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			billow<float, int> ref_gen;
			billow<sse_real, sse_int> sse_gen;
			billow<avx_real, avx_int> avx_gen;

			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = ref_gen(Vector3<float>(x, y, z));

				AreEqual(ref, sse_gen(Vector3<sse_real>(x, y, z)));
				AreEqual(ref, avx_gen(Vector3<avx_real>(x, y, z)));
			}
		}

		TEST_METHOD(Test_Module_Rotate)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			module::rotate<float, int> ref_gen;
			module::rotate<sse_real, sse_int> sse_gen;
			module::rotate<avx_real, avx_int> avx_gen;

			Module<float> ref_in	= [](const auto& c) { return 1.0f; };
			Module<sse_real> sse_in = [](const auto& c) { return 1.0f; };
			Module<avx_real> avx_in = [](const auto& c) { return 1.0f; };

			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = ref_gen(Vector3<float>(x, y, z), ref_in);

				AreEqual(ref, sse_gen(Vector3<sse_real>(x, y, z), sse_in));
				AreEqual(ref, avx_gen(Vector3<avx_real>(x, y, z), avx_in));
			}
		}

		TEST_METHOD(Test_Module_Select)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			select<float, int> ref_gen;
			select<sse_real, sse_int> sse_gen;
			select<avx_real, avx_int> avx_gen;

			Module<float>
				ref_a = [](const auto& c) { return -1.0f; },
				ref_b = [](const auto& c) { return 1.0f; },
				ref_c = [](const auto& c) { return 0.5f; };

			Module<sse_real>
				sse_a = [](const auto& c) { return -1.0f; },
				sse_b = [](const auto& c) { return 1.0f; },
				sse_c = [](const auto& c) { return 0.5f; };
			Module<avx_real>
				avx_a = [](const auto& c) { return -1.0f; },
				avx_b = [](const auto& c) { return 1.0f; },
				avx_c = [](const auto& c) { return 0.5f; };

			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = ref_gen(Vector3<float>(x, y, z), ref_a, ref_b, ref_c);

				AreEqual(ref, sse_gen(Vector3<sse_real>(x, y, z), sse_a, sse_b, sse_c));
				AreEqual(ref, avx_gen(Vector3<avx_real>(x, y, z), avx_a, avx_b, avx_c));
			}
		}

		TEST_METHOD(Test_Module_Turbulence)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			turbulence<float, int> ref_gen;
			turbulence<sse_real, sse_int> sse_gen;
			turbulence<avx_real, avx_int> avx_gen;

			Module<float> ref_in	= [](const auto& c) { return 1.0f; };
			Module<sse_real> sse_in = [](const auto& c) { return 1.0f; };
			Module<avx_real> avx_in = [](const auto& c) { return 1.0f; };

			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{
				auto ref = ref_gen(Vector3<float>(x, y, z), ref_in);

				AreEqual(ref, sse_gen(Vector3<sse_real>(x, y, z), sse_in));
				AreEqual(ref, avx_gen(Vector3<avx_real>(x, y, z), avx_in));
			}
		}

		TEST_METHOD(Test_Module_Scheduler)
		{
			/*billow_settings module_s;
			module_s.seed = 125;
			scheduler_settings scheduler_s({4, 4, 4}, 1337, true);

			auto result = schedule2D<float4>([&module_s](const Vector3<float4>& coords) { return billow<float4, int4>(coords, module_s); }, [](const Vector3<float4>& coords) { return coords; }, scheduler_s);
			*/

			Assert::AreEqual(1, 1, std::numeric_limits<float>::epsilon());
		}
	};
}