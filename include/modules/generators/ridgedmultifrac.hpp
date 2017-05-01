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

namespace cacophony {	namespace modules {
	using namespace zacc;
	using namespace math;

	// Multifractal code originally written by F. Kenton "Doc Mojo" Musgrave,
	// 1998.  Modified by jas for use with libnoise.
	// Modified by Sergej Zuyev for use with cacophony, zacc

	MODULE(mod_ridged_multifractal)
	{
	public:
		zfloat frequency, lacunarity;
		Quality quality = Quality::Standard;
		int seed;
		int octaves;

		zfloat spectralWeights[30];

		void init_spectral_weights(float lacunarity)
		{
			float h = 1.0, freq = 1.0;

			for (int i = 0; i < 30; i++)
			{
				// Compute weight for each frequency.
				spectralWeights[i] = pow(freq, -h);
				freq *= lacunarity;
			}
		}

		mod_ridged_multifractal(float frequency = 1.0, float lacunarity = 2.0, int seed = 0, int octaves = 6)
			: BASE(mod_ridged_multifractal)::cloneable(0), frequency(frequency), lacunarity(lacunarity), seed(seed), octaves(octaves)
		{
			init_spectral_weights(lacunarity);
		}

		void deserialize(const json &source) override
		{
			float sl;
			frequency = source.value<float>("frequency", 1.0f);
			lacunarity = sl = source.value<float>("lacunarity", 2.0f);
			seed = source.value<int>("seed", 0);
			octaves = source.value<int>("octaves", 6);
			quality  = static_cast<Quality>(source.value<int>("quality", static_cast<int>(Quality::Standard)));

			init_spectral_weights(sl);

		}

		zfloat operator()(const vec3<zfloat>& coords) const override
		{
			auto _coords = coords * frequency;

			// Workaround. SSE/AVX seed member gets modified => invalid for next iterations
			zint _seed = seed;

			zfloat
				signal = 0.0,
				value = 0.0,
				weight = 1.0,

				// These parameters should be user-defined; they may be exposed in a
				// future version of libnoise.
				offset = 1.0,
				gain = 2.0;

			for (int currentOctave = 0; currentOctave < octaves; currentOctave++) {

				// Get the coherent-noise value.
				signal = noisegen<branch>::gradient_coherent_3d(
					clamp_int32<zfloat>(_coords),
					(_seed + currentOctave) & 0xFFFF'FFFF,
					quality);

				// Make the ridges.
				signal = offset - vabs(signal);

				// Square the signal to increase the sharpness of the ridges.
				signal *= signal;

				// The weighting from the previous octave is applied to the signal.
				// Larger values have higher weights, producing sharp points along the
				// ridges.
				signal *= weight;

				// Weight successive contributions by the previous signal. 
				weight = vclamp(signal * gain, 0, 1);

				// Add the signal to the output value. [value += (signal * spectralWeights[currentOctave]);]
				value = vfmadd(signal, spectralWeights[currentOctave], value);
				// Go to the next octave.
				_coords *= lacunarity;
			}

			//return (value * 1.25f) - 1.0f;
			return vfmsub(value, static_cast<zfloat>(1.25f), 1);
		}
	};
}}