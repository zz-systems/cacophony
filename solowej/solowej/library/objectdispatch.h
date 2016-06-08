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