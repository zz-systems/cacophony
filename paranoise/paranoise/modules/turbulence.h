#pragma once
#ifndef PARANOISE_MODULES_TURBULENCE
#define PARANOISE_MODULES_TURBULENCE

#include "dependencies.h"
#include "perlin.h"


namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED class turbulence : 
		public cloneable<module_base<SIMD_T>, turbulence<SIMD_T>>,
		serializable<json>
	{	
	public:
		vec3<vreal> power;

		mat3x3<vreal> dinput = {
			vec3<vreal>(12414.0f, 65124.0f, 31337.0f) / vec3<vreal>(65536.0f),
			vec3<vreal>(26519.0f, 18128.0f, 60493.0f) / vec3<vreal>(65536.0f),
			vec3<vreal>(53820.0f, 11213.0f, 44845.0f) / vec3<vreal>(65536.0f)
		};

		turbulence(float power = 1.0, int roughness = 3, float frequency = 1.0, float lacunarity = 2.0, float persistence = 0.5, int seed = 0, Quality quality = Quality::Standard)
			:
			cloneable(1),
			power(power),
			_xdistort(frequency, lacunarity, persistence, seed, roughness, quality),
			_ydistort(frequency, lacunarity, persistence, seed, roughness, quality),
			_zdistort(frequency, lacunarity, persistence, seed, roughness, quality)
		{}

		turbulence(const turbulence<vreal, vint>& rhs)
			: cloneable(rhs),
			power(rhs.power),
			_xdistort(rhs._xdistort), _ydistort(rhs._ydistort),	_zdistort(rhs._zdistort)
		{}

		MODULE_PROPERTY(source, 0)

		const json& operator <<(const json &source) override
		{
			json tmp = source;
			tmp["octaves"] = tmp.value("roughness", 3);

			_xdistort << tmp;
			_ydistort << tmp;
			_zdistort << tmp;
			power = vec3<vreal>(source.value("power", 1.0f));

			return source;
		}

		// Apply turbulence to the source input
		vreal operator()(const vec3<vreal> &coords) const override
		{	
			auto distortion = power * vec3<vreal>(
				_xdistort(dinput._0 + coords),
				_ydistort(dinput._1 + coords),
				_zdistort(dinput._2 + coords));
			
			return get_source()(coords + distortion);
		}
	private:
		perlin<vreal, vint> _xdistort, _ydistort, _zdistort;
		// workaround to prevent VC 2015 linker crash (module obj -> module func)
		//Module<vreal> _xdistort_workaround, _ydistort_workaround, _zdistort_workaround;
	};
}}}
#endif