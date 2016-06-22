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

namespace zzsystems { namespace solowej { namespace platform {
    using namespace gorynych;
    using namespace modules;

    DISPATCHED class ALIGNED cpu_scheduler_mt :
        public scheduler_base<capability>
    {
        using vreal = typename static_dispatcher<capability>::vreal;
        using vint  = typename static_dispatcher<capability>::vint;

    public:
        virtual void schedule(float *target, const vec3<float> &origin) /*const*/ override
        {
            size_t word = dim<vreal>();
            auto d = this->config.dimensions;

            if(d.z >= d.y)
            {
                //parallel_for(irange(0, d.z), [&](const int z)
                #pragma omp parallel for
                for(int z = 0; z < d.z; z++)
                {
                    _MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
                    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
                    auto depth = z * d.y;

                    for(int y = 0; y < d.y; y++)
                    {
                        auto stride = target + ((depth + y) * d.x);
                        auto remainder = d.x % word;
                        auto adjust = remainder > 0 ? word - remainder : 0;
                        for (size_t x = 0; x < d.x; x += word)
                        {
                            auto coords = this->transform(this->build_coords(
                                static_cast<vreal>(static_cast<int>(x)),
                                static_cast<vreal>(y),
                                static_cast<vreal>(z)));


                            auto r = this->source(static_cast<vec3<vreal>>(origin) + coords);

                            this->write_result(d, stride, remainder, x, r);
                        }
                    }
                }//);
            }
            else
            {
                for(int z = 0; z < d.z; z++)
                {
                    auto depth = z * d.y;


                    //parallel_for(irange(0, d.y), [&](const int y)
                    #pragma omp parallel for
                    for(int y = 0; y < d.y; y++)
                    {
                        _MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
                        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

                        auto stride = target + ((depth + y) * d.x);
                        auto remainder = d.x % word;
                        auto adjust = remainder > 0 ? word - remainder : 0;

                        for (size_t x = 0; x < d.x; x += word)
                        {
                            auto coords = this->transform(this->build_coords(
                                    static_cast<vreal>(static_cast<int>(x)),
                                    static_cast<vreal>(y),
                                    static_cast<vreal>(z)));


                            auto r = this->source(static_cast<vec3<vreal>>(origin) + coords);

                            this->write_result(d, stride, remainder, x, r);
                        }
                    }//);
                }
            }
        }
    };
}}}