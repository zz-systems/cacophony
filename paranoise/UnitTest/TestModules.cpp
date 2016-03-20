#include "stdafx.h"
//#include "CppUnitTest.h"
//
//#include "../paranoise/parallel/all.h"
//#include "../paranoise/modules/all.h"
//#include "../paranoise/scheduler.h"
//#include "util.h"
//
//using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//using namespace paranoise::interpolation;
//using namespace paranoise::parallel;
//using namespace paranoise::module;
//using namespace paranoise::scheduler;
//using namespace paranoise;
//
//namespace UnitTest
//{
//	TEST_CLASS(TestModules)
//	{
//	public:
//
//		TEST_METHOD(Test_Module_Perlin)
//		{
//			for (float z = -1; z <= 1; z += 0.25)
//			for (float y = -1; y <= 1; y += 0.25)
//			for (float x = -1; x <= 1; x += 0.25)
//			{
//				auto ref = perlin<float, int>(Vector3<float>(x, y, z), perlin_settings<float, int>());
//
//				AreEqual(ref, perlin<float4, int4>(Vector3<float4>(x, y, z), perlin_settings<float4, int4>()));
//				//AreEqual(ref, perlin<float8, int8>(Vector3<float8>(1, 1, 1), settings));
//			}
//		}
//
//		TEST_METHOD(Test_Module_RidgedMultiFractal)
//		{
//			auto settings = ridged_settings();
//			auto refresult = ridged<float, int>(Vector3<float>(1, 1, 1), settings);
//
//			//auto settings		= perlin_settings<float4, int4>();
//			auto result = ridged<float4, int4>(Vector3<float4>(1, 1, 1), settings);
//
//			Assert::AreEqual(refresult, result.values[0], std::numeric_limits<float>::epsilon());
//		}
//
//		TEST_METHOD(Test_Module_Billow)
//		{
//			auto refresult = billow<float, int>(Vector3<float>(1, 1, 1), billow_settings<float, int>());
//
//			//auto settings		= perlin_settings<float4, int4>();
//			auto result = billow<float4, int4>(Vector3<float4>(1, 1, 1), billow_settings<float4, int4>());
//
//			Assert::AreEqual(refresult, result.values[0], std::numeric_limits<float>::epsilon());
//		}
//
//		TEST_METHOD(Test_Module_Scheduler)
//		{
//			/*billow_settings module_s;
//			module_s.seed = 125;
//			scheduler_settings scheduler_s({4, 4, 4}, 1337, true);
//
//			auto result = schedule2D<float4>([&module_s](const Vector3<float4>& coords) { return billow<float4, int4>(coords, module_s); }, [](const Vector3<float4>& coords) { return coords; }, scheduler_s);
//			*/
//
//			Assert::AreEqual(1, 1, std::numeric_limits<float>::epsilon());
//		}
//	};
//}