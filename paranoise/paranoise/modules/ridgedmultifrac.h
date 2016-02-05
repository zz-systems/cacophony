#pragma once
#ifndef PARANOISE_MODULES_RIDGED
#define PARANOISE_MODULES_RIDGED

#include "../noisegenerators.h"
//#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	//using namespace x87compat;

	struct ridged_settings
	{
		float frequency = 1.0, lacunarity = 2.0;
		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;

		float spectralWeights[30];
		ridged_settings()
		{
			float	h		= 1.0, 
					freq	= 1.0;

			for (int i = 0; i < 30; i++) 
			{
				// Compute weight for each frequency.
				spectralWeights[i] = std::powf(freq, -h);
				freq = freq * lacunarity;
			}
		}
	};


	// Multifractal code originally written by F. Kenton "Doc Mojo" Musgrave,
	// 1998.  Modified by jas for use with libnoise.
	SIMD_ENABLE(TReal, TInt)
	inline TReal ridged(const Vector3<TReal>& coords, const ridged_settings& settings)
	{
		auto _coords = coords * Vector3<TReal>(settings.frequency);

		TReal signal = 0.0;
		TReal value = 0.0;
		TReal weight = 1.0;

		// These parameters should be user-defined; they may be exposed in a
		// future version of libnoise.
		TReal offset = 1.0;
		TReal gain = 2.0;

		for (int currentOctave = 0; currentOctave < settings.octaves; currentOctave++) {

			// Make sure that these floating-point values have the same range as a 32-
			// bit integer so that we can pass them to the coherent-noise functions.
			Vector3<TReal> n(truncate<TReal, TInt>(_coords.x), truncate<TReal, TInt>(_coords.y), truncate<TReal, TInt>(_coords.z));

			// Get the coherent-noise value.
			TInt seed = (settings.seed + currentOctave) & 0xffffffff;

			signal = GradientCoherentNoise3D(n, seed, settings.quality);

			// Make the ridges.
			signal = paranoise::parallel::abs(signal);
			signal = offset - signal;

			// Square the signal to increase the sharpness of the ridges.
			signal = signal * signal;

			// The weighting from the previous octave is applied to the signal.
			// Larger values have higher weights, producing sharp points along the
			// ridges.
			signal = signal * weight;

			// Weight successive contributions by the previous signal.
			weight = signal * gain;
			weight = clamp<TReal>(weight, 0.0, 1.0);

			// Add the signal to the output value.
			value = value + (signal * settings.spectralWeights[currentOctave]);

			// Go to the next octave.
			_coords = _coords * Vector3<TReal>(settings.lacunarity);
		}

		return (value * 1.25f) - 1.0f;
	}
}}
#endif