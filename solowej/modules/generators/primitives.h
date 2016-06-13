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

namespace zzsystems { namespace solowej { namespace modules
{
	using namespace math;
	using namespace gorynych;

	MODULE(mod_spheres)
	{
	public:
		vreal frequency;
		mod_spheres(const vreal &frequency = 1.0f)
			: BASE(mod_spheres)::cloneable(0), frequency(frequency)
		{}

		const json& operator<<(const json &source) override
		{
			frequency = source.value<float>("frequency", 1.0f);
			return source;
		}

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto _coords = coords * frequency;

			auto distFromCenter			= vsqrt(dot(_coords, _coords));
			auto distFromSmallerSphere	= distFromCenter - vfloor(distFromCenter);
			auto distFromLargerSphere	= cfl<vreal, 1>::val() - distFromSmallerSphere;

			auto nearestDist			= vmin(distFromSmallerSphere, distFromLargerSphere);

			// Puts it in the -1.0 to +1.0 range. [ 1 - (nearestDist * 4) ] => [ (-4 * nearestDist) + 1 ]
			return vfmadd(nearestDist, -cfl<vreal, 4>::val(), cfl<vreal, 1>::val());
		}
	};

	MODULE(mod_cylinders)
	{
	public:
		vreal frequency;
		mod_cylinders(const vreal &frequency = 1.0f)
			: BASE(mod_cylinders)::cloneable(0), frequency(frequency)
		{}

		const json& operator<<(const json &source) override
		{
			frequency = source.value<float>("frequency", 1.0f);
			return source;
		}

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto distFromCenter			= vsqrt(coords.x * coords.x + coords.z * coords.z) * frequency;
			auto distFromSmallerSphere	= distFromCenter - vfloor(distFromCenter);
			auto distFromLargerSphere	= cfl<vreal, 1>::val() - distFromSmallerSphere;

			auto nearestDist			= vmin(distFromSmallerSphere, distFromLargerSphere);

			// Puts it in the -1.0 to +1.0 range. [ 1 - (nearestDist * 4) ] => [ (-4 * nearestDist) + 1 ]
			return vfmadd(nearestDist, -cfl<vreal, 4>::val(), cfl<vreal, 1>::val());
		}
	};

	MODULE(mod_checkerboard)
	{
	public:
		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto one = cfl<vint, 1>::val();

			// Equivalent to
			// return (vint(coords.x) & 1 ^ vint(coords.y) & 1 ^ vint(coords.z) & 1) ? -1.0 : 1.0		
			// ( And repeat that for each vector field :) )
			return one - (((static_cast<vint>(coords.x) ^ static_cast<vint>(coords.y) ^ static_cast<vint>(coords.z)) & one) << 1);
		}
	};

	MODULE(mod_const)
	{
	public:
		vreal value;
		const json& operator<<(const json &source) override
		{
			value = source.value<float>("value", 0.0f);
			return source;
		}

		vreal operator()(const vec3<vreal>& coords) const override
		{
			return value;
		}
	};
}}}