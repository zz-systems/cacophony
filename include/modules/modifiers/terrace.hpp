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

	MODULE(mod_terrace)
	{
	public:
		std::vector<zfloat, aligned_allocator<zfloat, 32>> points;

		bool invert = false;

		mod_terrace(const std::initializer_list<zfloat> &points = {}, bool invert = false)
			: BASE(mod_terrace)::cloneable(1), points(points), invert(invert)
		{}

		void deserialize(const json &source) override
		{
			if (source["points"] != nullptr && source["points"].is_array())
			{
				for (auto point : source["points"])
				{
					points.push_back(point.get<float>());
				}
			}

		}

		MODULE_PROPERTY(source, 0)

		zfloat operator()(const vec3<zfloat>& coords) const override
		{
			auto cpc = points.size();
			assert(cpc >= 4);

			zfloat out1, out0, set_value, already_set;

			out1 = out0 = already_set = 0;

			size_t i0, i1;
			auto val = get_source()(coords);

			for (size_t i = 0; i < points.size(); i++)
			{
				// Point > supplied values?
				set_value = points[i] > val;

				// Get indexces
				i1 = vclamp<size_t>(i - 1, 0, points.size() - 1);
				i0 = vclamp<size_t>(i,     0, points.size() - 1);

				// mask out already set values
				auto select_new = set_value && !already_set;

				// Set values if not set yet
				out1 = vsel(select_new, points[i1], out1);
				out0 = vsel(select_new, points[i0], out0);

				// Fill accumulating mask
				already_set = already_set || set_value;

				// If all results are set, quit prematurely
				if (is_set(already_set))
					break;
			}


			// blend results
			auto alpha = (val - out0) / (out1 - out0);
						
			if (invert)
			{
				alpha = 1 - alpha;
				std::swap(out0, out1);
			}

			alpha *= alpha;

			return vsel(out1 == out0, out0, lerp(out0, out1, alpha));
		}		
	};	
}}
