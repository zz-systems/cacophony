#pragma once

#include "../solowej/modules/all.h"
#include "../solowej/util/color.h"

namespace zzsystems { namespace solowej { namespace examples {
	using namespace modules;
	using namespace math;
	using namespace util;

	SIMD_ENABLE(vreal, vint)
		Module<vreal> generate_wood()
	{
		cout << "generate wood texture" << endl;
		
		perlin<vreal, vint> woodGrainNoise(48, 2.20703125f, 0.5f, 3, 21337);
		turbulence<vreal, vint> perturbedWood(1.0f / 256.0f, 4, 4.0, 2.0f, 0.5, 1);
		rotate<vreal, vint> rotatedWood({ 84.0f, 0, 0 });
		turbulence<vreal, vint> finalWood(1.0f / 64.0f, 4, 2.0f, 2.0f, 0.5f, 2);

		//auto baseGrains = voronoi<vreal, vint>(16, 2, 1, true);

		Module<vreal> baseWood = [=](const auto& c) { return cylinders<vreal>(c, 16.0f); };
		
		//Module<vreal> baseWood = [=](const auto& c) { return baseGrains(c); };
		Module<vreal> scaledBaseWoodGrain = [=](const auto& c) { return scale_input<vreal>(c, { 1.0f, 0.25f, 1.0f }, woodGrainNoise); };
		Module<vreal> woodGrain = [=](const auto& c) { return scale_output_biased<vreal>(c, 0.25f, 0.125f, scaledBaseWoodGrain);};

		Module<vreal> combinedWood = [=](const auto& c) { return baseWood(c) + woodGrain(c); };
		Module<vreal> _perturbedWood = [=](const auto& c) { return perturbedWood(c, combinedWood);};

		Module<vreal> translatedWood = [=](const auto& c) { return translate<vreal>(c, { 0.0f, 0.0f, 1.48f }, _perturbedWood);};
		Module<vreal> _rotatedWood = [=](const auto& c) { return rotatedWood(c, translatedWood);};

		return [=](const auto& c) { return finalWood(c, _rotatedWood);};

		//return  [=](const auto& c) { return cylinders<vreal>(c, 16.0f); };
	}

	gradient1D wood_grad =
	{
		//{ -15000,	color_rgb(219, 124, 8, 255) },
		{ -1.0000f,	color_rgb(189, 94, 4, 255) },
		{ 0.50f,		color_rgb(144, 48, 6, 255) },
		{ 1.0000f,	color_rgb(60, 10, 8, 255) },
		//{ 15000,	color_rgb(30, 5, 4, 255) }
	};
}}}