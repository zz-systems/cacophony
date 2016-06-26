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

namespace zzsystems { namespace solowej {	namespace modules {
	using namespace gorynych;	
	using namespace math;

	/*MODULE(mod_pow)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		pow() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vpow(get_a()(coords) , get_b()(coords));
		}
	};*/

	MODULE(mod_add)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mod_add() : BASE(mod_add)::cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) + get_b()(coords);
		}
	};

	MODULE(mod_sub)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mod_sub() : BASE(mod_sub)::cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) - get_b()(coords);
		}
	};

	MODULE(mod_mul)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mod_mul() : BASE(mod_mul)::cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) * get_b()(coords);
		}
	};

	MODULE(mod_div)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mod_div() : BASE(mod_div)::cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) / get_b()(coords);
		}
	};

	MODULE(mod_blend)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);
		MODULE_PROPERTY(alpha, 2);

		mod_blend() : BASE(mod_blend)::cloneable(3) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			//return lerp(v0, v1, (alpha + 1.0) / 2.0);
			return lerp(get_a()(coords),
						get_b()(coords), 
						(get_alpha()(coords) + cfl<vreal, 1>::val()) / cfl<vreal, 2>::val()
				);
		}
	};

	MODULE(mod_translate_input)
	{
	public:
		vec3<vreal> offset;
		mod_translate_input(const vec3<vreal> &offset = vec3<vreal>(0.0f)) :
			BASE(mod_translate_input)::cloneable(1),
			offset(offset)
		{}

		mod_translate_input(const mod_translate_input<SIMD_T> &rhs) :
			BASE(mod_translate_input)::cloneable(rhs), offset(rhs.offset)
		{}
		
		const json& operator <<(const json &source) override
		{
			auto so = source["offset"];
// TODO: add "value" default read
			offset = vec3<vreal>(so[0].get<float>(), so[1].get<float>(), so[2].get<float>());

			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords + offset);
		}
	};

	MODULE(mod_scale_input)
	{
	public:
		vec3<vreal> scale;
		mod_scale_input(const vec3<vreal> &scale = vec3<vreal>(1.0f)) :
			BASE(mod_scale_input)::cloneable(1), scale(scale)
		{}

		mod_scale_input(const mod_scale_input &rhs) :
			BASE(mod_scale_input)::cloneable(rhs), scale(rhs.scale)
		{}

		const json& operator <<(const json &source) override
		{
			auto so = source["scale"];
// TODO: add "value" default read
			scale = vec3<vreal>(so[0].get<float>(), so[1].get<float>(), so[2].get<float>());
			return source;
		}

		MODULE_PROPERTY(a, 0);
		
		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords * scale);
		}
	};

	MODULE(mod_scale_output)
	{
	public:
		vreal scale;
		mod_scale_output(const vreal &scale = 1.0f) :
			BASE(mod_scale_output)::cloneable(1), scale(scale)
		{}

		mod_scale_output(const mod_scale_output &rhs) :
			BASE(mod_scale_output)::cloneable(rhs), scale(rhs.scale)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value<float>("scale", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) * scale;
		}
	};

	MODULE(mod_scale_output_biased)
	{
	public:
		vreal scale, bias;
		mod_scale_output_biased(const vreal &scale = 1.0f, const vreal &bias = 0.0f) :
			BASE(mod_scale_output_biased)::cloneable(1), scale(scale), bias(bias)
		{}

		mod_scale_output_biased(const mod_scale_output_biased &rhs) :
			BASE(mod_scale_output_biased)::cloneable(rhs), scale(rhs.scale), bias(rhs.bias)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value<float>("scale", 1.0f);
			bias = source.value<float>("bias", 0.0f);
			return source;
		}
		
		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vfmadd(get_a()(coords), scale, bias);
		}
	};

	MODULE(mod_abs)
	{
	public:
		MODULE_PROPERTY(a, 0);

		mod_abs() : BASE(mod_abs)::cloneable(1) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vabs(get_a()(coords));
		}
	};

	MODULE(mod_min)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mod_min() : BASE(mod_min)::cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override 
		{
			return vmin(get_a()(coords), get_b()(coords));
		}
	};

	MODULE(mod_max)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mod_max() : BASE(mod_max)::cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vmax(get_a()(coords), get_b()(coords));
		}
	};

	MODULE(mod_clamp)
	{
	public:
		vreal min, max;
		mod_clamp(const vreal &min = -1.0f, const vreal &max = 1.0f) :
			BASE(mod_clamp)::cloneable(1), min(min), max(max)
		{}

		mod_clamp(const mod_clamp &rhs) :
			BASE(mod_clamp)::cloneable(rhs), min(rhs.min), max(rhs.max)
		{}

		const json& operator <<(const json &source) override
		{
			min = source.value<float>("min", -1.0f);
			max = source.value<float>("max", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vclamp(get_a()(coords), min, max);
		}
	};
}}}
