#pragma once
#ifndef NOISEPP_MODULE_MODULES_H
#define NOISEPP_MODULE_MODULES_H

#include <assert.h>
#include "../basetypes.h"
#include "../intrinsic.h"
#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	SIMD_ENABLE_F(TReal)
	inline TReal blend(TReal v0, TReal v1, TReal alpha)
	{
		return InterpolateLinear(v0, v1, (alpha + 1.0) / 2.0);
	}

	SIMD_ENABLE_F(TReal)
	inline TReal blend(	Module<TReal> v0,
						Module<TReal> v1,
						Module<TReal> alpha,
						Vector3<TReal> coords)
	{
		return blend(v0(coords), v1(coords), alpha(coords));
	}

	// Apply turbulence to the source input
	SIMD_ENABLE_F(TReal)
	inline TReal turbulence(Module<TReal>			source,
							Vector3<Module<TReal>> distorters,
							Vector3<TReal> coords)
	{
		auto dinput = Matrix3x3<TReal>(
							{ 12414.0, 65124.0, 31337.0 },
							{ 26519.0, 18128.0, 60493.0 },
							{ 53820.0, 11213.0, 44845.0 }
						) / 65536.0 + coords,

			distortion = Vector3<TReal>(distorters.x(dinput.x), distorters.y(dinput.y), distorters.z(dinput.z)) * power;


		return source(coords + distortion);
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
										const Vector3<TReal>& factor, 
										const Vector3<TReal>& bias, 
										const Vector3<TReal>& coords)
	{
		return source(coords) * scale + bias;
	}

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
