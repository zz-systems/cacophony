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
#include "../generators/perlin.h"

namespace zzsystems { namespace solowej { namespace modules {
            using namespace gorynych;
            using namespace math;

            MODULE(mod_steepness)
            {
            public:
                vreal terrain_height;

                vint terrain_width, terrain_height;

                mod_steepness() :
                        BASE(mod_steepness)::cloneable(1)
                {}

                mod_steepness(const mod_steepness<vreal, vint>& rhs)
                        : BASE(mod_steepness)::cloneable(rhs)
                {}

                MODULE_PROPERTY(source, 0)

                const json& operator <<(const json &source) override
                {


                    return source;
                }

                // Apply turbulence to the source input
                vreal operator()(const vec3<vreal> &coords) const override
                {
                    auto source = *get_source();

                    auto slopeX = source(vsel(coords.x() < terrain_width, coords.x() + 1, x), coords.y(), 0)
                                - source(vsel(coords.x() > 0,             coords.x() - 1, x), coords.y(), 0);

                    auto slopeZ = source(coords.x(), vsel(coords.y() < terrain_height, coords.y() + 1, coords.y()), 0)
                                - source(coords.x(), vsel(coords.y() > 0,              coords.y() + -1, coords.y()), 0);


                    slopeX = vsel(coords.x() == 0 || coords.x() == terrain_width - 1,  slopeX * 2, slopeX);
                    slopeZ = vsel(coords.y() == 0 || coords.y() == terrain_height - 1, slopeZ * 2, slopeZ);

                    vec3 normal(-slopeX, 2, slopeZ);

                    //normal.Normalize();
                    //float steepness = Mathf.Acos(Vector3.Dot(normal, Vector3.up));

                    return 0;
                }
            private:

            };
        }}}