#pragma once
#ifndef PARANOISE_MODULE_MODULES_H
#define PARANOISE_MODULE_MODULES_H

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

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
		return source(coords) * scale;
	}

	// Scale the source module's output and add a bias
	SIMD_ENABLE_F(TReal)
	inline TReal scale_output_biased(const Vector3<TReal>& coords, const TReal& scale, const TReal& bias, Module<TReal> source)
	{
		// source(coords) * scale + bias;
		return vfmadd(source(coords), scale, bias);
	}

	// generate points on concentric spheres
	SIMD_ENABLED
	inline vreal spheres(const Vector3<vreal>& coords, const vreal& frequency)
	{
		auto _coords = coords * frequency;

		auto distFromCenter			= vsqrt(dot(_coords, _coords));
		auto distFromSmallerSphere	= distFromCenter - vfloor(distFromCenter);
		auto distFromLargerSphere	= fastload<vreal>::_1() - distFromSmallerSphere;
		auto nearestDist			= vmin(distFromSmallerSphere, distFromLargerSphere);

		// Puts it in the -1.0 to +1.0 range.
		//fastload<TReal>::_1() - (nearestDist * fastload<TReal>::_4());
		return vfmadd(nearestDist, -fastload<vreal>::_4(), fastload<vreal>::_1());
	}

	// generate points on cylinders
	SIMD_ENABLED
	inline vreal cylinders(const Vector3<vreal>& coords, const vreal& frequency)
	{		
		auto distFromCenter			= vsqrt(coords.x * coords.x + coords.z * coords.z) * frequency;
		auto distFromSmallerSphere	= distFromCenter - vfloor(distFromCenter);
		auto distFromLargerSphere	= fastload<vreal>::_1() - distFromSmallerSphere;
		auto nearestDist			= vmin(distFromSmallerSphere, distFromLargerSphere);


		// Puts it in the -1.0 to +1.0 range.
		//fastload<TReal>::_1() - (nearestDist * fastload<TReal>::_4());
		return vfmadd(nearestDist, -fastload<vreal>::_4(), fastload<vreal>::_1());
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
}}}
#endif
