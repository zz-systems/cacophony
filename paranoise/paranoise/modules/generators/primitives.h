#pragma once
#ifndef ZZSYSTEMS_PARANOISE_MODULES_GENERATORS_H
#define ZZSYSTEMS_PARANOISE_MODULES_GENERATORS_H

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules { namespace generators
{
	using namespace math;
	using namespace constants;

	SIMD_ENABLED 
	struct spheres : 
		public cloneable<module_base<SIMD_T>, spheres<SIMD_T>>,
		public serializable<json>
	{
		vreal frequency;
		spheres(const vreal &frequency = 1.0f)
			: cloneable(0), frequency(frequency)
		{}

		const json& operator<<(const json &source) override
		{
			frequency = source.value("frequency", 1.0f);
			return source;
		}

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto _coords = coords * frequency;

			auto distFromCenter			= vsqrt(dot(_coords, _coords));
			auto distFromSmallerSphere	= distFromCenter - vfloor(distFromCenter);
			auto distFromLargerSphere	= fastload<vreal>::_1() - distFromSmallerSphere;

			auto nearestDist			= vmin(distFromSmallerSphere, distFromLargerSphere);

			// Puts it in the -1.0 to +1.0 range. [ 1 - (nearestDist * 4) ] => [ (-4 * nearestDist) + 1 ]
			return vfmadd(nearestDist, -fastload<vreal>::_4(), fastload<vreal>::_1());
		}
	};

	SIMD_ENABLED
	struct cylinders :
		public cloneable<module_base<SIMD_T>, cylinders<SIMD_T>>,
		public serializable<json>
	{
		vreal frequency;
		cylinders(const vreal &frequency = 1.0f)
			: cloneable(0), frequency(frequency)
		{}

		const json& operator<<(const json &source) override
		{
			frequency = source.value("frequency", 1.0f);
			return source;
		}

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto distFromCenter			= vsqrt(coords.x * coords.x + coords.z * coords.z) * frequency;
			auto distFromSmallerSphere	= distFromCenter - vfloor(distFromCenter);
			auto distFromLargerSphere	= fastload<vreal>::_1() - distFromSmallerSphere;

			auto nearestDist			= vmin(distFromSmallerSphere, distFromLargerSphere);

			// Puts it in the -1.0 to +1.0 range. [ 1 - (nearestDist * 4) ] => [ (-4 * nearestDist) + 1 ]
			return vfmadd(nearestDist, -fastload<vreal>::_4(), fastload<vreal>::_1());
		}
	};

	SIMD_ENABLED
	struct checkerboard : module_base<vreal, vint>
	{
		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto one = fastload<vint>::_();

			// Equivalent to
			// return (vint(coords.x) & 1 ^ vint(coords.y) & 1 ^ vint(coords.z) & 1) ? -1.0 : 1.0		
			// ( And repeat that for each vector field :) )
			return one - (((static_cast<vint>(coords.x) ^ static_cast<vint>(coords.y) ^ static_cast<vint>(coords.z)) & one) << one);
		}
	};
}}}}
#endif