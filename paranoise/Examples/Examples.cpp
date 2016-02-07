// Examples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>

#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"

#include "lib/CImg-1.6.9/CImg.h"

using namespace paranoise;
using namespace paranoise::parallel;
using namespace paranoise::module;
using namespace paranoise::scheduler;
using namespace paranoise::util;

using namespace std;
using namespace std::placeholders;

using namespace cimg_library;


SIMD_ENABLE(TReal, TInt)
Module<TReal> generate_granite_bind()
{
	Module<TReal> primaryGranite	= std::bind(billow<TReal, TInt>,  _1, billow_settings(8.0, 2.18359375, 0.625, 6, 1000));
	Module<TReal> baseGrains		= std::bind(voronoi<TReal, TInt>, _1, voronoi_settings(16, 2, 1, true));
	
	Module<TReal> scaledGrains		= std::bind(scale_output_biased<TReal>, baseGrains, (TReal)-0.5, (TReal)0.0, _1);
	Module<TReal> combinedGranite	= std::bind(add<TReal>,	primaryGranite, scaledGrains, _1);
	Module<TReal> finalGranite		= std::bind(turbulence<TReal, TInt>, combinedGranite, _1, turbulence_settings(1.0 / 8.0, 6, 4.0, 2.0, 0.5, 6, 2));

	return baseGrains;
}

SIMD_ENABLE(TReal, TInt)
Module<TReal> generate_wood_bind()
{
	Module<TReal> baseWood				= [=](const auto& c) { return concentric_cylinders<TReal>((TReal)16.0f, c); };
	Module<TReal> woodGrainNoise		= [=](const auto& c) { return perlin<TReal, TInt>(c, perlin_settings(48, 2.20703125f, 0.5f, 3)); };
	Module<TReal> scaledBaseWoodGrain	= [=](const auto& c) { return scale_input<TReal>(woodGrainNoise, Vector3<TReal>(1, 0.25f, 1), c); };
	Module<TReal> woodGrain				= [=](const auto& c) { return scale_output_biased<TReal>(scaledBaseWoodGrain, (TReal)0.25f, (TReal)0.125f, c);};

	Module<TReal> combinedWood			= [=](const auto& c) { return add<TReal>(baseWood, woodGrain, c); };
	Module<TReal> perturbedWood			= [=](const auto& c) { return turbulence<TReal, TInt>(combinedWood, c, turbulence_settings(1.0f / 256.0f, 4, 4.0, 2.0f, 0.5, 6, 1));};

	Module<TReal> translatedWood		= [=](const auto& c) { return translate<TReal>(perturbedWood, Vector3<TReal>((TReal)0, (TReal)0, (TReal)1.48f), c);};
	Module<TReal> rotatedWood			= [=](const auto& c) { return rotate<TReal, TInt>(translatedWood, c, rotate_settings<TReal>(Vector3<float>(84, 0, 0)));};

	Module<TReal> finalWood				= [=](const auto& c) { return turbulence<TReal, TInt>(rotatedWood, c, turbulence_settings(1.0f / 64.0f, 4, 2.0f, 2.0f, 0.5f, 6, 2));};

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
	{ -1.0000,	color_rgb(189, 94, 4, 255) },
	{ 0.50,		color_rgb(144, 48, 6, 255) },
	{ 1.0000,	color_rgb( 60, 10, 8, 255) }
};

int main()
{
	cout << "generate granite" << endl;
	_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

	using _float = float8;
	using _int = int8;

	auto result = schedule2D<_float>(generate_wood_bind<_float, _int>(), id<Vector3<_int>>, scheduler_settings(Vector3<int>(512, 512, 0), 1337, true));
	CImg<uint8> img(512, 512, 1, 3);
	
	auto index = 0;
	auto v = *result;

	for (int y = 0; y < v.size(); y++)
	for (int x = 0; x < v[y].size(); x++)
	{
		auto color = wood_grad.getColor(v[y][x]);

		img(x, y, 0) = color.r;
		img(x, y, 1) = color.g;
		img(x, y, 2) = color.b;
	}

	img.display("kebabtron", true);
	//CImgDisplay disp;
	//disp.display(img);

    return 0;
}


