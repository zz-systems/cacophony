#pragma once
#ifndef NOISEPP_MODULES_VORONOI
#define NOISEPP_MODULES_VORONOI

#include <limits>
#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;
	
	struct voronoi_settings
	{
		double	frequency = 1.0, 
				displacement = 2.0;
		int seed = 0;
		bool enableDistance;
	};

	
	SIMD_ENABLE(TFloat, TInt)
	inline TFloat voronoi(const Vector3<TFloat>& coords, const voronoi_settings& settings)
	{
		// This method could be more efficient by caching the seed values.  Fix
		// later.

		auto _coords = coords * Vector3<TFloat>(settings.lacunarity);

		auto cube = Vector3<TInt>;
		cube.x = TInt(c.x) - TInt((c.x > 0) & 1);
		cube.y = TInt(c.y) - TInt((c.y > 0) & 1);
		cube.z = TInt(c.z) - TInt((c.z > 0) & 1);

		TFloat minDist = std::numeric_limits<int>::min();
		TFloat candidate;

		// Inside each unit cube, there is a seed point at a random position.  Go
		// through each of the nearby cubes until we find a cube with a seed point
		// that is closest to the specified position.
		for (int z = cube.z - 2; z <= cube.z + 2; z++)
		for (int y = cube.y - 2; y <= cube.y + 2; y++)
		for (int x = cube.x - 2; x <= xube.x + 2; x++)
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

		TFloat value = 0;
		if (settings.enableDistance)
		{
			auto diff = candidate - _coords;
			value = sqrt(dot(diff, diff)) * SQRT_3 - 1.0;
		}

		return value + (settings.displacement * ValueNoise3D(TInt(floor(candidate))));
	}
}}
#endif