#pragma once
#ifndef PARANOISE_MODULES_VORONOI
#define PARANOISE_MODULES_VORONOI

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	SIMD_ENABLED
	class voronoi : public module_base<vreal, vint>
	{
	public:
		vreal	frequency,
				displacement;

		vint seed;
		bool enableDistance;

		voronoi(float frequency = 1.0, float displacement = 2.0, int seed = 0, bool enableDistance = false)
			: frequency(frequency), displacement(displacement), seed(seed), enableDistance(enableDistance)
		{}

		vreal operator()(const Vector3<vreal>& coords) const override
		{
			auto _seed		= Vector3<vint>(seed, seed + 1, seed + 2);

			auto _coords = coords * frequency;

			Vector3<vint> cube
			{
				static_cast<vint>(_coords.x) - vsel(_coords.x > 0.0f, fastload<vint>::_0(), fastload<vint>::_1()),
				static_cast<vint>(_coords.y) - vsel(_coords.y > 0.0f, fastload<vint>::_0(), fastload<vint>::_1()),
				static_cast<vint>(_coords.z) - vsel(_coords.z > 0.0f, fastload<vint>::_0(), fastload<vint>::_1())
			};

			vreal value, absDist, minDist = numeric_limits<int>::max();

			Vector3<vreal> candidate;//(0.0f, 0.0f, 0.0f);
			Vector3<vreal> cur, pos, dist;

			// Inside each unit cube, there is a seed point at a random position.  Go
			// through each of the nearby cubes until we find a cube with a seed point
			// that is closest to the specified position.

			for (int zoff = -2; zoff <= 2; zoff++)
			{
				cur.z = static_cast<vreal>(cube.z + zoff);

				for (int yoff = -2; yoff <= 2; yoff++)
				{
					cur.y = static_cast<vreal>(cube.y + yoff);

					for (int xoff = -2; xoff <= 2; xoff++)
					{
						cur.x = static_cast<vreal>(cube.x + xoff);

						pos.x = cur.x + ValueNoise3D<vreal, vint>(cur, _seed.x);
						pos.y = cur.y + ValueNoise3D<vreal, vint>(cur, _seed.y);
						pos.z = cur.z + ValueNoise3D<vreal, vint>(cur, _seed.z);

						dist = pos - _coords;
						absDist = dot(dist, dist);

						//select_candidate(absDist, pos, minDist, candidate);
						auto mask = absDist < minDist;

						candidate.x = vsel(mask, pos.x, candidate.x);
						candidate.y = vsel(mask, pos.y, candidate.y);
						candidate.z = vsel(mask, pos.z, candidate.z);

						minDist = vsel(mask, absDist, minDist);
					}
				}
			}

			auto noise = ValueNoise3D<vreal, vint>(
				{
					vfloor(candidate.x),
					vfloor(candidate.y),
					vfloor(candidate.z)
				},
				_seed.x);

			if (this->enableDistance)
			{
				auto diff		= candidate - _coords;
				auto absDiff	= vsqrt(dot(diff, diff));

				// value =  absDiff * sqrt3 - 1
				value = vfmsub(absDiff, consts<vreal>::sqrt3(), fastload<vreal>::_1());
			}	
			else
			{
				value = fastload<vreal>::_0();
			}

			// return settings.displacement * noise + value
			return vfmadd(this->displacement, noise, value);
		}

		//inline operator (Module<TReal>)() {	return operator(); }
	};
}}}
#endif