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
#include "../generators/perlin.h"

namespace zzsystems { namespace solowej { namespace modules {
	using namespace gorynych;	
	using namespace math;

	MODULE(turbulence)
	{	
	public:
		vec3<vreal> power;

		mat3x3<vreal> dinput = {
			vec3<vreal>(12414.0f, 65124.0f, 31337.0f) / vec3<vreal>(65536.0f),
			vec3<vreal>(26519.0f, 18128.0f, 60493.0f) / vec3<vreal>(65536.0f),
			vec3<vreal>(53820.0f, 11213.0f, 44845.0f) / vec3<vreal>(65536.0f)
		};

		turbulence(float power = 1.0, int roughness = 3, float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int seed = 0, Quality quality = Quality::Standard)
			:
			cloneable(1),
			power(power),
			_xdistort(frequency, lacunarity, persistence, seed, roughness, quality),
			_ydistort(frequency, lacunarity, persistence, seed, roughness, quality),
			_zdistort(frequency, lacunarity, persistence, seed, roughness, quality)
		{}

		turbulence(const turbulence<vreal, vint>& rhs)
			: cloneable(rhs),
			power(rhs.power),
			_xdistort(rhs._xdistort), _ydistort(rhs._ydistort),	_zdistort(rhs._zdistort)
		{}

		MODULE_PROPERTY(source, 0)

		const json& operator <<(const json &source) override
		{
			json tmp = source;
			tmp["octaves"] = tmp.value("roughness", 3);

			_xdistort << tmp;
			_ydistort << tmp;
			_zdistort << tmp;
			power = vec3<vreal>(source.value("power", 1.0f));

			return source;
		}

		// Apply turbulence to the source input
		vreal operator()(const vec3<vreal> &coords) const override
		{	
			auto distortion = power * vec3<vreal>(
				_xdistort(dinput._0 + coords),
				_ydistort(dinput._1 + coords),
				_zdistort(dinput._2 + coords));
			
			return get_source()(coords + distortion);
		}
	private:
		perlin<vreal, vint> _xdistort, _ydistort, _zdistort;
		// workaround to prevent VC 2015 linker crash (module obj -> module func)
		//Module<vreal> _xdistort_workaround, _ydistort_workaround, _zdistort_workaround;
	};
}}}