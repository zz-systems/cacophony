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


#include "dispatch.h"

namespace zzsystems { namespace solowej {
    using namespace gorynych;

//static function<shared_ptr<simd_engine>()> get_engine;

static void register_branches()
{
	//STATIC_DISPATCH_ONE((compiler<capability>::register_compiler<compiler_v09>()));
}

#if defined(COMPILE_AVX2)



        std::shared_ptr<engine_base> get_avx2_engine()
        {
        	return static_pointer_cast<engine_base>(make_shared<avx2_engine>());
        }
#elif defined(COMPILE_AVX1)

		std::shared_ptr<engine_base> get_avx1_engine()
		{
			return static_pointer_cast<engine_base>(make_shared<avx1_engine>());
		}
#elif defined(COMPILE_SSE4FMA)

		std::shared_ptr<engine_base> get_sse4fma_engine()
		{
			return static_pointer_cast<engine_base>(make_shared<sse4fma_engine>());
		}
#elif defined(COMPILE_SSE4)

        std::shared_ptr<engine_base> get_sse4_engine()
		{
			return static_pointer_cast<engine_base>(make_shared<sse4_engine>());
		}
#elif defined(COMPILE_SSSE3)

        std::shared_ptr<engine_base> get_ssse3_engine()
		{
			return static_pointer_cast<engine_base>(make_shared<ssse3_engine>());
		}
#elif defined(COMPILE_SSE3)

        std::shared_ptr<engine_base> get_sse3_engine()
		{
			return static_pointer_cast<engine_base>(make_shared<sse3_engine>());
		}
#elif defined(COMPILE_SSE2)

        std::shared_ptr<engine_base> get_sse2_engine()
		{
			return static_pointer_cast<engine_base>(make_shared<sse2_engine>());
		}
#else

        std::shared_ptr<engine_base> get_fpu_engine()
		{
			return static_pointer_cast<engine_base>(make_shared<fpu_engine>());
		}
#endif
        //}

}}