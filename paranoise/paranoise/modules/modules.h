#pragma once
#ifndef PARANOISE_MODULE_MODULES_H
#define PARANOISE_MODULE_MODULES_H

#include <assert.h>
#include "../basetypes.h"
#include "perlin.h"
#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE_F(TReal)
	inline TReal blend(TReal v0, TReal v1, TReal alpha)
	{
		return lerp(v0, v1, (alpha + 1.0) / 2.0);
	}

	// blend input values according to blend factor alpha
	SIMD_ENABLE_F(TReal)
	inline TReal blend(	Module<TReal> v0,
						Module<TReal> v1,
						Module<TReal> alpha,
						Vector3<TReal> coords)
	{
		return blend(v0(coords), v1(coords), alpha(coords));
	}

	struct turbulence_settings : perlin_settings
	{
		float power; 
		int roughness;

		turbulence_settings(float power = 1.0, int roughness = 3, float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int octaves = 6, int seed = 0, Quality quality = Quality::Standard)
			: perlin_settings(frequency, lacunarity, persistence, seed, octaves, quality),
			power(power), roughness(roughness)
		{}
	};
		
	// Apply turbulence to the source input
	SIMD_ENABLE(TReal, TInt)
	inline TReal turbulence(Module<TReal> source,
							const Vector3<TReal> &coords, 
							const turbulence_settings &settings)
	{
		Matrix3x3<TReal> dinput = {
			Vector3<TReal>{ 12414.0f, 65124.0f, 31337.0f } / (Vector3<TReal>) 65536.0f + coords,
			Vector3<TReal>{ 26519.0f, 18128.0f, 60493.0f } / (Vector3<TReal>) 65536.0f + coords,
			Vector3<TReal>{ 53820.0f, 11213.0f, 44845.0f } / (Vector3<TReal>) 65536.0f + coords
		};

		auto distortion = Vector3<TReal>(
				perlin<TReal, TInt>(dinput._0, settings), 
				perlin<TReal, TInt>(dinput._1, settings), 
				perlin<TReal, TInt>(dinput._2, settings)) * (Vector3<TReal>)settings.power;


		return source(coords + distortion);
	}

	SIMD_ENABLE_F(TReal)
	inline TReal add(Module<TReal> one, Module<TReal> other, const Vector3<TReal> &coords)
	{		
		return one(coords) + other(coords);
	}
		
	// Translate the source module's input
	SIMD_ENABLE_F(TReal)
	inline TReal translate(Module<TReal> source, const Vector3<TReal>& translation, const Vector3<TReal>& coords)
	{
		return source(coords + translation);
	}

	// Scale the source module's input
	SIMD_ENABLE_F(TReal)
	inline TReal scale_input(Module<TReal> source, const Vector3<TReal>& scale, const Vector3<TReal>& coords)
	{
		return source(coords * scale);
	}

	// Scale the source module's output and add a bias
	SIMD_ENABLE_F(TReal)
	inline TReal scale_output_biased(	Module<TReal> source, 
										const TReal& factor, 
										const TReal& bias, 
										const Vector3<TReal>& coords)
	{
		return source(coords) * factor + bias;
	}

	// generate points on concentric spheres
	SIMD_ENABLE_F(TReal)
	inline TReal concentric_spheres(const TReal& frequency, 
									const Vector3<TReal>& coords)
	{
		auto _coords = coords * frequency;

		auto distFromCenter			= sqrt(dot(_coords, _coords));
		auto distFromSmallerSphere	= distFromCenter - floor(distFromCenter);
		auto distFromLargerSphere	= 1.0 - distFromSmallerSphere;
		auto nearestDist			= min(distFromSmallerSphere, distFromLargerSphere);

		return 1.0 - (nearestDist * 4.0); // Puts it in the -1.0 to +1.0 range.
	}

	// generate points on concentric cylinders
	SIMD_ENABLE_F(TReal)
	inline TReal concentric_cylinders(const TReal& frequency,
									  const Vector3<TReal>& coords)
	{
		auto _coords = coords;
		_coords.x *= frequency;
		_coords.z *= frequency;
		
		auto distFromCenter			= sqrt(_coords.x * _coords.x + _coords.z * _coords.z);
		auto distFromSmallerSphere	= distFromCenter - floor(distFromCenter);
		auto distFromLargerSphere	= (TReal)1.0f - distFromSmallerSphere;
		auto nearestDist			= min(distFromSmallerSphere, distFromLargerSphere);

		return 1.0f - (nearestDist * 4.0f); // Puts it in the -1.0 to +1.0 range.
	}

	SIMD_ENABLE_F(TReal)
	inline TReal pow(	const Module<TReal>& source, 
						const Module<TReal>& exponent,
						const Vector3<TReal>& coords)
	{
		return pow(	source	(coords), 
					exponent(coords));
	}

	// Create a checkerboard pattern
	SIMD_ENABLE(TReal, TInt)
	inline TReal checkerboard(const Vector3<TReal>& coords)
	{
		// Equivalent to
		// return (TInt(coords.x) & 1 ^ TInt(coords.y) & 1 ^ TInt(coords.z) & 1) ? -1.0 : 1.0
		// ( And repeat that for each vector field :) )
		return 1 - (((TInt(coords.x) ^ TInt(coords.y) ^ TInt(coords.z)) & 1) << 1);
	}
}}
#endif
