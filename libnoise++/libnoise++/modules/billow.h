#pragma once
#ifndef NOISEPP_MODULES_BILLOW
#define NOISEPP_MODULES_BILLOW

#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	struct billow_settings
	{
		double frequency = 1.0, lacunarity = 2.0, persistence = 0.5;
		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;
	};

	SIMD_ENABLE(TFloat, TInt)
	inline TFloat billow(Vector3<TFloat>& coords, const billow_settings& settings)
	{
		TFloat value = 0.0;
		TFloat signal = 0.0;
		TFloat curPersistence = 1.0;
		Vector3<TFloat> n;
		TInt seed;

		coords = coords * Vector3<TFloat>(settings.lacunarity);

		for (int currentOctave = 0; currentOctave < settings.octaves; currentOctave++) {

			// Make sure that these floating-point values have the same range as a 32-
			// bit integer so that we can pass them to the coherent-noise functions.
			n.x = TInt(coords.x);
			n.y = TInt(coords.y);
			n.z = TInt(coords.z);

			// Get the coherent-noise value from the input value and add it to the
			// final result.
			seed = (settings.seed + currentOctave) & 0xffffffff;

			signal = GradientCoherentNoise3D(n, seed, settings.quality);

			signal = 2.0 * abs(signal) - 1.0;

			value = value + signal * curPersistence;

			// Prepare the next octave.
			coords = coords * Vector3<TFloat>(settings.lacunarity);

			curPersistence = curPersistence * settings.persistence;
		}

		value = value + 0.5;

		return value;
	}
}}
#endif