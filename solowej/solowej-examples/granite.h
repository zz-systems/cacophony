#pragma once

#include "../submodules/gorynych/gorynych/gorynych.h"
#include "../solowej/modules/all.h"
#include "../solowej/util/color.h"

namespace zzsystems { namespace solowej { namespace examples {
	using namespace modules;
	using namespace math;
	using namespace gorynych;
	using namespace util;

	SIMD_ENABLE(vreal, vint)
		Module<vreal> generate_granite()
	{
		cout << "generate granite texture" << endl;

		auto primaryGranite = billow<vreal, vint>(8.0, 2.18359375, 0.625, 6, 100);
		auto baseGrains		= voronoi<vreal, vint>(16, 2, 1, true);
		auto perturb		= turbulence<vreal, vint>(1.0 / 8.0, 6, 4.0, 2.0, 0.5, 2);

		Module<vreal> combinedGranite = [=](const vec3<vreal>& c) { return (primaryGranite(c) + baseGrains(c)) * static_cast<vreal>(-0.5f); };

		return [=](const auto& c) { return perturb(c, combinedGranite); };
	}

	gradient1D granite_grad =
	{
		{ -1.01f, color_rgb(0,   0,   0, 255) },
		{ -0.9375f, color_rgb(0,   0,   0, 255) },
		{ -0.8750f, color_rgb(216, 216, 242, 255) },
		{ 0.0000f, color_rgb(191, 191, 191, 255) },
		{ 0.5000f, color_rgb(210, 116, 125, 255) },
		{ 0.7500f, color_rgb(210, 113,  98, 255) },
		{ 1.01f, color_rgb(255, 176, 192, 255) }
	};

}}}