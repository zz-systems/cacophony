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

	MODULE(mod_rotate)
	{
	public:
		mat3x3<vreal> rot;
		vec3<float>  angles;

		mod_rotate() : mod_rotate({ 0.0f, 0.0f, 0.0f }) {}

		mod_rotate(const vec3<float> &angles) :
			BASE(mod_rotate)::cloneable(1),
			angles(angles)
		{
			build_rot_matrix(angles);
		}

		mod_rotate(const mod_rotate<vreal, vint> &rhs) :
			BASE(mod_rotate)::cloneable(rhs),
			rot(rhs.rot), 
			angles(rhs.angles)
		{}

		const json& operator <<(const json &source) override
		{
			if (source["angles"] != nullptr && source["angles"].is_object())
			{
				angles.x = source.value<float>("x", 0.0f);
				angles.y = source.value<float>("y", 0.0f);
				angles.z = source.value<float>("z", 0.0f);
			}

			build_rot_matrix(angles);
			return source;
		}

		MODULE_PROPERTY(source, 0)

		vreal operator()(const vec3<vreal>& coords) const override
		{
			return get_source()(rot * coords);
		}
	private:
		void build_rot_matrix(const vec3<float>& angles)
		{
			auto cos = vec3<vreal>(::cos(angles.x), ::cos(angles.y), ::cos(angles.z));
			auto sin = vec3<vreal>(::sin(angles.x), ::sin(angles.y), ::sin(angles.z));

			rot =
					{
							{ sin.x * sin.y * sin.z + cos.y * cos.z,	cos.x * sin.z,		sin.y * cos.z - cos.y * sin.x * sin.z },
							{ sin.y * sin.x * cos.z - cos.y * sin.z,	cos.x * cos.z,		-cos.y * sin.x * cos.z - sin.y * sin.z },
							{ -sin.y * cos.x,							sin.x,				cos.y * cos.x }
					};
		}
	};
}}