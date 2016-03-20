#pragma once

#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"
namespace paranoise { namespace examples {
	using namespace module;
	using namespace util;

	SIMD_ENABLE(TReal, TInt)
		Module<TReal> generate_wood()
	{
		cout << "generate wood texture" << endl;
		
		perlin<TReal, TInt> woodGrainNoise(48, 2.20703125f, 0.5f, 3, 21337);
		turbulence<TReal, TInt> perturbedWood(1.0f / 256.0f, 4, 4.0, 2.0f, 0.5, 1);
		rotate<TReal, TInt> rotatedWood({ 84.0f, 0, 0 });
		turbulence<TReal, TInt> finalWood(1.0f / 64.0f, 4, 2.0f, 2.0f, 0.5f, 2);

		//auto baseGrains = voronoi<TReal, TInt>(16, 2, 1, true);

		Module<TReal> baseWood = [=](const auto& c) { return cylinders<TReal>(c, 16.0f); };
		
		//Module<TReal> baseWood = [=](const auto& c) { return baseGrains(c); };
		Module<TReal> scaledBaseWoodGrain = [=](const auto& c) { return scale_input<TReal>(c, { 1.0f, 0.25f, 1.0f }, woodGrainNoise); };
		Module<TReal> woodGrain = [=](const auto& c) { return scale_output_biased<TReal>(c, 0.25f, 0.125f, scaledBaseWoodGrain);};

		Module<TReal> combinedWood = [=](const auto& c) { return baseWood(c) + woodGrain(c); };
		Module<TReal> _perturbedWood = [=](const auto& c) { return perturbedWood(c, combinedWood);};

		Module<TReal> translatedWood = [=](const auto& c) { return translate<TReal>(c, { 0.0f, 0.0f, 1.48f }, _perturbedWood);};
		Module<TReal> _rotatedWood = [=](const auto& c) { return rotatedWood(c, translatedWood);};

		return [=](const auto& c) { return finalWood(c, _rotatedWood);};

		//return  [=](const auto& c) { return cylinders<TReal>(c, 16.0f); };
	}

	gradient1D wood_grad =
	{
		//{ -15000,	color_rgb(219, 124, 8, 255) },
		{ -1.0000,	color_rgb(189, 94, 4, 255) },
		{ 0.50,		color_rgb(144, 48, 6, 255) },
		{ 1.0000,	color_rgb(60, 10, 8, 255) },
		//{ 15000,	color_rgb(30, 5, 4, 255) }
	};
}}