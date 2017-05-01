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
#include <unordered_map>
#include <mutex>


namespace cacophony { namespace modules {
	//using namespace std;

	using namespace math;

	MODULE(mod_cache)
	{
	public:
		typedef std::unordered_map<float, float> xmap;
		typedef std::unordered_map<float, xmap>  yxmap;
		typedef std::unordered_map<float, yxmap> zyxmap;

		Module<branch> source;

		mod_cache(const Module<branch> &source)
			: source(source), _mut(zacc::make_shared<std::mutex>())
		{}

		mod_cache(const mod_cache<branch> &source)
			: source(source.source), _mut(source._mut)
		{}
		//#error TODO: slices! SIMD: [x0 x1 x2 x3] [y0 y0 y0 y0] [z0 z0 z0 z0]
		inline zfloat operator()(const vec3<zfloat>& coords) const override
		{
			vec3<zfloat> _coords = coords;

			bool dirty = false;
			zfloat	result;

			float   x[dim<zfloat>()], y[dim<zfloat>()], z[dim<zfloat>()], e[dim<zfloat>()];

			extract(_coords.x, x);
			extract(_coords.y, y);
			extract(_coords.z, z);

			_mut->lock();

			for (size_t i = 0; i < dim<zfloat>(); i++)
			{
				auto zc = _cache.find(z[i]);
				if (zc == _cache.end())
				{
					dirty = true;
					break;
				}

				auto yc = zc->second.find(y[i]);
				if (yc == zc->second.end())
				{
					dirty = true;
					break;
				}

				auto xc = yc->second.find(x[i]);
				if (xc == yc->second.end())
				{
					dirty = true;
					break;
				}


				e[i] = _cache[z[i]][y[i]][x[i]];
			}


			_mut->unlock();

			if (dirty)
			{
				result = source(coords);

				_mut->lock();

				for (size_t i = 0; i < dim<zfloat>(); i++)
				{				

					if (_cache.find(z[i]) == _cache.end())
						_cache.insert({ z[i], yxmap() });
					if (_cache[z[i]].find(y[i]) == _cache[z[i]].end())
						_cache[z[i]].insert({ y[i], xmap() });

					if (_cache[z[i]][y[i]].find(x[i]) == _cache[z[i]][y[i]].end())
						_cache[z[i]][y[i]].insert({ x[i], e[i] });
					else
						_cache[z[i]][y[i]][x[i]] = e[i];

					
					/*_cache.try_emplace(z[i], yxmap());
					_cache[z[i]].try_emplace(y[i], xmap());
					_cache[z[i]][y[i]][x[i]] = e[i];*/
				}

				_mut->unlock();
			}
			result = e;
			return result;
		}

		inline operator Module<branch>() const override { return [this](const auto &c) { return this->operator()(c); }; }

	private:
		mutable zyxmap	_cache;
		std::shared_ptr<std::mutex>	_mut;
	};
}}