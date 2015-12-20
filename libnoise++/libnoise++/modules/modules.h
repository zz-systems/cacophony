#pragma once
#ifndef NOISEPP_MODULE_MODULES_H
#define NOISEPP_MODULE_MODULES_H

#include <assert.h>
#include "../basetypes.h"
#include "../intrinsic.h"
#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	SIMD_ENABLE_F(TFloat)
	inline FLOAT blend(TFloat v0, TFloat v1, TFloat alpha)
	{
		return InterpolateLinear(v0, v1, (alpha + 1.0) / 2.0);
	}

	SIMD_ENABLE_F(TFloat)
	inline TFloat blend(Module<TFloat> v0,
						Module<TFloat> v1,
						Module<TFloat>alpha,
						Vector3<TFloat> c)
	{
		return InterpolateLinear(v0(c), v1(c), (alpha(c) + 1.0) / 2.0);
	}


	SIMD_ENABLE_F(TFloat)
	inline TFloat turbulence(Module<TFloat>			source,
							Vector3<Module<TFloat>> distorters,
							Vector3<TFloat> coords)
	{
		auto dinput = Matrix3x3<TFloat>(
							{ 12414.0, 65124.0, 31337.0 },
							{ 26519.0, 18128.0, 60493.0 },
							{ 53820.0, 11213.0, 44845.0 }
						) / 65536.0 + coords,

			distortion = Vector3<TFloat>(distorters.x(dinput.x), distorters.y(dinput.y), distorters.z(dinput.z)) * power;


		return source(coords + distortion);
	}
		
	SIMD_ENABLE_F(TFloat)
	inline TFloat translate(Module<TFloat> source, const Vector3<TFloat>& translation, const Vector3<TFloat>& coords)
	{
		return source(coords + translation);
	}

	SIMD_ENABLE_F(TFloat)
	inline TFloat scale_coords(Module<TFloat> source, const Vector3<TFloat>& scale, const Vector3<TFloat>& coords)
	{
		return source(coords * scale);
	}

	SIMD_ENABLE_F(TFloat)
	inline TFloat scale_biased(	Module<TFloat> source, 
								const Vector3<TFloat>& factor, 
								const Vector3<TFloat>& bias, 
								const Vector3<TFloat>& coords)
	{
		return source(coords) * scale + bias;
	}
}}
#endif
