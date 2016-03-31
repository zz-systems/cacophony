#pragma once
#ifndef PARANOISE_MODULES_PERLIN
#define PARANOISE_MODULES_PERLIN

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	SIMD_ENABLED
	class perlin : public module_base<vreal, vint>
	{
	public:
		vreal   frequency, 
				lacunarity, 
				persistence;

		int octaves;
		vint seed;

		Quality quality = Quality::Standard;
		

		perlin(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}


		perlin(const perlin<vreal, vint>& rhs)
			: frequency(rhs.frequency), lacunarity(rhs.lacunarity), persistence(rhs.persistence), seed(rhs.seed), octaves(rhs.octaves), quality(rhs.quality)
		{}
				
		vreal operator()(const Vector3<vreal>& c) const override
		{
			vreal	value = fastload<vreal>::_0(),
					currentPersistence = fastload<vreal>::_1();

			auto _coords = c * frequency;

			for (int curOctave = 0; curOctave < octaves; curOctave++)
			{
				value += currentPersistence * GradientCoherentNoise3D<vreal, vint>(
					clamp_int32<vreal>(_coords),
					seed + curOctave, //& 0xffffffff,
					quality);

				// Prepare the next octave.
				_coords *= lacunarity;
				currentPersistence *= persistence;
			}

			return value;
		}
	};	
}}}
#endif