// Examples.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
#include "stdafx.h"
#endif

#include <iostream>

#include "run.h"
//#include "wood.h"
//#include "granite.h"
//#include "planet.h"
//#include "run.h"
//#include "json.h"

//#include "../gorynych/gorynych/gorynych.h"
//#include "../solowej/engine/all.h"
#include "../solowej/library/solowejlib.h"

namespace zzsystems { namespace solowej { namespace examples {
	using namespace gorynych;
	using namespace util;

	void run()
	{
		system_info sysinfo;
		cout << sysinfo;

		//engine e;

		//e.info.feature_flags = CAPABILITY_NONE;
		//sysinfo.setFlag(CAPABILITY_AVX2, false);
		//sysinfo.setFlag(CAPABILITY_AVX1, false);
		//e.info.setFlag(CAPABILITY_SSE42, false);
		//e.info.setFlag(CAPABILITY_SSE41, false);
		//e.info.setFlag(CAPABILITY_FMA3, false);
		//e.info.setFlag(CAPABILITY_FMA4, false);

		solowej_get_engine("examples")->info.feature_flags = ( sysinfo.feature_flags);
		solowej_get_engine("examples")->compile_file("config/planet.json");

		auto flags = sysinfo.feature_flags;

        auto d = solowej_get_engine("examples")->settings.dimensions;

		auto f1 = [&]()
		{
			//std::vector<float, aligned_allocator<float, 32>> t(w * h * 1);
			auto t = make_shared<vector<float>>(d.x * d.y * d.z);

			::solowej_get_engine("examples")->info.feature_flags = (flags);
			::solowej_run("examples", &t->at(0), 0, 0, 0);

			return t;
		};

		auto f2 = [&]()
		{
			auto t = make_shared<vector<float>>(d.x * d.y * d.z);

			::solowej_get_engine("examples")->info.feature_flags = (CAPABILITY_NONE);
			::solowej_run("examples", &t->at(0), 0, 0, 0);

			return t;
		};

		gradient1D grad
		{
			{ -1.01f, color_rgb(0, 0, 0, 255) },
			{ -0.9375f, color_rgb(0,   0,   0, 255) },
			{ -0.8750f, color_rgb(44, 216, 242, 255) },
			{ 0.0000f, color_rgb(191, 191, 191, 255) },
			{ 0.5000f, color_rgb(33, 116, 125, 255) },
			{ 0.7500f, color_rgb(45, 113,  98, 255) },
			{ 1.01f, color_rgb(255, 176, 192, 255) }
		};

		zzsystems::solowej::examples::run(d.x, d.z, f1, f2, grad);
	}
}}}

int main(int argc, char** argv)
{
	zzsystems::solowej::examples::run();

	return 0;
}
