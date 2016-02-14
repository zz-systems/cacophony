#pragma once
#ifndef PARANOISE_MODULES_TERRACE
#define PARANOISE_MODULES_TERRACE

#include <map>
#include <algorithm>
#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE(TReal, TInt)
	struct terrace
	{
		std::map<float, double> points;
		bool invert = false;

		terrace(const std::initializer_list<std::pair<const float, float>> &points, bool invert = false)
			: points(points), invert(invert)
		{}

		inline TReal operator()(const Vector3<TReal>& coords, const Module<TReal>& source) const
		{
			auto cpc = points.count();
			assert(cpc >= 4);

			auto val = source(coords);
			TInt startIndexes;

			for (int i = 0, auto iter = points.begin(); iter != points.end(); i++, iter++)
			{
				startIndexes |= sel(iter->first > val, i, consts<TInt>::zero());
			}

			auto i1 = clamp(startIndexes - consts<TInt>::one(), consts<TInt>::zero(), cpc - consts<TInt>::one());
			auto i0 = clamp(startIndexes, consts<TInt>::zero(), cpc - consts<TInt>::one());

			auto mask = i1 == i2;

			//auto result = (i2 == i1) & 
			TReal in1, in0, alpha, cp3, cp2, cp1, cp0, result;

			for (int i = 0; i < dim<TReal>(); i++)
			{
				in0.values[i] = std::advance(settings.points.begin(), i1.values[i])->first;
				in1.values[i] = std::advance(settings.points.begin(), i2.values[i])->first;

				cp1.values[i] = std::advance(settings.points.begin(), i1.values[i])->first;
				cp0.values[i] = std::advance(settings.points.begin(), i0.values[i])->first;

				result.values[i] = std::advance(settings.points.begin(), i1.values[i])->second;
			}

			alpha = (val - in0) / (in1 - in0);

			if (invert)
			{
				alpha = 1.0 - alpha;
				std::swap(cp0, cp1);
			}

			alpha *= alpha;

			return sel(mask, result, lerp(cp0, cp1, alpha));
		}

		// inline operator (Module<TReal>)() { return operator(); }
	};	
}}
#endif