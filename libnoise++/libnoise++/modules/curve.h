#pragma once
#ifndef NOISEPP_MODULES_CURVE
#define NOISEPP_MODULES_CURVE

#include <map>
#include <algorithm>
#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	SIMD_ENABLE_F(TReal)
	struct curve_settings
	{
		double frequency = 1.0, lacunarity = 2.0, persistence = 0.5;
		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;
		Module<TReal> source;
		std::map<double, double> points;
	};

	SIMD_ENABLE(TReal, TInt)
	inline TReal curve(const Vector3<TReal>& coords, const curve_settings<TReal>& settings)
	{
		assert(settings.points.count() >= 4);

		auto val = settings.source(coords);
		TInt startIndexes;
				
		for (int i = 0, auto iter = settings.points.begin(); iter != settings.points.end(); i++, iter++)
		{
			startIndexes = startIndexes | (iter->first > val) & i;
		}
	}
}}
#endif