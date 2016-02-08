// Examples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include "lib/CImg-1.6.9/CImg.h"


#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"


using namespace paranoise;
using namespace paranoise::parallel;
using namespace paranoise::module;
using namespace paranoise::scheduler;
using namespace paranoise::util;

using namespace std;
using namespace std::placeholders;

using namespace std::chrono;

SIMD_ENABLE(TReal, TInt)
Module<TReal> generate_granite_bind()
{
	auto bs = billow_settings<TReal, TInt>(8.0, 2.18359375, 0.625, 6, 100);
	auto vs = voronoi_settings(16, 2, 1, true);
	auto ts = turbulence_settings<TReal, TInt>(1.0 / 8.0, 6, 4.0, 2.0, 0.5, 2);

	Module<TReal> primaryGranite	= [=](const auto& c) { return billow<TReal, TInt>(c, billow_settings<TReal, TInt>(8.0, 2.18359375, 0.625, 6, 100)); };
	Module<TReal> baseGrains		= [=](const auto& c) { return voronoi<TReal, TInt>(c, voronoi_settings(16, 2, 1, true)); };
	
	Module<TReal> scaledGrains		= [=](const auto& c) { return scale_output_biased<TReal>(baseGrains, -0.5, 0.0, c);};
	Module<TReal> combinedGranite	= [=](const auto& c) { return primaryGranite(c) + scaledGrains(c); };
	Module<TReal> finalGranite		= [=](const auto& c) { return turbulence<TReal, TInt>(combinedGranite, c, turbulence_settings<TReal, TInt>(1.0 / 8.0, 6, 4.0, 2.0, 0.5, 2)); };

	return finalGranite;
}

SIMD_ENABLE(TReal, TInt)
Module<TReal> generate_wood_bind()
{
	Module<TReal> primaryGranite		= [=](const auto& c) { return billow<TReal, TInt>(c, billow_settings(48, 2.20703125f, 0.5f, 3)); };
	//Module<TReal> baseGrains			= std::bind(voronoi<TReal, TInt>, _1, voronoi_settings(16, 2, 1, true));


	Module<TReal> baseWood				= [=](const auto& c) { return concentric_cylinders<TReal>(16.0f, c); };
	Module<TReal> woodGrainNoise		= [=](const auto& c) { return perlin<TReal, TInt>(c, perlin_settings(48, 2.20703125f, 0.5f, 3, 21337)); };
	Module<TReal> scaledBaseWoodGrain = [=](const auto& c) { return scale_input<TReal>(woodGrainNoise, { 1, 0.25f, 1 }, c); };
	Module<TReal> woodGrain				= [=](const auto& c) { return scale_output_biased<TReal>(scaledBaseWoodGrain, 0.25f, 0.125f, c);};

	Module<TReal> combinedWood			= [=](const auto& c) { return add<TReal>(baseWood, woodGrain, c); };
	Module<TReal> perturbedWood			= [=](const auto& c) { return turbulence<TReal, TInt>(combinedWood, c, turbulence_settings(1.0f / 256.0f, 4, 4.0, 2.0f, 0.5, 6, 111));};

	Module<TReal> translatedWood		= [=](const auto& c) { return translate<TReal>(perturbedWood, { 0, 0, 1.48f }, c);};
	Module<TReal> rotatedWood			= [=](const auto& c) { return rotate<TReal, TInt>(translatedWood, c, rotate_settings<TReal>({ 84.0f, 0, 0 }));};

	Module<TReal> finalWood				= [=](const auto& c) { return turbulence<TReal, TInt>(rotatedWood, c, turbulence_settings(1.0f / 64.0f, 4, 2.0f, 2.0f, 0.5f, 6, 222));};

	return finalWood;
}

gradient1D granite_grad =
{
	{ -1.0000, color_rgb(0,   0,   0, 255) },
	{ -0.9375, color_rgb(0,   0,   0, 255) },
	{ -0.8750, color_rgb(216, 216, 242, 255) },
	{ 0.0000, color_rgb(191, 191, 191, 255) },
	{ 0.5000, color_rgb(210, 116, 125, 255) },
	{ 0.7500, color_rgb(210, 113,  98, 255) },
	{ 1.0000, color_rgb(255, 176, 192, 255) }
};

