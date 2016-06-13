//
// Vectorized and parallelized version of libnoise using the gorynych SIMD toolkit
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

#include "../dependencies.h"

namespace zzsystems { namespace solowej { namespace modules {
	using namespace gorynych;
	
	using namespace math;

	MODULE(mod_buffer)
	{
	public:
		MODULE_PROPERTY(source, 0)

		mod_buffer(const Module<vreal> &source)
			: BASE(mod_buffer)::cloneable(1), _is_cached(false)
		{}

		mod_buffer(const mod_buffer<vreal, vint> &source)
			: BASE(mod_buffer)::cloneable(source), _is_cached(source._is_cached)
		{}

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto eq = coords == _buf_coords;
			if(!(_is_cached && static_cast<bool>(eq.x) && static_cast<bool>(eq.y) && static_cast<bool>(eq.z)))
			{
				_buffered = get_source()(coords);
				_buf_coords = coords;
			}

			_is_cached = true;
			return _buffered;			
		}		

	private:
		mutable vec3<vreal> _buf_coords;
		mutable vreal _buffered;
		mutable bool _is_cached;
	};

}}}