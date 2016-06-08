//
// Created by szuyev on 16.05.16.
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