gradient1D wood_grad =
{	
	//{ -15000,	color_rgb(219, 124, 8, 255) },
	{ -1.0000,	color_rgb(189, 94, 4, 255) },
	{ 0.50,		color_rgb(144, 48, 6, 255) },
	{ 1.0000,	color_rgb( 60, 10, 8, 255) },
	//{ 15000,	color_rgb(30, 5, 4, 255) }
};

int main()
{
	bool avxSupported = false;

	// If Visual Studio 2010 SP1 or later
#if (_MSC_FULL_VER >= 160040219)
	// Checking for AVX requires 3 things:
	// 1) CPUID indicates that the OS uses XSAVE and XRSTORE
	//     instructions (allowing saving YMM registers on context
	//     switch)
	// 2) CPUID indicates support for AVX
	// 3) XGETBV indicates the AVX registers will be saved and
	//     restored on context switch
	//
	// Note that XGETBV is only available on 686 or later CPUs, so
	// the instruction needs to be conditionally run.
	int cpuInfo[4];
	__cpuid(cpuInfo, 1);

	bool osUsesXSAVE_XRSTORE = cpuInfo[2] & (1 << 27) || false;
	bool cpuAVXSuport = cpuInfo[2] & (1 << 28) || false;

	if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
	{
		// Check if the OS will save the YMM registers
		unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
		avxSupported = (xcrFeatureMask & 0x6) || false;
	}
#endif

	if (avxSupported)
	{
		cout << ("AVX is supported") << endl;
	}
	else
	{
		cout << ("AVX is NOT supported\n") << endl;;
	}

	cout << "generate granite" << endl;
	_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

	using _float = float4;
	using _int = int4;

	using vec = Vector3<float>;
	using fvec = Vector3<_float>;

	const int w = 4096; //8192 * 2;
	const int h = 4096; //8192 * 2;

	scheduler_settings settings({ w, h, 1 }, 1337, true);

	auto simd_t1 = high_resolution_clock::now();

	auto result = schedule2D<_float>(
		generate_granite_bind<_float, _int>(),
		[w, h](const fvec &c) { return (c) * fvec(2, 2, 2) / fvec(w, h, h) - fvec(1, 1, 1);},
		settings);

	auto simd_t2 = high_resolution_clock::now();
	auto simd_duration = duration_cast<milliseconds>(simd_t2 - simd_t1).count();
	auto v = *result;
	cout << "SIMD Duration (Code + Compiler optimized): " << simd_duration << endl;

	auto sisd_t1 = high_resolution_clock::now();

	auto discard = schedule2D<float>(
		generate_granite_bind<float, int>(),
		[w, h](const vec &c) { return (c)* vec(2, 2, 2) / vec(w, h, h) - vec(1, 1, 1);},
		settings);

	auto sisd_t2 = high_resolution_clock::now();
	auto sisd_duration = duration_cast<milliseconds>(sisd_t2 - sisd_t1).count();

	
	
	cout << "SISD Duration (Compiler optimized): " << sisd_duration << endl;
	cout << "Speedup : " << (double)sisd_duration / simd_duration << endl << endl;

	cout << "Minval: " << *min_element(begin(v), end(v)) << endl << endl;
	cout << "Maxval: " << *max_element(begin(v), end(v)) << endl << endl;

	cimg_library::CImg<uint8> img(w, h, 1, 3);
	
	//std::ofstream ostr("R:/Temp/dump.tab", ios_base::out);

	
	parallel_for(0, (int)v.size(), [&](const auto i)
	//for (int i = 0; i < v.size(); i++)	
	{
		int x = i % w;
		int y = i / w;
		

		auto color = granite_grad.getColor(v[i]);

		img(x, y, 0) = color.r;
		img(x, y, 1) = color.g;
		img(x, y, 2) = color.b;
	}
	);

	img.display("kebabtron", true);
	

    return 0;
}


