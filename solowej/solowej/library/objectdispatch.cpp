//
// Created by szuyev on 14.05.16.
//
#include "objectdispatch.h"

namespace zzsystems { namespace solowej {
    using namespace gorynych;


#if defined(COMPILE_AVX2)
        std::shared_ptr<simd_engine> get_avx2_engine()
        {
        	return static_pointer_cast<simd_engine>(make_shared<avx2_engine>());
        }
#elif defined(COMPILE_AVX1)
		std::shared_ptr<simd_engine> get_avx1_engine()
		{
			return static_pointer_cast<simd_engine>(make_shared<avx1_engine>());
		}
#elif defined(COMPILE_SSE4FMA)
		std::shared_ptr<simd_engine> get_sse4fma_engine()
		{
			return static_pointer_cast<simd_engine>(make_shared<sse4fma_engine>());
		}
#elif defined(COMPILE_SSE4)
        std::shared_ptr<simd_engine> get_sse4_engine()
		{
			return static_pointer_cast<simd_engine>(make_shared<sse4_engine>());
		}
#elif defined(COMPILE_SSSE3)
        std::shared_ptr<simd_engine> get_ssse3_engine()
		{
			return static_pointer_cast<simd_engine>(make_shared<ssse3_engine>());
		}
#elif defined(COMPILE_SSE3)
        std::shared_ptr<simd_engine> get_sse3_engine()
		{
			return static_pointer_cast<simd_engine>(make_shared<sse3_engine>());
		}
#elif defined(COMPILE_SSE2)
        std::shared_ptr<simd_engine> get_sse2_engine()
		{
			return static_pointer_cast<simd_engine>(make_shared<sse2_engine>());
		}
#else
        std::shared_ptr<simd_engine> get_fpu_engine()
		{
			return static_pointer_cast<simd_engine>(make_shared<fpu_engine>());
		}
#endif
        //}

}}