#pragma once
#ifndef PARANOISE_MODULES_VORONOI
#define PARANOISE_MODULES_VORONOI

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED
	class voronoi : 
		public cloneable<module_base<SIMD_T>, voronoi<SIMD_T>>,
		public serializable<json>
	{
	public:
		vreal	frequency,
				displacement;

		vint seed;
		bool enableDistance;

		voronoi(float frequency = 1.0, float displacement = 2.0, int seed = 0, bool enableDistance = false)
			: cloneable(0), frequency(frequency), displacement(displacement), seed(seed), enableDistance(enableDistance)
		{}

		const json& operator <<(const json &source) override
		{
			frequency = source.value("frequency", 1.0f);
			displacement = source.value("displacement", 2.0f);

			seed = source.value("seed", 0);
			enableDistance = source.value("enableDistance", false);

			return source;
		}

		vreal operator()(const vec3<vreal>& coords) const override
		{	
			auto _coords = coords * frequency;
			auto _seed = vec3<vint>(seed, seed + 1, seed + 2);
						
			vec3<vint> lowerEdge
			(
				vfloor(_coords.x), 
				vfloor(_coords.y), 
				vfloor(_coords.z)  
			);

			vreal value(0), absDist(0), minDist(static_cast<float>(numeric_limits<int>::max()));

			vec3<vreal> candidate(0);//(0.0f, 0.0f, 0.0f);
			vec3<vreal> cur_r(0), pos(0), dist(0);
			vec3<vint> cur(0);

			// Inside each unit cube, there is a seed point at a random position.  Go
			// through each of the nearby cubes until we find a cube with a seed point
			// that is closest to the specified position.

			for (auto zoff = -2; zoff <= 2; zoff++)
			{	
				cur_r.z = static_cast<vreal>(cur.z = lowerEdge.z + zoff);

				for (auto yoff = -2; yoff <= 2; yoff++)
				{
					cur_r.y = static_cast<vreal>(cur.y = lowerEdge.y + yoff);

					for (auto xoff = -2; xoff <= 2; xoff++)
					{
						cur_r.x = static_cast<vreal>(cur.x = lowerEdge.x + xoff);

						pos.x = cur_r.x + ValueNoise3D<vreal, vint>(cur, _seed.x);
						pos.y = cur_r.y + ValueNoise3D<vreal, vint>(cur, _seed.y);
						pos.z = cur_r.z + ValueNoise3D<vreal, vint>(cur, _seed.z);

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
					static_cast<vint>(vfloor(candidate.x)),
					static_cast<vint>(vfloor(candidate.y)),
					static_cast<vint>(vfloor(candidate.z))
				},
				_seed.x);

			auto v = value;

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

			/*if (dim<vreal>() > 1)
				cout << extract(noise)[4] << endl;*/
			/*v = value;
			cout << extract(v)[0] << endl;*/
			// return settings.displacement * noise + value
			return vfmadd(this->displacement, noise, value);
		}
	};
}}}
#endif