#pragma once
#ifndef PARANOISE_MODULES_TURBULENCE
#define PARANOISE_MODULES_TURBULENCE

#include "../noisegenerators.h"
#include "perlin.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE(TReal, TInt)
	struct turbulence : perlin<TReal, TInt>
	{	
		Vector3<TReal> power;

		Matrix3x3<TReal> dinput = {
			Vector3<TReal>(12414.0f, 65124.0f, 31337.0f) / Vector3<TReal>(65536.0f),
			Vector3<TReal>(26519.0f, 18128.0f, 60493.0f) / Vector3<TReal>(65536.0f),
			Vector3<TReal>(53820.0f, 11213.0f, 44845.0f) / Vector3<TReal>(65536.0f)
		};

		turbulence(float power = 1.0, int roughness = 3, float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int seed = 0, Quality quality = Quality::Standard)
			: perlin<TReal, TInt>(frequency, lacunarity, persistence, seed, roughness, quality),
			power(power)
		{}

		turbulence(const turbulence<TReal, TInt>& rhs)
			: perlin<TReal, TInt>(rhs),
			power(rhs.power)
		{}

		// Apply turbulence to the source input
		inline TReal operator()(const Vector3<TReal> &c, const Module<TReal> &source) const
		{
			auto distortion = Vector3<TReal>(
				perlin<TReal, TInt>::operator()(dinput._0 + c),
				perlin<TReal, TInt>::operator()(dinput._1 + c),
				perlin<TReal, TInt>::operator()(dinput._2 + c)) * power;


			return source(c + distortion);
		}

		// inline operator (Module<TReal>)() { return operator(); }
	};
}}
#endif