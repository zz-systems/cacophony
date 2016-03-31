#pragma once
#ifndef PARANOISE_MODULES_BILLOW
#define PARANOISE_MODULES_BILLOW

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	SIMD_ENABLED
	class billow : public module_base<vreal, vint>
	{
		vreal frequency, lacunarity, persistence;
		vint seed;
				
		int octaves;
		Quality quality;

		billow(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}

		vreal operator()(const Vector3<vreal>& c) const override
		{
			vreal value = 0.0, signal = 0.0, curPersistence = 1.0, n;

			auto _coords = c * lacunarity;

			for (int currentOctave = 0; currentOctave < octaves; currentOctave++) {

				// Make sure that these floating-point values have the same range as a 32-
				// bit integer so that we can pass them to the coherent-noise functions.	

				n = clamp_int32(_coords);

				// Get the coherent-noise value from the input value and add it to the
				// final result.
				signal = GradientCoherentNoise3D(n, seed + currentOctave, quality);

				signal = vfmsub(fastload<vreal>::_2(), vabs(signal), fastload<vreal>::_1());

				//value += signal * curPersistence;
				value = vfmadd(signal, curPersistence, value);
				

				// Prepare the next octave.
				_coords *= lacunarity;
				curPersistence *= persistence;
			}

			value += 0.5f;

			return value;
		}
	};

	
}}}
#endif