// Examples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "run.h"
#include "wood.h"
#include "granite.h"
#include "planet.h"
#include "run.h"
#include "json.h"

#include "../paranoise/parallel/all.h"
#include "../paranoise/engine/all.h"

namespace zzsystems { namespace paranoise { namespace examples {
	using namespace simdal;
	using namespace scheduler;

	template<typename capability, typename vreal, typename vint>
	auto exec(const scheduler_settings& settings)
	{
		cout << ">>dispatch: using " << static_dispatcher<capability>::unit_name() << " branch" << endl;

		cpu_scheduler<vreal> s(
			generate_from_json<vreal, vint>(),
			nullptr,
			settings
			);

		return s();
	}

	void run()
	{
		system_info sysinfo;
		cout << sysinfo;

		engine e;
		
		e.compile_file("perlin_test.json");
		auto flags = e.info.feature_flags;

		auto w = e.settings.dimensions.x, h = e.settings.dimensions.y;

		auto f1 = [&]()
		{
			//e.info.setFlag(CAPABILITY_AVX2, false);			
			e.info.feature_flags = flags;
			return e.run({0, 0, 0});
		};

		auto f2 = [&]()
		{
			e.info.feature_flags = CAPABILITY_NONE;		
			return e.run({ 0, 0, 0 });
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
}}}

int main(int argc, char** argv)
{
	zzsystems::paranoise::examples::run();

	return 0;
}
