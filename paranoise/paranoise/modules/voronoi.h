#pragma once
#ifndef PARANOISE_MODULES_VORONOI
#define PARANOISE_MODULES_VORONOI

#include <limits>
#include "../noisegenerators.h"
#include "../parallel/all.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace parallel;
	struct voronoi_settings
	{
		float	frequency, 
				displacement;
		int seed;
		bool enableDistance;

		voronoi_settings(float frequency = 1.0, float displacement = 2.0, int seed = 0, bool enableDistance = false)
			: frequency(frequency), displacement(displacement), seed(seed), enableDistance(enableDistance)
		{}
	};

	
	SIMD_ENABLE(TReal, TInt)
	inline TReal voronoi(const Vector3<TReal>& coords, const voronoi_settings& settings)
	{
		// This method could be more efficient by caching the seed values.  Fix
		// later.

		auto _coords = coords * Vector3<TReal>(settings.frequency);

		Vector3<TInt> cube;// = _coords - ((_coords > 0) & 1);
		cube.x = (TInt) (_coords.x - sel(_coords.x > 0, 1, 0)); //((_coords.x > 0) & 1));
		cube.y = (TInt) (_coords.y - sel(_coords.y > 0, 1, 0)); //((_coords.y > 0) & 1));
		cube.z = (TInt) (_coords.z - sel(_coords.z > 0, 1, 0)); //((_coords.z > 0) & 1));

		TReal minDist = std::numeric_limits<TReal>::min();
		Vector3<TReal> candidate;

		auto word = sizeof(TReal) >> 2;
		// Inside each unit cube, there is a seed point at a random position.  Go
		// through each of the nearby cubes until we find a cube with a seed point
		// that is closest to the specified position.

		//for (int dim = 0; dim < word; dim++)
		//for (int z = extract(cube.z)[dim] - 2; z <= extract(cube.z)[dim] + 2; z += 1)
		//for (int y = extract(cube.y)[dim] - 2; y <= extract(cube.y)[dim] + 2; y += 1)
		//for (int x = extract(cube.x)[dim] - 2; x <= extract(cube.x)[dim] + 2; x += 1)

		for (int zoff = -2; zoff <= 2; zoff++)
		for (int yoff = -2; yoff <= 2; yoff++)
		for (int xoff = -2; xoff <= 2; xoff++)
		{
			auto cur = Vector3<TReal>(cube.x + xoff, cube.y + yoff, cube.z + zoff);

			auto pos = cur + Vector3<TReal> {
				ValueNoise3D<TReal, TInt>(cur, settings.seed),
				ValueNoise3D<TReal, TInt>(cur, settings.seed + 1),
				ValueNoise3D<TReal, TInt>(cur, settings.seed + 2),
			};

			auto dist = pos - _coords;
			auto absDist = dot(dist, dist);

			select_candidate(absDist, pos, minDist, candidate);
			//find_candidate<TReal, TInt>(_coords, Vector3<TReal>(x, y, z), settings.seed, minDist, candidate);
		}

		/*find_candidate(Vector3<TReal>(cube.x - 2, cube.y - 2, cube.z - 2), minDist, candidate);
		find_candidate(Vector3<TReal>(cube.x - 2, cube.y - 2, cube.z - 1), minDist, candidate);
		find_candidate(Vector3<TReal>(cube.x - 2, cube.y - 2, cube.z - 0), minDist, candidate);
		find_candidate(Vector3<TReal>(cube.x - 2, cube.y - 2, cube.z + 1), minDist, candidate);
		find_candidate(Vector3<TReal>(cube.x - 2, cube.y - 2, cube.z + 2), minDist, candidate);*/

		TReal value = 0;
		if (settings.enableDistance)
		{
			auto diff = candidate - _coords;
			value = paranoise::sqrt(dot(diff, diff)) * SQRT_3 - 1.0f;
		}

		return value + (settings.displacement * ValueNoise3D<TReal, TInt>(Vector3<TInt>(candidate), settings.seed));
	}	

	SIMD_ENABLE_F(TReal)
	inline void select_candidate(const TReal &absDist, const Vector3<TReal> &pos, TReal &minDist, Vector3<TReal> &candidate)
	{
		auto mask	= absDist < minDist;
		minDist		= sel(mask, absDist, minDist);
		candidate	= sel(Vector3<TReal>(mask), pos, candidate);
	}

	template<>
	inline void select_candidate(const float &absDist, const Vector3<float> &pos, float &minDist, Vector3<float> &candidate)
	{
		auto mask	= absDist < minDist;
		minDist		= sel(mask, absDist, minDist);
		candidate	= sel(mask, pos, candidate);
	}

}}
#endif