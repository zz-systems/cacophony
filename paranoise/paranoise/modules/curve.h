#pragma once
#ifndef PARANOISE_MODULES_CURVE
#define PARANOISE_MODULES_CURVE

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED
	class curve : 
		public cloneable<module_base<vreal, vint>, curve<SIMD_T>>,
		public serializable<json>
	{
	public:
		vector<pair<const vreal, vreal>> points;

		MODULE_PROPERTY(source, 0)
			
		curve(const initializer_list<pair<const vreal, vreal>>& points) 
			: cloneable(1), points(points)
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

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto val = get_source()(coords);
			
			vreal v3, v2, v1, v0, set_value, already_set = fastload<vreal>::_0();
		
			size_t i0, i1, i2, i3;

			// TODO: bounds
			for (size_t i = 0; i < points.size(); i++)
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
	};	
}}}
#endif