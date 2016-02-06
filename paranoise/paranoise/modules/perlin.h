#pragma once
#ifndef PARANOISE_MODULES_PERLIN
#define PARANOISE_MODULES_PERLIN

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;
	
	struct perlin_settings
	{
		float   frequency, 
				lacunarity, 
				persistence;

		Quality quality = Quality::Standard;
		int seed = 0;
		int octaves = 6;

		perlin_settings(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}
	};

	SIMD_ENABLE(TReal, TInt)
	inline TReal perlin(const Vector3<TReal>& coords, const perlin_settings& settings)
	{
		using VReal = Vector3<TReal>;

		TReal	value				= 0, 
				currentPersistence	= 1;

		const VReal lacunarity	= settings.lacunarity;
		const TReal persistence	= settings.persistence;
		
		auto _coords = coords * (Vector3<TReal>) settings.frequency;
		
		for (int curOctave = 0; curOctave < settings.octaves; curOctave++)
		{
			value += currentPersistence * GradientCoherentNoise3D<TReal, TInt>(
											clamp_int32<TReal>(_coords),
											(settings.seed + curOctave) & 0xffffffff,
											settings.quality);

			// Prepare the next octave.
			_coords				*= lacunarity;
			currentPersistence	*= persistence;
		}

		return value;
	}
}}
#endif