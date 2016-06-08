//
// Created by szuyev on 16.05.16.
//

#pragma once

#include "../engine/specialized_simd_engines.h"

namespace zzsystems { namespace solowej {


//#define instantiation \
    //{ \
    //    template class parser<vreal, vint>; \
    //   template class cpu_scheduler<vreal>;\
    //}

        //void instantiate() {

#if defined(COMPILE_AVX2)
    std::shared_ptr<simd_engine> get_avx2_engine();
#endif
#if defined(COMPILE_AVX1)
    std::shared_ptr<simd_engine> get_avx1_engine();
#endif
#if defined(COMPILE_SSE4FMA)
    std::shared_ptr<simd_engine> get_sse4fma_engine();
#endif
#if defined(COMPILE_SSE4)
    std::shared_ptr<simd_engine> get_sse4_engine();
#endif
#if defined(COMPILE_SSSE3)
    std::shared_ptr<simd_engine> get_ssse3_engine();
#endif
#if defined(COMPILE_SSE3)
    std::shared_ptr<simd_engine> get_sse3_engine();
#endif
#if defined(COMPILE_SSE2)
    std::shared_ptr<simd_engine> get_sse2_engine();
#endif
    std::shared_ptr<simd_engine> get_fpu_engine();


    }}