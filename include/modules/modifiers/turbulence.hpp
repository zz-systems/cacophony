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
#include "modules/generators/perlin.hpp"

namespace cacophony { namespace modules {
	using namespace zacc;
	using namespace math;

	MODULE(mod_turbulence)
	{	
	public:
		vec3<zfloat> power;

		mat3x3<zfloat> dinput = {
			vec3<zfloat>(12414.0f, 65124.0f, 31337.0f) / vec3<zfloat>(65536.0f),
			vec3<zfloat>(26519.0f, 18128.0f, 60493.0f) / vec3<zfloat>(65536.0f),
			vec3<zfloat>(53820.0f, 11213.0f, 44845.0f) / vec3<zfloat>(65536.0f)
		};

		mod_turbulence(float power = 1.0, int roughness = 3, float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int seed = 0, Quality quality = Quality::Standard) :
			BASE(mod_turbulence)::cloneable(1),
			power(power),
			_xdistort(frequency, lacunarity, persistence, seed, roughness, quality),
			_ydistort(frequency, lacunarity, persistence, seed, roughness, quality),
			_zdistort(frequency, lacunarity, persistence, seed, roughness, quality)
		{}

		mod_turbulence(const mod_turbulence<branch>& rhs)
			: BASE(mod_turbulence)::cloneable(rhs),
			power(rhs.power),
			_xdistort(rhs._xdistort), _ydistort(rhs._ydistort),	_zdistort(rhs._zdistort)
		{}

		MODULE_PROPERTY(source, 0)

		void deserialize(const json &source) override
		{
			json tmp = source;
			tmp["octaves"] = tmp.value<int>("roughness", 3);

			_xdistort << tmp;
			_ydistort << tmp;
			_zdistort << tmp;
			power = vec3<zfloat>(source.value<float>("power", 1.0f));

		}

		// Apply turbulence to the source input
		zfloat operator()(const vec3<zfloat> &coords) const override
		{	
			auto distortion = power * vec3<zfloat>(
				_xdistort(dinput.get_row(0) + coords),
				_ydistort(dinput.get_row(1) + coords),
				_zdistort(dinput.get_row(2) + coords));
			
			return get_source()(coords + distortion);
		}
	private:
		mod_perlin<branch> _xdistort, _ydistort, _zdistort;
		// workaround to prevent VC 2015 linker crash (module obj -> module func)
		//Module<branch> _xdistort_workaround, _ydistort_workaround, _zdistort_workaround;
	};
}}