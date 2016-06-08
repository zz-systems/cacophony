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


#include "simd_engine.h"
#include "scheduler.h"
#include "compiler.h"

namespace zzsystems {
    namespace solowej {

        template<typename capability>
        class specialized_simd_engine : public simd_engine
        {
            using vreal = typename static_dispatcher<capability>::vreal;
            using vint  = typename static_dispatcher<capability>::vint;
        public:
            // COmpile from json object
            virtual void compile(const nlohmann::json &source) override
            {
                cout << ">>compiling " << static_dispatcher<capability>::unit_name() << " branch" << endl;

                _scheduler << source["environment"]["scheduler"];

                _scheduler.source = _compiler.compile(source);
                //_scheduler.transform = [](const auto&c) {return c;};
            }

            // Run - generating an array inside
            virtual float* run(const vec3<float> &origin) override
            {
                cout << ">>executing " << static_dispatcher<capability>::unit_name() << " branch" << endl;
                return _scheduler(origin);
            }

            // Run in place
            virtual void run(const vec3<float> &origin, float *target) override
            {
                cout << ">>executing " << static_dispatcher<capability>::unit_name() << " branch" << endl;
                _scheduler(target, origin);
            }
        private:
            cpu_scheduler<vreal> _scheduler;
            compiler<vreal, vint> _compiler;
        };

#if defined(COMPILE_AVX2)
        class alignas(32) avx2_engine : public specialized_simd_engine<capability_AVX2>
        { };
#elif defined(COMPILE_AVX1)
        class alignas(32) avx1_engine : public specialized_simd_engine<capability_AVX1>
        { };
#elif defined(COMPILE_SSE4FMA)
        class alignas(16) sse4fma_engine : public specialized_simd_engine<capability_SSE4FMA>
        { };
#elif defined(COMPILE_SSE4)
        class alignas(16) sse4_engine : public specialized_simd_engine<capability_SSE4>
        { };
#elif defined(COMPILE_SSSE3)
        class alignas(16) ssse3_engine : public specialized_simd_engine<capability_SSSE3>
        { };
#elif defined(COMPILE_SSE3)
        class alignas(16) sse3_engine : public specialized_simd_engine<capability_SSE3>
        { };
#elif defined(COMPILE_SSE2)
        class alignas(16) sse2_engine : public specialized_simd_engine<capability_SSE2>
        { };
#else
        class fpu_engine : public specialized_simd_engine<capability_FPU>
        { };
#endif


    }
}
