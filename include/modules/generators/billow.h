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

namespace zzsystems { namespace solowej { namespace modules {
	using namespace gorynych;	
	using namespace math;

	MODULE(mod_billow)
	{
	public:
		vreal frequency, lacunarity, persistence;
		vint seed;
				
		int octaves;
		Quality quality;

		mod_billow(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: BASE(mod_billow)::cloneable(0), frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}

		const json& operator<<(const json &source) override
		{			
			frequency	= source.value<float>("frequency", 1.0f);
			lacunarity	= source.value<float>("lacunarity", 2.0f);
			persistence = source.value<float>("persistence", 0.5f);

			seed		= source.value<int>("seed", 0);
			octaves		= source.value<int>("octaves", 6);
			quality		= static_cast<Quality>(source.value<int>("quality", static_cast<int>(Quality::Standard)));

			return source;
		}

		vreal operator()(const vec3<vreal>& c) const override
		{
			vreal value = 0.0, signal = 0.0, curPersistence = 1.0;
			vec3<vreal> n;

			auto _coords = c * lacunarity;

			for (int currentOctave = 0; currentOctave < octaves; currentOctave++) {

				// Make sure that these floating-point values have the same range as a 32-
				// bit integer so that we can pass them to the coherent-noise functions.	

				n = clamp_int32<vreal>(_coords);

				// Get the coherent-noise value from the input value and add it to the
				// final result.
				signal = noisegen<SIMD_T>::gradient_coherent_3d(n, seed + currentOctave, quality);

				signal = vfmsub(cfl<vreal, 2>::val(), vabs(signal), cfl<vreal, 1>::val());

				//value += signal * curPersistence;
				value = vfmadd(signal, curPersistence, value);
				

				// Prepare the next octave.
				_coords *= lacunarity;
				curPersistence *= persistence;
			}

			value = value + 0.5f;

			return value;
		}
	};

	
}}}