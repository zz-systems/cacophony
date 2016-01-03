#pragma once
#ifndef PARANOISE_MODULES_PERLIN
#define PARANOISE_MODULES_PERLIN

#include "../noisegenerators.h"

namespace paranoise { namespace module {
	using namespace paranoise::generators;

	
	struct perlin_settings
	{
		double frequency = 1.0, lacunarity = 2.0, persistence = 0.5;
		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;
	};

	SIMD_ENABLE(TReal, TInt)
	inline TReal perlin(const Vector3<TReal>& coords, const perlin_settings& settings)
	{
		TReal	value				= 0, 
				signal				= 0, 
				currentPersistence	= 0;
		Vector3<TReal> n;
		TInt seed;

		auto _coords = coords * Vector3<TReal>(settings.frequency);

		for (int curOctave = 0; curOctave < settings.octaves; curOctave++)
		{
			// Make sure that these floating-point values have the same range as a 32-
			// bit integer so that we can pass them to the coherent-noise functions.
			n.x = TInt(_coords.x);
			n.y = TInt(_coords.y);
			n.z = TInt(_coords.z);

			// Get the coherent-noise value from the input value and add it to the
			// final result.
			seed = (settings.seed + curOctave) & (int)0xffffffff;

			signal = GradientCoherentNoise3D(n, seed, settings.quality);
			value = value + signal * currentPersistence;

			// Prepare the next octave.
			_coords = _coords * Vector3<TReal>(settings.lacunarity);
			currentPersistence = currentPersistence * settings.persistence;
		}

		return value;
	}
}}
#endif