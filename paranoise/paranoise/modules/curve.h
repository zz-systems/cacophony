#pragma once
#ifndef PARANOISE_MODULES_CURVE
#define PARANOISE_MODULES_CURVE

#include <map>
#include <algorithm>
#include "../noisegenerators.h"
//#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	//using namespace x87compat;

	SIMD_ENABLE_F(TReal)
	struct curve_settings
	{
		Module<TReal> source;
		std::map<double, double> points;
	};

	SIMD_ENABLE(TReal, TInt)
	inline TReal curve(const Vector3<TReal>& coords, const curve_settings<TReal>& settings)
	{
		auto cpc = settings.points.count();		
		assert(cpc >= 4);

		auto val = settings.source(coords);
		TInt startIndexes;
		
		for (int i = 0, auto iter = settings.points.begin(); iter != settings.points.end(); i++, iter++)
		{
			startIndexes = startIndexes | (iter->first > val) & i;
		}

		auto i3 = clamp(startIndexes + 1, 0, cpc - 1);
		auto i2 = clamp(startIndexes    , 0, cpc - 1);
		auto i1 = clamp(startIndexes - 1, 0, cpc - 1);
		auto i0 = clamp(startIndexes - 2, 0, cpc - 1);

		auto mask = i1 == i2;
		
		//auto result = (i2 == i1) & 
		TReal in1, in0, alpha, cp3, cp2, cp1, cp0, result;

		for (int i = 0; i < elem_count(in1); i++)
		{
			in0.values[i] = std::advance(settings.points.begin(), i1.values[i])->first;
			in1.values[i] = std::advance(settings.points.begin(), i2.values[i])->first;			

			cp3.values[i] = std::advance(settings.points.begin(), i3.values[i])->first;
			cp2.values[i] = std::advance(settings.points.begin(), i2.values[i])->first;
			cp1.values[i] = std::advance(settings.points.begin(), i1.values[i])->first;
			cp0.values[i] = std::advance(settings.points.begin(), i0.values[i])->first;

			result.values[i] = std::advance(settings.points.begin(), i1.values[i])->second;
		}

		alpha = (val - in0) / (in1 - in0);

		return result & mask | ~mask & cerp(cp0, cp1, cp2, cp3, cp4, alpha);
	}
}}
#endif