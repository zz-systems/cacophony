#pragma once
#ifndef PARANOISE_MODULE_MODULES_H
#define PARANOISE_MODULE_MODULES_H

#include <assert.h>
#include <map>

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
	inline TReal blend( Vector3<TReal> coords,
						Module<TReal> v0,
						Module<TReal> v1,
						Module<TReal> alpha)
	{
		return blend(v0(coords), v1(coords), alpha(coords));
	}	
		
	// Translate the source module's input
	SIMD_ENABLE_F(TReal)
	inline TReal translate(const Vector3<TReal>& coords, const Vector3<TReal>& translation, Module<TReal> source)
	{
		return source(coords + translation);
	}

	// Scale the source module's input
	SIMD_ENABLE_F(TReal)
	inline TReal scale_input(const Vector3<TReal>& coords, const Vector3<TReal>& scale, Module<TReal> source)
	{
		return source(coords * scale);
	}

	// Scale the source module's output and add a bias
	SIMD_ENABLE_F(TReal)
	inline TReal scale_output(const Vector3<TReal>& coords, const TReal& scale, Module<TReal> source)
	{
		return source(coords) * factor;
	}

	// Scale the source module's output and add a bias
	SIMD_ENABLE_F(TReal)
	inline TReal scale_output_biased(const Vector3<TReal>& coords, const TReal& scale, const TReal& bias, Module<TReal> source)
	{
		return source(coords) * factor + bias;
	}

	// generate points on concentric spheres
	SIMD_ENABLE_F(TReal)
	inline TReal spheres(const TReal& frequency, 
									const Vector3<TReal>& coords)
	{
		auto _coords = coords * frequency;

		auto distFromCenter			= sqrt(dot(_coords, _coords));
		auto distFromSmallerSphere	= distFromCenter - floor(distFromCenter);
		auto distFromLargerSphere	= 1.0 - distFromSmallerSphere;
		auto nearestDist			= min(distFromSmallerSphere, distFromLargerSphere);

		return 1.0 - (nearestDist * 4.0); // Puts it in the -1.0 to +1.0 range.
	}

	// generate points on cylinders
	SIMD_ENABLE_F(TReal)
	inline TReal cylinders(const Vector3<TReal>& coords, const TReal& frequency)
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
		return 1 - (((static_cast<TInt>(coords.x) ^ static_cast<TInt>(coords.y) ^ static_cast<TInt>(coords.z)) & 1) << 1);
	}

	SIMD_ENABLE(TReal, TInt)
	inline Module<TReal> memoize(const Module<TReal>& source)
	{
		return [source](const Vector3<TReal>& coords)
		{
			static std::map<Vector3<TReal>, TReal> cache;
			
			return cache.count(coords) > 0 
				? cache[coords]
				: cache[coords] = source(coords);
		};
	}
}}
#endif
