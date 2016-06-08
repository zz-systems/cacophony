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


#include "dependencies.h"
#include <fstream>
#include <type_traits>

#include "../../gorynych/gorynych/gorynych.h"

#include "../library/objectdispatch.h"
#include "simd_engine.h"
#include "scheduler_base.h"

namespace zzsystems { namespace solowej
{
	class engine :
		public simd_engine
	{		
	public:
		gorynych::system_info info;
		std::string file_name;
		scheduler_settings settings;

		aligned_map<int, shared_ptr<simd_engine>> _engines;

		engine()
		{
#if defined(COMPILE_AVX2)
				_engines[gorynych::capability_AVX2::value] 		= get_avx2_engine();
#endif
#if defined(COMPILE_AVX1)
				_engines[gorynych::capability_AVX1::value] 		= get_avx1_engine();
#endif
#if defined(COMPILE_SSE4FMA)
				_engines[gorynych::capability_SSE4FMA::value] 	= get_sse4fma_engine();
#endif
#if defined(COMPILE_SSE4)
				_engines[gorynych::capability_SSE4::value] 		= get_sse4_engine();
#endif
#if defined(COMPILE_SSSE3)
				_engines[gorynych::capability_SSSE3::value] 	= get_ssse3_engine();
#endif
#if defined(COMPILE_SSE3)
				_engines[gorynych::capability_SSE3::value] 		= get_sse3_engine();
#endif
#if defined(COMPILE_SSE2)
				_engines[gorynych::capability_SSE2::value] 		= get_sse2_engine();
#endif
#if defined(COMPILE_FPU)
				_engines[gorynych::capability_FPU::value] 		= get_fpu_engine();
#endif
		}

		// COmpile from json object
		void compile(const nlohmann::json &source)
		{
			settings << source["environment"]["scheduler"];
			auto environment = source["environment"];

			// No limits
			int feature_set = environment.value("max_feature_set", -1);

			// Restrict with extracted bitmask
			info.feature_flags &= feature_set;


			// For each valid branch
			SIMD_BUILD(info,
			{
				_engines[capability::value]->compile(source);
			});
		}

		float* run(const vec3<float> &origin)
		{
			SIMD_DISPATCH(info,
			{
				return _engines[capability::value]->run(origin);
			});

			return nullptr;
		}

		void run(const vec3<float> &origin, float *target)
		{
			SIMD_DISPATCH(info,
			{
				_engines[capability::value]->run(origin, target);
			});
		}
	private:
	};
}}