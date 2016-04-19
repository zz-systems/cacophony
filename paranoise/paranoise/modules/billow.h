#pragma once
#ifndef PARANOISE_MODULES_BILLOW
#define PARANOISE_MODULES_BILLOW

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED
	class billow : 
		public cloneable<module_base<SIMD_T>, billow<SIMD_T>>,
		public serializable<json>
	{
	public:
		vreal frequency, lacunarity, persistence;
		vint seed;
				
		int octaves;
		Quality quality;

		billow(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: cloneable(0), frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}

		const json& operator<<(const json &source) override
		{			
			frequency	= source.value("frequency", 1.0f);
			lacunarity	= source.value("lacunarity", 2.0f);
			persistence = source.value("persistence", 0.5f);

			seed		= source.value("seed", 0);
			octaves		= source.value("octaves", 6);
			quality		= static_cast<Quality>(source.value("quality", static_cast<int>(Quality::Standard)));

			return source;
		}

		vreal operator()(const vec3<vreal>& c) const override
		{
			vreal value = 0.0, signal = 0.0, curPersistence = 1.0;
			vec3<vreal> n;

			auto _coords = c * lacunarity;

			for (int currentOctave = 0; currentOctave < octaves; currentOctave++) {

				// Make sure that these floating-point values have the same range as a 32-
				// bit integer so that we can pass them to the coherent-noise functions.	

				n = clamp_int32<vreal>(_coords);

				// Get the coherent-noise value from the input value and add it to the
				// final result.
				signal = GradientCoherentNoise3D(n, seed + currentOctave, quality);

				signal = vfmsub(fastload<vreal>::_2(), vabs(signal), fastload<vreal>::_1());

				//value += signal * curPersistence;
				value = vfmadd(signal, curPersistence, value);
				

				// Prepare the next octave.
				_coords *= lacunarity;
				curPersistence *= persistence;
			}

			value = value + 0.5f;

			return value;
		}
	};

	
}}}
#endif