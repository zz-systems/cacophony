#pragma once
#ifndef PARANOISE_MODULES_CURVE
#define PARANOISE_MODULES_CURVE

#include <map>
#include <vector>
#include <algorithm>

#include "../parallel/all.h"
#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace parallel;
	using namespace x87compat;

	SIMD_ENABLE(TReal, TInt)
	struct curve
	{
		vector<pair<const TReal, TReal>> points;

		curve(const initializer_list<pair<const TReal, TReal>>& lst) : points(lst)
		{}

		inline TReal operator()(const Vector3<TReal>& coords, const Module<TReal> &source) const
		{
			auto val = source(coords);
			
			TReal v3, v2, v1, v0, set_value, already_set = fastload<TReal>::_0();
		
			int i0, i1, i2, i3;

			// TODO: bounds
			for (int i = 0; i < points.size(); i++)
			{
				// Point > supplied values?
				set_value = points[i].first > val;	

				// Get indeces
				i3 = vclamp<int>(i + 1, 0, points.size() - 1);
				i2 = vclamp<int>(i,		0, points.size() - 1);
				i1 = vclamp<int>(i - 1, 0, points.size() - 1);
				i0 = vclamp<int>(i - 2, 0, points.size() - 1);
				
				// Set values if not set yet
				v3 = vsel(set_value && !already_set, points[i3].second, v3);
				v2 = vsel(set_value && !already_set, points[i2].second, v2);

				v1 = vsel(set_value && !already_set, points[i1].second, v1);
				v0 = vsel(set_value && !already_set, points[i0].second, v0);				

				// Fill accumulating mask
				already_set = already_set || set_value;

				// If all results are set, quit prematurely
				if (static_cast<bool>(already_set))
					break;
			}

			// prevent division by zero by adding an offset
			v1 = vsel(v2 == v1, v1 + v2, v1);

			// blend results
			auto alpha = (val - v1) / (v2 - v1);// +numeric_limits<float>::epsilon());

			//return vsel(result == ones, cerp(v0, v1, v2, v3, alpha), result);
			return cerp(v0, v1, v2, v3, alpha);
		}

		// inline operator (Module<TReal>)() { return operator(); }
	};	
}}
#endif