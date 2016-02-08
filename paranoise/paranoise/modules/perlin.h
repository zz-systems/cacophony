#pragma once
#ifndef PARANOISE_MODULES_PERLIN
#define PARANOISE_MODULES_PERLIN

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;
	
	SIMD_ENABLE(TReal, TInt)
	struct perlin_settings
	{
		TReal   frequency, 
				lacunarity, 
				persistence;

		Quality quality = Quality::Standard;
		TInt seed;
		int octaves;

		perlin_settings(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}

		perlin_settings(const perlin_settings<TReal, TInt> &s)
			: frequency(s.frequency), lacunarity(s.lacunarity), persistence(s.persistence), seed(s.seed), octaves(s.octaves), quality(s.quality)
		{}
	};

	SIMD_ENABLE(TReal, TInt)
	inline TReal perlin(const Vector3<TReal>& c, const perlin_settings<TReal, TInt>& s)
	{
		using VReal = Vector3<TReal>;

		TReal	value				= 0, 
				currentPersistence	= 1;
		
		auto _coords = c * s.frequency;
		
		for (int curOctave = 0; curOctave < s.octaves; curOctave++)
		{
			value += currentPersistence * GradientCoherentNoise3D<TReal, TInt>(
											clamp_int32<TReal>(_coords),
											s.seed + curOctave, //& 0xffffffff,
											s.quality);

			// Prepare the next octave.
			_coords				*= s.lacunarity;
			currentPersistence	*= s.persistence;
		}

		return value;
	}
}}
#endif