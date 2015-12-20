#pragma once
#ifndef NOISEPP_MODULES_PERLIN
#define NOISEPP_MODULES_PERLIN

#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	
	struct perlin_settings
	{
		double frequency = 1.0, lacunarity = 2.0, persistence = 0.5;
		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;
	};

	SIMD_ENABLE(TFloat, TInt)
	inline TFloat perlin(Vector3<TFloat>& coords, const perlin_settings& settings)
	{
		TFloat	value				= 0, 
				signal				= 0, 
				currentPersistence	= 0;
		Vector3<TFloat> n;
		TInt seed;

		coords = coords * Vector3<TFloat>(settings.frequency);

		for (int curOctave = 0; curOctave < settings.octaves; curOctave++)
		{
			// Make sure that these floating-point values have the same range as a 32-
			// bit integer so that we can pass them to the coherent-noise functions.
			n.x = TInt(coords.x);
			n.y = TInt(coords.y);
			n.z = TInt(coords.z);

			// Get the coherent-noise value from the input value and add it to the
			// final result.
			seed = (settings.seed + curOctave) & 0xffffffff;

			signal = GradientCoherentNoise3D(n, seed, settings.quality);
			value = value + signal * currentPersistence;

			// Prepare the next octave.
			coords = coords * Vector3<TFloat>(settings.lacunarity);
			currentPersistence = currentPersistence * settings.persistence;
		}

		return value;
	}
}}
#endif