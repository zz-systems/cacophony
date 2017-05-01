//
// Vectorized and parallelized version of libnoise using the zacc SIMD toolkit
// Copyright (C) 2015-2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Original libnoise: 
// Copyright (C) 2003, 2004 Jason Bevins
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "modules/dependencies.hpp"

namespace cacophony { namespace modules {
	using namespace zacc;
	using namespace math;

	MODULE(mod_voronoi)
	{
	public:
		zfloat	frequency,
				displacement;

		zint seed;
		bool enableDistance;

		mod_voronoi(float frequency = 1.0, float displacement = 2.0, int seed = 0, bool enableDistance = false)
			: BASE(mod_voronoi)::cloneable(0), frequency(frequency), displacement(displacement), seed(seed), enableDistance(enableDistance)
		{}

		void deserialize(const json &source) override
		{
			frequency = source.value<float>("frequency", 1.0f);
			displacement = source.value<float>("displacement", 2.0f);

			seed = source.value<int>("seed", 0);
			enableDistance = source.value<bool>("enableDistance", false);

		}

		zfloat operator()(const vec3<zfloat>& coords) const override
		{	
			auto _coords = coords * frequency;
			auto _seed = vec3<zint>(seed, seed + 1, seed + 2);
						
			vec3<zint> lowerEdge
			(
				static_cast<zint>(vfloor(_coords.x)),
				static_cast<zint>(vfloor(_coords.y)),
				static_cast<zint>(vfloor(_coords.z))
			);

			zfloat value(0), absDist(0), minDist(static_cast<float>(std::numeric_limits<int>::max()));

			vec3<zfloat> candidate(0);//(0.0f, 0.0f, 0.0f);
			vec3<zfloat> cur_r(0), pos(0), dist(0);
			vec3<zint> cur(0);

			// Inside each unit cube, there is a seed point at a random position.  Go
			// through each of the nearby cubes until we find a cube with a seed point
			// that is closest to the specified position.

			for (auto zoff = -2; zoff <= 2; zoff++)
			{	
				cur_r.z = static_cast<zfloat>(cur.z = lowerEdge.z + zoff);

				for (auto yoff = -2; yoff <= 2; yoff++)
				{
					cur_r.y = static_cast<zfloat>(cur.y = lowerEdge.y + yoff);

					for (auto xoff = -2; xoff <= 2; xoff++)
					{
						cur_r.x = static_cast<zfloat>(cur.x = lowerEdge.x + xoff);

						pos.x = cur_r.x + noisegen<branch>::realvalue_3d(cur, _seed.x);
						pos.y = cur_r.y + noisegen<branch>::realvalue_3d(cur, _seed.y);
						pos.z = cur_r.z + noisegen<branch>::realvalue_3d(cur, _seed.z);

						dist = pos - _coords;
						absDist = dist.dot(dist);

						//select_candidate(absDist, pos, minDist, candidate);
						auto mask = absDist < minDist;

						candidate.x = vsel(mask, pos.x, candidate.x);
						candidate.y = vsel(mask, pos.y, candidate.y);
						candidate.z = vsel(mask, pos.z, candidate.z);

						minDist = vsel(mask, absDist, minDist);
					}
				}
			}
						
				auto noise = noisegen<branch>::realvalue_3d(
				{
					static_cast<zint>(vfloor(candidate.x)),
					static_cast<zint>(vfloor(candidate.y)),
					static_cast<zint>(vfloor(candidate.z))
				},
				_seed.x);

			auto v = value;

			if (this->enableDistance)
			{
				auto diff		= candidate - _coords;
				auto absDiff	= diff.magnitude();
				
				// value =  absDiff * sqrt3 - 1
                // TODO
				value = vfmsub(absDiff, 1.73205080756887729353, 1);
			}	
			else
			{
				value = 0;
			}

			/*if (dim<zfloat>() > 1)
				cout << extract(noise)[4] << endl;*/
			/*v = value;
			cout << extract(v)[0] << endl;*/
			// return settings.displacement * noise + value
			return vfmadd(this->displacement, noise, value);
		}
	};
}}