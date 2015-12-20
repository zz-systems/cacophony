#include "stdafx.h"
#include "CppUnitTest.h"

#include "../libnoise++/modules/perlin.h"
#include "../libnoise++/modules/ridgedmultifrac.h"
#include "../libnoise++/modules/billow.h"

#define TEST_USE_THREADS
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace noisepp::interpolation;
using namespace noisepp::intrinsic;
using namespace noisepp::module;
using namespace noisepp;

#ifdef TEST_USE_THREADS
#include <ppl.h>
using namespace concurrency;
#endif

namespace UnitTest
{
	TEST_CLASS(TestModules)
	{
	public:

		TEST_METHOD(Test_Module_Perlin)
		{
			auto settings = perlin_settings();
			auto refresult = perlin<double, int>(Vector3<double>(1, 1, 1), settings);

			auto result = perlin<m128d, m128i>(Vector3<m128d>(1, 1, 1), settings);

			Assert::AreEqual(refresult, result.values[0], std::numeric_limits<double>::epsilon());
		}

		TEST_METHOD(Test_Module_RidgedMultiFractal)
		{
			auto settings = ridged_settings();
			auto refresult = ridged<double, int>(Vector3<double>(1, 1, 1), settings);

			//auto settings		= perlin_settings<m128d, m128i>();
			auto result = ridged<m128d, m128i>(Vector3<m128d>(1, 1, 1), settings);

			Assert::AreEqual(refresult, result.values[0], std::numeric_limits<double>::epsilon());
		}

		TEST_METHOD(Test_Module_Billow)
		{
			auto settings = billow_settings();
			auto refresult = billow<double, int>(Vector3<double>(1, 1, 1), settings);

			//auto settings		= perlin_settings<m128d, m128i>();
			auto result = billow<m128d, m128i>(Vector3<m128d>(1, 1, 1), settings);

			Assert::AreEqual(refresult, result.values[0], std::numeric_limits<double>::epsilon());
		}
	};
}