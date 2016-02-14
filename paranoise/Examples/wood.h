#pragma once

#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"
namespace paranoise { namespace examples {

	using namespace util;

	SIMD_ENABLE(TReal, TInt)
		Module<TReal> generate_wood()
	{
		cout << "generate wood texture" << endl;

		Module<TReal> baseWood = [=](const auto& c) { return concentric_cylinders<TReal>(16.0f, c); };
		Module<TReal> woodGrainNoise = [=](const auto& c) { return perlin<TReal, TInt>(c, perlin_settings(48, 2.20703125f, 0.5f, 3, 21337)); };
		Module<TReal> scaledBaseWoodGrain = [=](const auto& c) { return scale_input<TReal>(woodGrainNoise, { 1, 0.25f, 1 }, c); };
		Module<TReal> woodGrain = [=](const auto& c) { return scale_output_biased<TReal>(scaledBaseWoodGrain, 0.25f, 0.125f, c);};

		Module<TReal> combinedWood = [=](const auto& c) { return add<TReal>(baseWood, woodGrain, c); };
		Module<TReal> perturbedWood = [=](const auto& c) { return turbulence<TReal, TInt>(combinedWood, c, turbulence_settings(1.0f / 256.0f, 4, 4.0, 2.0f, 0.5, 6, 111));};

		Module<TReal> translatedWood = [=](const auto& c) { return translate<TReal>(perturbedWood, { 0, 0, 1.48f }, c);};
		Module<TReal> rotatedWood = [=](const auto& c) { return rotate<TReal, TInt>(translatedWood, c, rotate_settings<TReal>({ 84.0f, 0, 0 }));};

		Module<TReal> finalWood = [=](const auto& c) { return turbulence<TReal, TInt>(rotatedWood, c, turbulence_settings(1.0f / 64.0f, 4, 2.0f, 2.0f, 0.5f, 6, 222));};

		return finalWood;
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