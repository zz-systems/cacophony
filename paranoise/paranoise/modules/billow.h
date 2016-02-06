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
		float frequency, lacunarity, persistence;
		Quality quality;
		int seed, octaves;
		
		billow_settings(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}
	};

	SIMD_ENABLE(TReal, TInt)
	inline TReal billow(const Vector3<TReal>& coords, const billow_settings& settings)
	{
		using VReal = Vector3<TReal>;

		TReal value = 0.0;
		TReal signal = 0.0;
		TReal curPersistence = 1.0;
		VReal n;
		TInt seed;

		const VReal lacunarity = settings.lacunarity;
		const TReal persistence = settings.persistence;

		auto _coords = coords * Vector3<TReal>(settings.lacunarity);

		for (int currentOctave = 0; currentOctave < settings.octaves; currentOctave++) {

			// Make sure that these floating-point values have the same range as a 32-
			// bit integer so that we can pass them to the coherent-noise functions.
			/*n.x = truncate<TReal, TInt>(_coords.x);
			n.y = truncate<TReal, TInt>(_coords.y);
			n.z = truncate<TReal, TInt>(_coords.z);*/

			n = clamp_int32(_coords);
			// Get the coherent-noise value from the input value and add it to the
			// final result.
			seed = (settings.seed + currentOctave) & 0xffffffff;

			signal = GradientCoherentNoise3D(n, seed, settings.quality);

			signal = 2.0f * paranoise::parallel::abs(signal) - 1.0f;

			value += signal * curPersistence;

			// Prepare the next octave.
			_coords *= lacunarity;
			curPersistence *= persistence;
		}

		value += (TReal)0.5f;

		return value;
	}
}}
#endif