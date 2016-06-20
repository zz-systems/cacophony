//
// Vectorized and parallelized version of libnoise using the gorynych SIMD toolkit
// Copyright (C) 2015-2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Original libnoise: 
// Copyright (C) 2003, 2004 Jason Bevins
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "../dependencies.h"

namespace zzsystems { namespace solowej { namespace modules {
	using namespace gorynych;
	
	using namespace math;

	MODULE(mod_terrace)
	{
	public:
		vector<pair<const vreal, vreal>> points;
		bool invert = false;

		mod_terrace(const initializer_list<pair<const vreal, vreal>> &points = {}, bool invert = false)
			: BASE(mod_terrace)::cloneable(1), points(points), invert(invert)
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

			vreal cp1, cp0, set_value, already_set = cfl<vreal, 0>::val();

			size_t i0, i1;
			auto val = get_source()(coords);

			for (size_t i = 0; i < points.size(); i++)
			{
				// Point > supplied values?
				set_value = points[i].first > val;

				// Get indexces
				i1 = vclamp<size_t>(i - 1, 0, points.size() - 1);
				i0 = vclamp<size_t>(i,     0, points.size() - 1);

				// Set values if not set yet
				
				cp1 = vsel(set_value && !already_set, points[i1].second, cp1);
				cp0 = vsel(set_value && !already_set, points[i0].second, cp0);

				// Fill accumulating mask
				already_set = already_set || set_value;

				// If all results are set, quit prematurely
				if (is_set(already_set))
					break;
			}

			// prevent division by zero by adding an offset
			cp0 = vsel(cp0 == cp1, cp1 + cp0, cp0);

			// blend results
			auto alpha = (val - cp0) / (cp1 - cp0);// +numeric_limits<float>::epsilon());
						
			if (invert)
			{
				alpha = cfl<vreal, 1>::val() - alpha;
				std::swap(cp0, cp1);
			}

			alpha *= alpha;

			return lerp(cp0, cp1, alpha);
		}		
	};	
}}}