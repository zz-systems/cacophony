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


#include "platform/dependencies.hpp"
#include <fstream>
#include <type_traits>

#include "engine_base.hpp"
#include "platform/compilers/compiler.hpp"
#include "platform/scheduling/scheduler.hpp"
#include "system/platform.hpp"

namespace cacophony { namespace platform
{
    DISPATCHED class alignas(branch::alignment) simd_engine :
            public engine_base
    {
    public:
        simd_engine()
        {
            std::cout << "creating simd_engine" << std::endl;
        }
        // COmpile from json object
        virtual void compile(const nlohmann::json &source) override
        {
            std::cout << ">>compiling " << branch::major_branch_name() << " branch" << std::endl;

            _scheduler << source["environment"]["scheduler"];

            _scheduler.set_source(_compiler.compile(source));
            _scheduler.transform = [](const auto&c) {return c;};

            //return _scheduler.get_config();
        }

        // Run - generating an array inside
        virtual float* run(const vec3<float> &origin) override
        {
            std::cout << ">>executing " << branch::major_branch_name() << " branch" << std::endl;
            return _scheduler(origin);
        }

        // Run in place
        virtual void run(const vec3<float> &origin, float *target) override
        {
            std::cout << ">>executing " << branch::major_branch_name() << " branch" << std::endl;
            _scheduler.schedule(target, origin);
        }

        // Run in place
        virtual void run(const vec3<float> &origin, int *target) override
        {
            std::cout << ">>executing " << branch::major_branch_name() << " branch" << std::endl;
            _scheduler.schedule(target, origin);
        }
    private:
        alignas(branch::alignment) scheduler<branch> _scheduler;
        alignas(branch::alignment) compiler<branch> _compiler;
    };
}}
