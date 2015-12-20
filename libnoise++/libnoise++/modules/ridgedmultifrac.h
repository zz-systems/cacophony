#pragma once
#ifndef NOISEPP_MODULES_RIDGED
#define NOISEPP_MODULES_RIDGED

#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	struct ridged_settings
	{
		double frequency = 1.0, lacunarity = 2.0;
		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;

		double spectralWeights[30];
		ridged_settings()
		{
			double h = 1.0;

			double frequency = 1.0;
			for (int i = 0; i < 30; i++) {
				// Compute weight for each frequency.
				spectralWeights[i] = pow(frequency, -h);
				frequency = frequency * lacunarity;
			}
		}
	};


	// Multifractal code originally written by F. Kenton "Doc Mojo" Musgrave,
	// 1998.  Modified by jas for use with libnoise.
	SIMD_ENABLE(TFloat, TInt)
	inline TFloat ridged(Vector3<TFloat>& coords, const ridged_settings& settings)
	{
		coords = coords * Vector3<TFloat>(settings.frequency);

		TFloat signal = 0.0;
		TFloat value = 0.0;
		TFloat weight = 1.0;

		// These parameters should be user-defined; they may be exposed in a
		// future version of libnoise.
		TFloat offset = 1.0;
		TFloat gain = 2.0;

		for (int currentOctave = 0; currentOctave < settings.octaves; currentOctave++) {

			// Make sure that these floating-point values have the same range as a 32-
			// bit integer so that we can pass them to the coherent-noise functions.
			Vector3<TFloat> n;
			n.x = TInt(coords.x);
			n.y = TInt(coords.y);
			n.z = TInt(coords.z);

			// Get the coherent-noise value.
			TInt seed = (settings.seed + currentOctave) & 0xffffffff;

			signal = GradientCoherentNoise3D(n, seed, settings.quality);

			// Make the ridges.
			signal = abs(signal);
			signal = offset - signal;

			// Square the signal to increase the sharpness of the ridges.
			signal = signal * signal;

			// The weighting from the previous octave is applied to the signal.
			// Larger values have higher weights, producing sharp points along the
			// ridges.
			signal = signal * weight;

			// Weight successive contributions by the previous signal.
			weight = signal * gain;
			weight = clamp<TFloat>(weight, 0.0, 1.0);

			// Add the signal to the output value.
			value = value + (signal * settings.spectralWeights[currentOctave]);

			// Go to the next octave.
			coords = coords * Vector3<TFloat>(settings.lacunarity);
		}

		return (value * 1.25) - 1.0;
	}
}}
#endif