#pragma once
#ifndef PARANOISE_MODULES_RIDGED
#define PARANOISE_MODULES_RIDGED

#include "dependencies.h"

namespace zzsystems { namespace paranoise {	namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	// Multifractal code originally written by F. Kenton "Doc Mojo" Musgrave,
	// 1998.  Modified by jas for use with libnoise.
	// Modified by Sergej Zuyev for use with paranoise, simdal

	SIMD_ENABLED
	class ridged_multifractal : public module_base<vreal, vint>
	{
	public:
		vreal frequency, lacunarity;
		Quality quality = Quality::Standard;
		int seed;
		int octaves;

		vreal spectralWeights[30];
		ridged_multifractal(float frequency = 1.0, float lacunarity = 2.0, int seed = 0, int octaves = 6)
			: frequency(frequency), lacunarity(lacunarity), seed(seed), octaves(octaves)
		{
			float h = 1.0, freq = 1.0;

			for (int i = 0; i < 30; i++)
			{
				// Compute weight for each frequency.
				spectralWeights[i] = powf(freq, -h);
				freq *= lacunarity;
			}
		}

		vreal operator()(const Vector3<vreal>& coords) const override
		{
			auto _coords = coords * frequency;

			// Workaround. SSE/AVX seed member gets modified => invalid for next iterations
			vint _seed = seed;

			vreal
				signal = 0.0,
				value = 0.0,
				weight = 1.0,

				// These parameters should be user-defined; they may be exposed in a
				// future version of libnoise.
				offset = 1.0,
				gain = 2.0;

			for (int currentOctave = 0; currentOctave < octaves; currentOctave++) {

				// Get the coherent-noise value.
				signal = GradientCoherentNoise3D<vreal, vint>(
					clamp_int32<vreal>(_coords),
					(_seed + currentOctave) & fastload<vint>::ones(),
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
				weight = vclamp(signal * gain, fastload<vreal>::_0(), fastload<vreal>::_1());

				// Add the signal to the output value. [value += (signal * spectralWeights[currentOctave]);]
				value = vfmadd(signal, spectralWeights[currentOctave], value);
				// Go to the next octave.
				_coords *= Vector3<vreal>(lacunarity);
			}

			//return (value * 1.25f) - 1.0f;
			return vfmsub(value, static_cast<vreal>(1.25f), fastload<vreal>::_1());
		}
	};
}}}
#endif