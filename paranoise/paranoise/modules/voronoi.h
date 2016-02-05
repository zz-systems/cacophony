#pragma once
#ifndef PARANOISE_MODULES_VORONOI
#define PARANOISE_MODULES_VORONOI

#include <limits>
#include "../noisegenerators.h"
//#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	//using namespace x87compat;

	struct voronoi_settings
	{
		double	frequency = 1.0, 
				displacement = 2.0;
		int seed = 0;
		bool enableDistance;
	};

	
	SIMD_ENABLE(TReal, TInt)
	inline TReal voronoi(const Vector3<TReal>& coords, const voronoi_settings& settings)
	{
		// This method could be more efficient by caching the seed values.  Fix
		// later.

		auto _coords = coords * Vector3<TReal>(settings.frequency);

		Vector3<TInt> cube;
		cube.x = TInt(_coords.x) - TInt((_coords.x > 0) & 1);
		cube.y = TInt(_coords.y) - TInt((_coords.y > 0) & 1);
		cube.z = TInt(_coords.z) - TInt((_coords.z > 0) & 1);

		TReal minDist = std::numeric_limits<int>::min();
		TReal candidate;

		// Inside each unit cube, there is a seed point at a random position.  Go
		// through each of the nearby cubes until we find a cube with a seed point
		// that is closest to the specified position.
		for (TInt z = cube.z - 2; z <= cube.z + 2; z = z + 1)
		for (TInt y = cube.y - 2; y <= cube.y + 2; y = y + 1)
		for (TInt x = cube.x - 2; x <= cube.x + 2; x = x + 1)
		{
			auto cur = Vector3<TInt>(x, y, z);

			auto pos = cur + {
				ValueNoise3D(cur, settings.seed),
				ValueNoise3D(cur, settings.seed + 1),
				ValueNoise3D(cur, settings.seed + 2),
			};

			auto dist	 = pos - _coords;
			auto absDist = dot(dist, dist);

			auto mdmask = TInt(dist < minDist);

			minDist		= dist & mdmask | minDist   & ~mdmask;
			candidate	= pos  & mdmask | candidate & ~mdmask;
		}

		TReal value = 0;
		if (settings.enableDistance)
		{
			auto diff = candidate - _coords;
			value = sqrt(dot(diff, diff)) * SQRT_3 - 1.0;
		}

		return value + (settings.displacement * ValueNoise3D(TInt(floor(candidate))));
	}
}}
#endif