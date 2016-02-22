#pragma once
#ifndef PARANOISE_MODULES_VORONOI
#define PARANOISE_MODULES_VORONOI

#include <limits>
#include "../noisegenerators.h"
#include "../parallel/all.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace parallel;

	SIMD_ENABLE(TReal, TInt)
	struct voronoi
	{
		TReal	frequency,
				displacement;

		TInt seed;
		bool enableDistance;

		voronoi(float frequency = 1.0, float displacement = 2.0, int seed = 0, bool enableDistance = false)
			: frequency(frequency), displacement(displacement), seed(seed), enableDistance(enableDistance)
		{}

		inline TReal operator()(const Vector3<TReal>& coords) const
		{
			auto _seed		= Vector3<TInt>(seed, seed + 1, seed + 2);

			auto _coords = coords * frequency;

			Vector3<TInt> cube
			{
				static_cast<TInt>(_coords.x) - vsel(_coords.x > 0.0f, fastload<TInt>::_0(), fastload<TInt>::_1()),
				static_cast<TInt>(_coords.y) - vsel(_coords.y > 0.0f, fastload<TInt>::_0(), fastload<TInt>::_1()),
				static_cast<TInt>(_coords.z) - vsel(_coords.z > 0.0f, fastload<TInt>::_0(), fastload<TInt>::_1())
			};

			TReal value, absDist, minDist = std::numeric_limits<int>::max();

			Vector3<TReal> candidate( 0.0f, 0.0f, 0.0f);
			Vector3<TReal> cur, pos, dist;

			// Inside each unit cube, there is a seed point at a random position.  Go
			// through each of the nearby cubes until we find a cube with a seed point
			// that is closest to the specified position.

			for (int zoff = -2; zoff <= 2; zoff++)
			{
				cur.z = static_cast<TReal>(cube.z + zoff);

				for (int yoff = -2; yoff <= 2; yoff++)
				{
					cur.y = static_cast<TReal>(cube.y + yoff);

					for (int xoff = -2; xoff <= 2; xoff++)
					{
						cur.x = static_cast<TReal>(cube.x + xoff);

						pos.x = cur.x + ValueNoise3D<TReal, TInt>(cur, _seed.x);
						pos.y = cur.y + ValueNoise3D<TReal, TInt>(cur, _seed.y);
						pos.z = cur.z + ValueNoise3D<TReal, TInt>(cur, _seed.z);

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

			auto noise = ValueNoise3D<TReal, TInt>(candidate, _seed.x);

			if (this->enableDistance)
			{
				auto diff		= candidate - _coords;
				auto absDiff	= vsqrt(dot(diff, diff));

				// value =  absDiff * sqrt3 - 1
				value = vfmsub(absDiff, consts<TReal>::sqrt3(), fastload<TReal>::_1());
			}			

			// return settings.displacement * noise + value
			return vfmadd(this->displacement, noise, value);
		}

		//inline operator (Module<TReal>)() {	return operator(); }
	};
}}
#endif