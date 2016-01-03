#pragma once
#ifndef PARANOISE_MODULES_BILLOW
#define PARANOISE_MODULES_BILLOW

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	struct billow_settings
	{
		double frequency = 1.0, lacunarity = 2.0, persistence = 0.5;
		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;
	};

	SIMD_ENABLE(TReal, TInt)
	inline TReal billow(Vector3<TReal>& coords, const billow_settings& settings)
	{
		TReal value = 0.0;
		TReal signal = 0.0;
		TReal curPersistence = 1.0;
		Vector3<TReal> n;
		TInt seed;

		coords = coords * Vector3<TReal>(settings.lacunarity);

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
			coords = coords * Vector3<TReal>(settings.lacunarity);

			curPersistence = curPersistence * settings.persistence;
		}

		value = value + 0.5;

		return value;
	}
}}
#endif