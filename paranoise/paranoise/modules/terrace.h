#pragma once
#ifndef PARANOISE_MODULES_TERRACE
#define PARANOISE_MODULES_TERRACE

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED
	class terrace : 
		public cloneable<module_base<vreal, vint>, terrace<SIMD_T>>,
		public serializable<json>
	{
	public:
		vector<pair<const vreal, vreal>> points;
		bool invert = false;

		terrace(const initializer_list<pair<const vreal, vreal>> &points = {}, bool invert = false)
			: cloneable(1), points(points), invert(invert)
		{}

		const json& operator <<(const json &source) override
		{
			if (source["points"] != nullptr && source["points"].is_array())
			{
				for (auto point : source["points"])
				{
					points.push_back(
						make_pair<vreal, vreal>(
							point["in"].get<float>(),
							point["out"].get<float>()));
				}
			}

			return source;
		}

		MODULE_PROPERTY(source, 0)

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto cpc = points.size();
			assert(cpc >= 4);

			vreal cp1, cp0, set_value, already_set = fastload<vreal>::_0();

			size_t i0, i1;
			auto val = get_source()(coords);

			for (size_t i = 0; i < points.size(); i++)
			{
				// Point > supplied values?
				set_value = points[i].first > val;

				// Get indeces				
				i1 = vclamp<int>(i - 1, 0, points.size() - 1);
				i0 = vclamp<int>(i, 0, points.size() - 1);

				// Set values if not set yet
				
				cp1 = vsel(set_value && !already_set, points[i1].second, cp1);
				cp0 = vsel(set_value && !already_set, points[i0].second, cp0);

				// Fill accumulating mask
				already_set = already_set || set_value;

				// If all results are set, quit prematurely
				if (static_cast<bool>(already_set))
					break;
			}

			// prevent division by zero by adding an offset
			cp0 = vsel(cp0 == cp1, cp1 + cp0, cp0);

			// blend results
			auto alpha = (val - cp0) / (cp1 - cp0);// +numeric_limits<float>::epsilon());
						
			if (invert)
			{
				alpha = fastload<vreal>::_1() - alpha;
				std::swap(cp0, cp1);
			}

			alpha *= alpha;

			return lerp(cp0, cp1, alpha);
		}
	};	
}}}
#endif