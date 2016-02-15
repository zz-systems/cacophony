#pragma once
#ifndef PARANOISE_MODULES_CURVE
#define PARANOISE_MODULES_CURVE

#include <map>
#include <vector>
#include <algorithm>

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE(TReal, TInt)
	struct curve
	{
		std::vector<std::pair<const TReal, TReal>> points;

		curve(const std::initializer_list<std::pair<const TReal, TReal>>& lst) : points(lst)
		{}

		inline TReal operator()(const Vector3<TReal>& coords, const Module<TReal> &source) const
		{
			auto val = source(coords);
			TReal v3, v2, v1, v0, mask;

			// TODO: bounds
			for (int i = 0; i < points.size() - 1; i++)
			{
				mask = points[i].first > val;

				v3 = sel(mask, points[i + 1].second, v3);
				v2 = sel(mask, points[i	   ].second, v2);
				v1 = sel(mask, points[i - 1].second, v1);
				v0 = sel(mask, points[i - 2].second, v0);
			}
			
			auto alpha	= (val - v1) / (v2 - v1);

			return cerp(v0, v1, v2, v3, alpha);
		}

		// inline operator (Module<TReal>)() { return operator(); }
	};	
}}
#endif