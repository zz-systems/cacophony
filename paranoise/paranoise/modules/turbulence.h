#pragma once
#ifndef PARANOISE_MODULES_TURBULENCE
#define PARANOISE_MODULES_TURBULENCE

#include "dependencies.h"
#include "perlin.h"


namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	SIMD_ENABLED
	class turbulence : perlin<vreal, vint>
	{	
	public:
		Vector3<vreal> power;

		Matrix3x3<vreal> dinput = {
			Vector3<vreal>(12414.0f, 65124.0f, 31337.0f) / Vector3<vreal>(65536.0f),
			Vector3<vreal>(26519.0f, 18128.0f, 60493.0f) / Vector3<vreal>(65536.0f),
			Vector3<vreal>(53820.0f, 11213.0f, 44845.0f) / Vector3<vreal>(65536.0f)
		};

		turbulence(float power = 1.0, int roughness = 3, float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int seed = 0, Quality quality = Quality::Standard)
			: perlin<vreal, vint>(frequency, lacunarity, persistence, seed, roughness, quality),
			module_base(1),	power(power)
		{}

		turbulence(const turbulence<vreal, vint>& rhs)
			: perlin<vreal, vint>(rhs),
			power(rhs.power)
		{}

		MODULE_PROPERTY(source, 0)

		// Apply turbulence to the source input
		vreal operator()(const Vector3<vreal> &coords) const override
		{			
			auto distortion = Vector3<vreal>(
				perlin<vreal, vint>::operator()(dinput._0 + coords),
				perlin<vreal, vint>::operator()(dinput._1 + coords),
				perlin<vreal, vint>::operator()(dinput._2 + coords)) * power;
			
			return get_source()(coords + distortion);
		}
	};
}}}
#endif