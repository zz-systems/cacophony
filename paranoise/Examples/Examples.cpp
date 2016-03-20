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
		system_info sysinfo;
		cout << sysinfo;
		//sysinfo.setFlag(CAPABILITY_AVX1, false);
		//_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

#ifdef PROFILE
		const int w = 4 * 4096;//4096; //8192 * 2;
		const int h = 4 * 4096;//4096; //8192 * 2;
#else 
		const int w = 1024 * 2; //8192 * 2;
		const int h = 1024 * 2; //8192 * 2;
#endif				
		scheduler_settings settings({ w, h, 1 }, 1337, true);

		auto f1 = [=]()
		{
			SIMD_DISPATCH(sysinfo,
			{ 
				//cout << ">>dispatch: using " << static_dispatcher<capability>::unit_name << " branch" << endl; 
				return schedule2D<vreal>(generate_complex_planet<vreal, vint>(),
					[w, h](const Vector3<vreal> &coords) { return (coords)*  Vector3<vreal>(10, 10, 2) / Vector3<vreal>(w, h, h) - Vector3<vreal>(5, 5, 1); },
					settings); 
			});
		};

		auto f2 = [=]()
		{
			system_info reference;
			reference.feature_flags = CAPABILITY_NONE;
			//cout << reference;
			//cout << ">>ref dispatch: using " << static_dispatcher<integral_constant<int, CAPABILITY_SSE2>>::unit_name << " branch" << endl;

			SIMD_DISPATCH(reference, 
			{
				//cout << ">>ref dispatch: using " << static_dispatcher<capability>::unit_name << " branch" << endl;

				return schedule2D<vreal>(
					generate_complex_planet<vreal, vint>(),
					[w, h](const Vector3<vreal> &coords) { return (coords)* Vector3<vreal>(10, 10, 2) / Vector3<vreal>(w, h, h) - Vector3<vreal>(5, 5, 1);},
					settings);
			});
		};
		gradient1D grad
		{
			{ -1.01, color_rgb(0, 0, 0, 255) },
			{ -0.9375, color_rgb(0,   0,   0, 255) },
			{ -0.8750, color_rgb(44, 216, 242, 255) },
			{ 0.0000, color_rgb(191, 191, 191, 255) },
			{ 0.5000, color_rgb(33, 116, 125, 255) },
			{ 0.7500, color_rgb(45, 113,  98, 255) },
			{ 1.01, color_rgb(255, 176, 192, 255) }
		};
		run(w, h, f1, f2, grad);
	}
}}

int main(int argc, char** argv)
{
	paranoise::examples::run();

	return 0;
}
