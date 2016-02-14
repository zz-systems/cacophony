#pragma once

#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"

namespace paranoise { namespace examples {
	using namespace util;

	SIMD_ENABLE(TReal, TInt)
		Module<TReal> generate_granite()
	{
		cout << "generate granite texture" << endl;

		auto primaryGranite = billow<TReal, TInt>(8.0, 2.18359375, 0.625, 6, 100);
		auto baseGrains = voronoi<TReal, TInt>(16, 2, 1, true);
		auto perturbate = turbulence<TReal, TInt>(1.0 / 8.0, 6, 4.0, 2.0, 0.5, 2);

		Module<TReal> combinedGranite = [=](const auto& c) { return primaryGranite(c) + baseGrains(c) * -0.5; };

		return [=](const auto& c) { return perturbate(c, combinedGranite); };
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

}}