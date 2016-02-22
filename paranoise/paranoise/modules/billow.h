#pragma once
#ifndef PARANOISE_MODULES_BILLOW
#define PARANOISE_MODULES_BILLOW

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE(TReal, TInt)
	struct billow
	{
		TReal frequency, lacunarity, persistence;
		Quality quality;
		TInt seed;
		int octaves;
		
		billow(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}

		inline TReal operator()(const Vector3<TReal>& c) const
		{
			using VReal = Vector3<TReal>;

			TReal value = 0.0;
			TReal signal = 0.0;
			TReal curPersistence = 1.0;
			VReal n;

			auto _coords = c * lacunarity;

			for (int currentOctave = 0; currentOctave < octaves; currentOctave++) {

				// Make sure that these floating-point values have the same range as a 32-
				// bit integer so that we can pass them to the coherent-noise functions.	

				n = clamp_int32(_coords);

				// Get the coherent-noise value from the input value and add it to the
				// final result.
				signal = GradientCoherentNoise3D(n, seed + currentOctave, quality);

				signal = vfmsub(fastload<TReal>::_2(), vabs(signal), fastload<TReal>::_1());
				value += signal * curPersistence;

				// Prepare the next octave.
				_coords *= lacunarity;
				curPersistence *= persistence;
			}

			value += 0.5f;

			return value;
		}

		// inline operator (Module<TReal>)() { return operator(); }
	};

	
}}
#endif