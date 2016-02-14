#pragma once
#ifndef PARANOISE_MODULES_PERLIN
#define PARANOISE_MODULES_PERLIN

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;
	
	SIMD_ENABLE(TReal, TInt)
	struct perlin
	{
		TReal   frequency, 
				lacunarity, 
				persistence;

		Quality quality = Quality::Standard;
		TInt seed;
		int octaves;

		perlin(float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: frequency(frequency), lacunarity(lacunarity), persistence(persistence), seed(seed), octaves(octaves), quality(quality)
		{}


		perlin(const perlin<TReal, TInt>& rhs)
			: frequency(rhs.frequency), lacunarity(rhs.lacunarity), persistence(rhs.persistence), seed(rhs.seed), octaves(rhs.octaves), quality(rhs.quality)
		{}
				
		inline TReal operator()(const Vector3<TReal>& c) const
		{
			using VReal = Vector3<TReal>;

			TReal	value = 0,
				currentPersistence = 1;

			auto _coords = c * frequency;

			for (int curOctave = 0; curOctave < octaves; curOctave++)
			{
				value += currentPersistence * GradientCoherentNoise3D<TReal, TInt>(
					clamp_int32<TReal>(_coords),
					seed + curOctave, //& 0xffffffff,
					quality);

				// Prepare the next octave.
				_coords *= lacunarity;
				currentPersistence *= persistence;
			}

			return value;
		}

		inline operator Module<TReal>() const { return [this](const auto &c) { return this->operator()(c); }; }
		//{ return &perlin<TReal, TInt>::operator(); }
	};	
}}
#endif