//
// Vectorized and parallelized version of libnoise using the zacc SIMD toolkit
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

#include "modules/dependencies.hpp"

namespace cacophony { namespace modules {
	using namespace zacc;
	using namespace math;

	MODULE(mod_curve)
	{
	public:
		std::vector<std::pair<const zfloat, zfloat>, aligned_allocator<zfloat, 32>> points;

		MODULE_PROPERTY(source, 0)

		mod_curve(const std::initializer_list<std::pair<const zfloat, zfloat>>& points = {})
			: BASE(mod_curve)::cloneable(1), points(points)
		{}

		void deserialize(const json &source) override
		{
			if (source["points"] != nullptr && source["points"].is_array()) 
			{
				for (auto point : source["points"])
				{
					points.push_back(
						std::make_pair<zfloat, zfloat>(
							point["in"].get<float>(),
							point["out"].get<float>()));
				}
			}

		}

		zfloat operator()(const vec3<zfloat>& coords) const override
		{
			auto cpc = points.size();
			assert(cpc >= 4);

			auto val = get_source()(coords);
			
			zfloat in1, in0, out3, out2, out1, out0, set_value, already_set;

			in1 = in0 = out3 = out2 = out1 = out0 = already_set = 0;

			size_t i0, i1, i2, i3;

			// TODO: bounds
			for (int i = 0; i < points.size(); i++)
			{
				// Point > supplied values?
				set_value = points[i].first > val;

				// Get indeces
				i3 = vclamp<int>(i + 1, 0, points.size() - 1);
				i2 = vclamp<int>(i,	    0, points.size() - 1);
				i1 = vclamp<int>(i - 1, 0, points.size() - 1);
				i0 = vclamp<int>(i - 2, 0, points.size() - 1);

				// mask out already set values
				auto select_new = set_value && !already_set;

				// Set values if not set yet
				in1 = vsel(select_new, points[i2].first, in1);
				in0 = vsel(select_new, points[i1].first, in0);

				out3 = vsel(select_new, points[i3].second, out3);
				out2 = vsel(select_new, points[i2].second, out2);

				out1 = vsel(select_new, points[i1].second, out1);
				out0 = vsel(select_new, points[i0].second, out0);

				// Fill accumulating mask
				already_set = already_set || set_value;

				// If all results are set, quit prematurely
				if (is_set(already_set))
					break;
			}

			// blend results
			auto alpha = (val - in0) / (in1 - in0);

			// return either out1 (when no proper value is found) or the cubic interpolated blended result
			return vsel(in0 == in1, out1, cerp(out0, out1, out2, out3, alpha));
		}
	};	
}}