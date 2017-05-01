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

#include "modules/generators/perlin.hpp"

namespace cacophony { namespace modules {
    using namespace zacc;
    using namespace math;

    MODULE(mod_displace)
    {
    public:
        mod_displace() :
                BASE(mod_displace)::cloneable(4)
        {}

        MODULE_PROPERTY(source, 0)
        MODULE_PROPERTY(x, 1)
        MODULE_PROPERTY(y, 2)
        MODULE_PROPERTY(z, 3)


        // Apply turbulence to the source input
        vreal operator()(const vec3<vreal> &coords) const override
        {
            vec3<vreal> distortion( get_x()(coords), get_y()(coords), get_z()(coords) );

            return get_source()(coords + distortion);
        }
    };
}}