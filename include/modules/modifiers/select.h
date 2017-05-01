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

	MODULE(mod_select)
	{
	public:
		float edgeFalloff = 0.0;
		vreal lowerBound = -1.0, upperBound = 1.0;

		mod_select(float edgeFalloff = 0.0, float lowerBound = -1.0, float upperBound = 1.0) :
			BASE(mod_select)::cloneable(3),
			edgeFalloff(edgeFalloff), 
			lowerBound(lowerBound),
			upperBound(upperBound)
		{}

		MODULE_PROPERTY(a, 0)
		MODULE_PROPERTY(b, 1)
		MODULE_PROPERTY(controller, 2)

		const json& operator <<(const json &source) override
		{
			edgeFalloff = source.value<float>("edgeFalloff", 0.0f);
			lowerBound	= source.value<float>("lowerBound", -1.0f);
			upperBound	= source.value<float>("upperBound", 1.0f);

			return source;
		}

		//, const Module<vreal>& a, const Module<vreal>& b, const Module<vreal>& controller
		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto cv = get_controller()(coords);
			auto r0 = get_a()(coords);
			auto r1 = get_b()(coords);
					
			if (edgeFalloff > 0.0)
			{
				// Path for vectorized code
				
				// condition vectors
				// if(cv < lowerBound - edgeFalloff) ...
				auto mask4 = (cv < (lowerBound - edgeFalloff));
				// if(cv < lowerBound + edgeFalloff) ...
				auto mask3 = (cv < (lowerBound + edgeFalloff));
				// if(cv < upperBound - edgeFalloff) ...
				auto mask2 = (cv < (upperBound - edgeFalloff));
				// if(cv < upperBound + edgeFalloff) ...
				auto mask1 = (cv < (upperBound + edgeFalloff));
				// else
				auto mask0 = !(mask4 || mask3 || mask2 || mask1);

				// merge calculation vectors
				auto result = mask4 && r0;
				result |= mask3 & interpolate_bounds(r0, r1, cv, lowerBound, edgeFalloff);
				result |= mask2 & r1;
				result |= mask1 & interpolate_bounds(r1, r0, cv, upperBound, edgeFalloff);
				result |= mask0 & r0;

				
				return result;
			}
			else
			{
				auto mask = (cv < lowerBound) || (cv > upperBound);

				return vsel(mask, r0, r1);
			}
		}
	private:
		inline vreal interpolate_bounds(const vreal& from,
			const vreal& to,
			const vreal& control,
			const vreal& bound, const vreal& edgeFalloff) const
		{
			auto lowerCurve = bound - edgeFalloff;
			auto upperCurve = bound + edgeFalloff;

			auto sc = scurve3((control - lowerCurve) / (upperCurve - lowerCurve));
			return lerp(from, to, sc);
		}
	};


	// path for scalar code 
	// , const Module<float>& a, const Module<float>& b, const Module<float>& controller
	template<>
	inline float mod_select<float, int>::operator()(const vec3<float>& coords) const
	{
		auto cv = get_controller()(coords);
		auto r0 = get_a()(coords);
		auto r1 = get_b()(coords);

		if (edgeFalloff > 0.0)
		{			
			// condition vectors
			if (cv < lowerBound - edgeFalloff)
				return r0;

			if (cv < lowerBound + edgeFalloff)
				return interpolate_bounds(r0, r1, cv, lowerBound, edgeFalloff);

			if (cv < upperBound - edgeFalloff)
				return r1;

			if (cv < upperBound + edgeFalloff)
				return interpolate_bounds(r1, r0, cv, upperBound, edgeFalloff);

			return r0;			
		}
		else
		{
			auto mask = (cv < lowerBound) || (cv > upperBound);

			return vsel(mask, r0, r1);
		}
	}
}}