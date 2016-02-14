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

			Vector3<TInt> cube{
				(TInt)_coords.x - sel(_coords.x > 0, consts<TInt>::zero(), consts<TInt>::one()),
				(TInt)_coords.y - sel(_coords.y > 0, consts<TInt>::zero(), consts<TInt>::one()),
				(TInt)_coords.z - sel(_coords.z > 0, consts<TInt>::zero(), consts<TInt>::one())
			};

			TReal value, absDist, minDist = static_cast<TInt>(std::numeric_limits<int>::max());

			Vector3<TReal> candidate = { 0, 0, 0 };
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

						candidate.x = sel(mask, pos.x, candidate.x);
						candidate.y = sel(mask, pos.y, candidate.y);
						candidate.z = sel(mask, pos.z, candidate.z);

						minDist = sel(mask, absDist, minDist);
					}
				}
			}

			if (this->enableDistance)
			{
				auto diff		= candidate - _coords;
				auto absDiff	= paranoise::parallel::sqrt(dot(diff, diff));

				// value =  absDiff * sqrt3 - 1
				value = fmsub(absDiff, consts<TReal>::sqrt3(), consts<TReal>::one());
			}

			auto noise = ValueNoise3D<TReal, TInt>(candidate, _seed.x);

			// return settings.displacement * noise + value
			return fmadd(this->displacement, noise, value);
		}

		//inline operator (Module<TReal>)() {	return operator(); }
	};
}}
#endif