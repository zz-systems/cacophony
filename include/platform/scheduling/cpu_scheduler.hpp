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

#include <type_traits>

namespace cacophony { namespace platform {
    using namespace zacc;
    using namespace modules;

    DISPATCHED class alignas(branch::alignment) cpu_scheduler :
        public scheduler_base<branch>
    {
        using base_t = scheduler_base<branch>;
    public:
        virtual void schedule(float *target, const vec3<float> &origin) /*const*/ override
        {
            std::cout << ">>executing " << branch::major_branch_name() << " branch @CPU" << std::endl;

            //execute<float>(target, origin);

            auto d          = this->config().dimensions;
            auto start      = this->transform(static_cast<vec3<zfloat>>(origin));
            auto real_size  = d.x * d.y * d.z;

            adjust_rounding_mode();

            zacc::generate<zfloat>(target, target + real_size, [this, &start](auto i)
            {
                // Somehow the result is shift by one pixel. Compensate by adding 1 to start.
                auto point = this->transform(this->build_coords(i - 1));

                return this->source(point + start);
            });
        }

        virtual void schedule(int *target, const vec3<float> &origin) /*const*/ override
        {
            std::cout << ">>executing " << branch::major_branch_name() << " branch @CPU" << std::endl;
            //execute<int>(target, origin);

            auto d          = this->config().dimensions;
            auto start      = this->transform(static_cast<vec3<zfloat>>(origin));
            auto real_size  = d.x * d.y * d.z;

            adjust_rounding_mode();

            zacc::generate<zint>(target, target + real_size, [this, &start](auto i)
            {
                // Somehow each strobe is shifted by one pixel. Compensate by building coordinates fpor previous index.
                auto point = this->transform(this->build_coords(i - 1));

                return static_cast<zint>(this->source(point + start).floor());
            });
        }
    };
}}