// Examples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "run.h"
#include "wood.h"
#include "granite.h"
#include "planet.h"
#include "run.h"

#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"

namespace paranoise { namespace examples {
	using namespace parallel;
	using namespace scheduler;

	void run()
	{
		system_info c;
		cout << c;

		//_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

#ifdef PROFILE
		const int w = 4096;//4096; //8192 * 2;
		const int h = 4096;//4096; //8192 * 2;
#else 
		const int w = 4096; //8192 * 2;
		const int h = 4096; //8192 * 2;
#endif
		using capability = integral_constant<int, CAPABILITY_SSE2 | CAPABILITY_FMA3 | CAPABILITY_SSE41>;

		using vfloat = float4<capability>;
		using vint = int4<capability>;

		using fvec = Vector3<vfloat>;
		using vec = Vector3<float>;


		scheduler_settings settings({ w, h, 1 }, 1337, true);

		auto f1 = [=]()
		{
			return schedule2D<vfloat>(
				generate_complex_planet<vfloat, vint>(),
				[w, h](const fvec &coords) { return (coords)* fvec(10, 10, 10) / fvec(w, h, h) - fvec(5, 5, 5); },
				settings);
		};

		auto f2 = [=]()
		{
			return schedule2D<float>(
				generate_complex_planet<float, int>(),
				[w, h](const vec &coords) { return (coords)* vec(10, 10, 10) / vec(w, h, h) - vec(5, 5, 5);},
				settings);
		};

		run(w, h, f1, f2, granite_grad);
	}
}}

int main(int argc, char** argv)
{
	paranoise::examples::run();

	return 0;
}
