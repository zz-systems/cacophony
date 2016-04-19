#pragma once
#ifndef PARANOISE_MODULES_PERLIN
#define PARANOISE_MODULES_PERLIN

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED
	class perlin : 
		public cloneable<module_base<SIMD_T>, perlin<SIMD_T>>,
		public serializable<json>
	{
	public:
		vreal   frequency, 
				lacunarity, 
				persistence;

		int octaves;
		vint seed;

		Quality quality;
		

		perlin(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: cloneable(0), frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}


		perlin(const perlin<vreal, vint>& rhs)
			: frequency(rhs.frequency), lacunarity(rhs.lacunarity), persistence(rhs.persistence), seed(rhs.seed), octaves(rhs.octaves), quality(rhs.quality)
		{}
				
		const json& operator<<(const json &source) override
		{
			frequency = source.value("frequency", 1.0f);
			lacunarity = source.value("lacunarity", 2.0f);
			persistence = source.value("persistence", 0.5f);

			seed = source.value("seed", 0);
			octaves = source.value("octaves", 6);
			quality  = static_cast<Quality>(source.value("quality", static_cast<int>(Quality::Standard)));

			return source;
		}

		vreal operator()(const vec3<vreal>& c) const override
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