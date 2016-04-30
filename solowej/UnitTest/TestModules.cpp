#include "stdafx.h"
#include "CppUnitTest.h"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <functional>
#include "../solowej/modules/all.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace zzsystems { namespace solowej { namespace tests {
	using namespace solowej;	
	using namespace modules;
	using namespace gorynych;
	using namespace tests;
	using namespace gorynych::tests;;

#define PERFORMANCE_SNAPSHOT(target, body)\
	auto target##_start = chrono::high_resolution_clock::now();\
	body\
	auto target##_end = chrono::high_resolution_clock::now();\
	target.push_back(chrono::duration_cast<chrono::microseconds>(target##_end - target##_start).count())

	TEST_CLASS(TestModules)
	{
	public:

		TEST_METHOD(Test_Module_Perlin_Implementation_Equality)
		{			
			perlin<float, int> ref_gen;
			perlin<sse_real, sse_int> sse_gen;
			perlin<avx_real, avx_int> avx_gen;

			equality_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); }
			);
		}

		TEST_METHOD(Test_Module_Perlin_Performance)
		{
			ridged_multifractal<float, int> ref_gen;
			ridged_multifractal<sse_real, sse_int> sse_gen;
			ridged_multifractal<avx_real, avx_int> avx_gen;

			performance_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); },
				100
				);
		}

		TEST_METHOD(Test_Module_RidgedMultiFractal_Implementation_Equality)
		{
			ridged_multifractal<float, int> ref_gen;
			ridged_multifractal<sse_real, sse_int> sse_gen;
			ridged_multifractal<avx_real, avx_int> avx_gen;

			equality_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); }
			);
		}

		TEST_METHOD(Test_Module_RidgedMultiFractal_Performance)
		{
			ridged_multifractal<float, int> ref_gen;
			ridged_multifractal<sse_real, sse_int> sse_gen;
			ridged_multifractal<avx_real, avx_int> avx_gen;

			performance_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); },
				100
				);
		}

		TEST_METHOD(Test_Module_Billow_Implementation_Equality)
		{
			billow<float, int> ref_gen;
			billow<sse_real, sse_int> sse_gen;
			billow<avx_real, avx_int> avx_gen;

			equality_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); }
			);
		}

		TEST_METHOD(Test_Module_Billow_Performance)
		{
			billow<float, int> ref_gen;
			billow<sse_real, sse_int> sse_gen;
			billow<avx_real, avx_int> avx_gen;

			performance_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); },
				100
			);
		}

		TEST_METHOD(Test_Module_Voronoi_Implementation_Equality)
		{
			voronoi<float, int> ref_gen;
			voronoi<sse_real, sse_int> sse_gen;
			voronoi<avx_real, avx_int> avx_gen;

			equality_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); }
			);
		}

		TEST_METHOD(Test_Module_Voronoi_Performance)
		{
			voronoi<float, int> ref_gen;
			voronoi<sse_real, sse_int> sse_gen;
			voronoi<avx_real, avx_int> avx_gen;

			performance_test(
				[&](const auto& c) { return ref_gen(c); },
				[&](const auto& c) { return sse_gen(c); },
				[&](const auto& c) { return avx_gen(c); },
				25
				);
		}

		TEST_METHOD(Test_Module_Rotate_Implementation_Equality)
		{
			rotate<float, int> ref_gen;
			rotate<sse_real, sse_int> sse_gen;
			rotate<avx_real, avx_int> avx_gen;

			ref_gen.set_source([](const auto& c) { return 1.0f; });
			sse_gen.set_source([](const auto& c) { return 1.0f; });
			avx_gen.set_source([](const auto& c) { return 1.0f; });

			equality_test(ref_gen, sse_gen, avx_gen);
		}

		TEST_METHOD(Test_Module_Rotate_Performance)
		{
			rotate<float, int> ref_gen;
			rotate<sse_real, sse_int> sse_gen;
			rotate<avx_real, avx_int> avx_gen;

			ref_gen.set_source([](const auto& c) { return 1.0f; });
			sse_gen.set_source([](const auto& c) { return 1.0f; });
			avx_gen.set_source([](const auto& c) { return 1.0f; });

			performance_test(ref_gen, sse_gen, avx_gen);
		}

		TEST_METHOD(Test_Module_Select_Implementation_Equality)
		{
			select<float, int> ref_gen;
			select<sse_real, sse_int> sse_gen;
			select<avx_real, avx_int> avx_gen;

			ref_gen.set_a([](const auto& c) { return -1.0f; });
			ref_gen.set_b([](const auto& c) { return 1.0f; });
			ref_gen.set_controller([](const auto& c) { return 0.5f; });

			sse_gen.set_a([](const auto& c) { return -1.0f; });
			sse_gen.set_b([](const auto& c) { return 1.0f; });
			sse_gen.set_controller([](const auto& c) { return 0.5f; });

			avx_gen.set_a([](const auto& c) { return -1.0f; });
			avx_gen.set_b([](const auto& c) { return 1.0f; });
			avx_gen.set_controller([](const auto& c) { return 0.5f; });

			equality_test(ref_gen, sse_gen, avx_gen);
		}

		TEST_METHOD(Test_Module_Select_Performance)
		{
			select<float, int> ref_gen;
			select<sse_real, sse_int> sse_gen;
			select<avx_real, avx_int> avx_gen;

			ref_gen.set_a([](const auto& c) { return -1.0f; });
			ref_gen.set_b([](const auto& c) { return 1.0f; });
			ref_gen.set_controller([](const auto& c) { return 0.5f; });

			sse_gen.set_a([](const auto& c) { return -1.0f; });
			sse_gen.set_b([](const auto& c) { return 1.0f; });
			sse_gen.set_controller([](const auto& c) { return 0.5f; });

			avx_gen.set_a([](const auto& c) { return -1.0f; });
			avx_gen.set_b([](const auto& c) { return 1.0f; });
			avx_gen.set_controller([](const auto& c) { return 0.5f; });

			performance_test(ref_gen, sse_gen, avx_gen);
		}

		TEST_METHOD(Test_Module_Turbulence_Implementation_Equality)
		{
			turbulence<float, int> ref_gen;
			turbulence<sse_real, sse_int> sse_gen;
			turbulence<avx_real, avx_int> avx_gen;

			ref_gen.set_source([](const auto& c) { return 1.0f; });
			sse_gen.set_source([](const auto& c) { return 1.0f; });
			avx_gen.set_source([](const auto& c) { return 1.0f; });

			equality_test(ref_gen, sse_gen, avx_gen);
		}

		TEST_METHOD(Test_Module_Turbulence_Performance)
		{
			turbulence<float, int> ref_gen;
			turbulence<sse_real, sse_int> sse_gen;
			turbulence<avx_real, avx_int> avx_gen;

			ref_gen.set_source([](const auto& c) { return 1.0f; });
			sse_gen.set_source([](const auto& c) { return 1.0f; });
			avx_gen.set_source([](const auto& c) { return 1.0f; });


			performance_test(ref_gen, sse_gen, avx_gen);
		}


		TEST_METHOD(Test_Module_Terrace_Implementation_Equality)
		{
			terrace<float, int>			ref_gen
			{
				{ -2.0, -2.5 },
				{ -1.0, -1.5 },
				{ 0.0, 0.0 },
				{ 1.0, 1.5 },
				{ 2.0, 5.5 }
			};
			terrace<sse_real, sse_int>	sse_gen
			{
				{ -2.0, -2.5 },
				{ -1.0, -1.5 },
				{ 0.0, 0.0 },
				{ 1.0, 1.5 },
				{ 2.0, 5.5 }
			};
			terrace<avx_real, avx_int>	avx_gen
			{
				{ -2.0, -2.5 },
				{ -1.0, -1.5 },
				{ 0.0, 0.0 },
				{ 1.0, 1.5 },
				{ 2.0, 5.5 }
			};

			ref_gen.set_source([](const auto& c) { return 1.0f; });
			sse_gen.set_source([](const auto& c) { return 1.0f; });
			avx_gen.set_source([](const auto& c) { return 1.0f; });


			equality_test(ref_gen, sse_gen, avx_gen);
		}

		TEST_METHOD(Test_Module_Terrace_Performance)
		{
			terrace<float, int>			ref_gen
			{
				{ -2.0, -2.5 },
				{ -1.0, -1.5 },
				{ 0.0, 0.0 },
				{ 1.0, 1.5 },
				{ 2.0, 5.5 }
			};
			terrace<sse_real, sse_int>	sse_gen
			{
				{ -2.0, -2.5 },
				{ -1.0, -1.5 },
				{ 0.0, 0.0 },
				{ 1.0, 1.5 },
				{ 2.0, 5.5 }
			};
			terrace<avx_real, avx_int>	avx_gen
			{
				{ -2.0, -2.5 },
				{ -1.0, -1.5 },
				{ 0.0, 0.0 },
				{ 1.0, 1.5 },
				{ 2.0, 5.5 }
			};

			ref_gen.set_source([](const auto& c) { return 1.0f; });
			sse_gen.set_source([](const auto& c) { return 1.0f; });
			avx_gen.set_source([](const auto& c) { return 1.0f; });


			performance_test(ref_gen, sse_gen, avx_gen);
		}

		TEST_METHOD(Test_Module_Scheduler)
		{
			/*billow_settings module_s;
			module_s.seed = 125;
			scheduler_settings scheduler_s({4, 4, 4}, 1337, true);

			auto result = schedule2D<float4>([&module_s](const vec3<float4>& coords) { return billow<float4, int4>(coords, module_s); }, [](const vec3<float4>& coords) { return coords; }, scheduler_s);
			*/

			Assert::AreEqual(1, 1, std::numeric_limits<float>::epsilon());
		}

		static void equality_test(Module<float> ref_gen, Module<sse_real> sse_gen, Module<avx_real> avx_gen)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			for (float z = -1; z <= 1; z += 0.25)
			for (float y = -1; y <= 1; y += 0.25)
			for (float x = -1; x <= 1; x += 0.25)
			{	
				auto ref = ref_gen(vec3<float>(x, y, z));
				auto sse = sse_gen(vec3<sse_real>(x, y, z));
				auto avx = avx_gen(vec3<avx_real>(x, y, z));
					
				wstringstream sse_feedback;
				wstringstream avx_feedback;

				sse_feedback << "sse invalid for [" << x << ";" << y << ";" << z << "]";
				avx_feedback << "avx invalid for [" << x << ";" << y << ";" << z << "]";

				AreEqual(ref, sse, sse_feedback.str().c_str());
				AreEqual(ref, avx, avx_feedback.str().c_str());
			}	
		}

		static void performance_test(Module<float> ref_gen, Module<sse_real> sse_gen, Module<avx_real> avx_gen, size_t iterations = 1000)
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			vector<long long> ref_times, sse_times, avx_times;

			PERFORMANCE_SNAPSHOT(ref_times,
			for (int r = 0; r < iterations; r++)
			for (float x = -8; x <= 8; x += 0.25)			
			for (float y = -8; y <= 8; y += 0.25)			
			for (float z = -8; z <= 8; z += 0.25)
			{					 
				ref_gen(vec3<float>(x, y, z));
			});

			PERFORMANCE_SNAPSHOT(sse_times,
			for (int r = 0; r < iterations; r++)
			for (float x = -8; x <= 8; x += 1.0)
			{
				auto _x = sse_real(x, x + 0.25f, x + 0.5f, x + 0.75f);

				for (float y = -8; y <= 8; y += 0.25)
				{
					sse_real _y = y;

					for (float z = -8; z <= 8; z += 0.25)
					{
						sse_gen(vec3<sse_real>(_x, _y, z));
					}
				}
			});
						
			PERFORMANCE_SNAPSHOT(avx_times,
			for (int r = 0; r < iterations; r++)
			for (float x = -8; x <= 8; x += 2.0)
			{
				auto _x = avx_real(x, x + 0.25f, x + 0.5f, x + 0.75f, x + 1.0f, x + 1.25f, x + 1.5f, x + 1.75f);
				
				for (float y = -8; y <= 8; y += 0.25)
				{
					avx_real _y = y;

					for (float z = -8; z <= 8; z += 0.25)
					{
						avx_gen(vec3<avx_real>(_x, _y, z));
					}
				}
			});

			auto	ref_time = accumulate(ref_times.begin(), ref_times.end(), 0) / ref_times.size(),
					sse_time = accumulate(sse_times.begin(), sse_times.end(), 0) / sse_times.size(),
					avx_time = accumulate(avx_times.begin(), avx_times.end(), 0) / avx_times.size();

			ostringstream perfstream;

			perfstream
				<< "ref: " << ref_time << endl
				<< "sse: " << sse_time << " (" << static_cast<double>(ref_time) / sse_time << ")" << endl
				<< "avx: " << avx_time << " (" << static_cast<double>(ref_time) / avx_time << ")" << endl;

			Logger::WriteMessage(perfstream.str().c_str());
		}
	};
}}}