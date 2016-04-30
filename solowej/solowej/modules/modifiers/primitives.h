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

	/*MODULE(pow)
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

	MODULE(add)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		add() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) + get_b()(coords);
		}
	};
	MODULE(sub)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		sub() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) - get_b()(coords);
		}
	};

	MODULE(mul)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mul() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) * get_b()(coords);
		}
	};

	MODULE(div)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		div() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) / get_b()(coords);
		}
	};

	MODULE(blend)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);
		MODULE_PROPERTY(alpha, 2);

		blend() : cloneable(3) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			//return lerp(v0, v1, (alpha + 1.0) / 2.0);
			return lerp(get_a()(coords),
						get_b()(coords), 
						(get_alpha()(coords) + cfl<vreal>::_1()) / cfl<vreal>::_2()
				);
		}
	};

	MODULE(translate_input)
	{
	public:
		vec3<vreal> offset;
		translate_input(const vec3<vreal> &offset = vec3<vreal>(0.0f)) : 
			cloneable(1), 
			offset(offset)
		{}

		translate_input(const translate_input<SIMD_T> &rhs) : 
			cloneable(rhs), offset(rhs.offset)
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

	MODULE(scale_input)
	{
	public:
		vreal scale;
		scale_input(const vreal &scale = 1.0f) : 
			cloneable(1), scale(scale)
		{}

		scale_input(const scale_input &rhs) :
			cloneable(rhs), scale(rhs.scale)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value("scale", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);
		
		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords * scale);
		}
	};

	MODULE(scale_output)
	{
	public:
		vreal scale;
		scale_output(const vreal &scale = 1.0f) : 
			cloneable(1), scale(scale)
		{}

		scale_output(const scale_output &rhs) : 
			cloneable(rhs), scale(rhs.scale)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value("scale", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) * scale;
		}
	};

	MODULE(scale_output_biased)
	{
	public:
		vreal scale, bias;
		scale_output_biased(const vreal &scale = 1.0f, const vreal &bias = 0.0f) :
			cloneable(1), scale(scale), bias(bias)
		{}

		scale_output_biased(const scale_output_biased &rhs) :
			cloneable(rhs), scale(rhs.scale), bias(rhs.bias)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value("scale", 1.0f);
			bias = source.value("bias", 0.0f);
			return source;
		}
		
		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vfmadd(get_a()(coords), scale, bias);
		}
	};


	MODULE(min)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		min() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override 
		{
			return vmin(get_a()(coords), get_b()(coords));
		}
	};

	MODULE(max)
	{
	public:
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		max() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vmax(get_a()(coords), get_b()(coords));
		}
	};

	MODULE(clamp)
	{
	public:
		vreal min, max;
		clamp(const vreal &min = -1.0f, const vreal &max = 1.0f) :
			cloneable(1), min(min), max(max)
		{}

		clamp(const clamp &rhs) :
			cloneable(rhs), min(rhs.min), max(rhs.max)
		{}

		const json& operator <<(const json &source) override
		{
			min = source.value("min", -1.0f);
			max = source.value("max", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vclamp(get_a()(coords), min, max);
		}
	};
}}}
