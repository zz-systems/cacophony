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

#include "../../gorynych/gorynych/gorynych.h"
#include "../modules/module_base.h"

namespace zzsystems { namespace solowej { namespace models {

	VECTORIZED_F
	struct line_settings
	{
		vec3<vreal> from, to;
		bool attenuate = true;
	};

	VECTORIZED_F
	inline vreal line(const Module<vreal>& source, const line_settings<vreal>& settings, const vreal& pos)
	{
		auto coords = (settings.to - settings.from) * pos + settings.from;
		auto result = source(coords);

		return settings.attenuate
			? pos * (1 - pos) * 4 * result
			: result;
	}

	VECTORIZED_F
	inline vreal plane(const Module<vreal>& source, const vec3<vreal>& pos)
	{
		return source(vreal(pos.x, 0, pos.y))
	}

	VECTORIZED_F
		inline vreal cylinder(const Module<vreal>& source, const vreal& angle, const vreal& height)
	{
		auto _angle = angle * DEG_TO_RAD;
		vec3<vreal> coords;
		coords.y = height;

		for (int i = 0; i < dim(angle); i++)
		{
			coords.x.values[i] = cos(angle.values[i]);
			coords.z.values[i] = sin(angle.values[i]);
		}

		return source(coords);
	}


	VECTORIZED_F
	inline vreal sphere(const Module<vreal>& source, const vreal& lat, const vreal& lon)
	{
		vreal r, _lat = lat * DEG2RAD, _lon = lon * DEG2RAD;
		vec3<vreal> pos;

		for (int i = 0; i < dim(lat); i++)
		{
			r.values[i] = cos(lat.values[i]);

			pos.x.values[i] = cos(lon.values[i]);
			pos.y.values[i] = sin(lat.values[i]);
			pos.z.values[i] = sin(lon.values[i]);
		}

		pos.x = r * pos.x;
		pos.z = r * pos.z;

		return source(pos);
	}
}}
