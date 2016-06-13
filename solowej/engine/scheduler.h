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


#include "../../dependencies/asyncplusplus/include/async++.h"

#include <vector>
#include <memory>
#include "../../dependencies/gorynych/gorynych/gorynych.h"
#include "../util/serializable.h"
#include "../modules/module_base.h"
#include "scheduler_base.h"

namespace zzsystems { namespace solowej {
	using namespace async;
	using namespace gorynych;
	using namespace math;
	using namespace modules;



	VECTORIZED_F
	class cpu_scheduler
		: public serializable<json>
	{
	public:
		scheduler_settings	settings;
		Module<vreal>		source;
		Transformer<vreal>	transform;

		cpu_scheduler() : source(nullptr)//, transform(nullptr)
		{}

		cpu_scheduler(const Module<vreal> &source, const Transformer<vreal> &transformer)
			: source(source)//, //transform(transformer)
		{}

		cpu_scheduler(const Module<vreal> &source, const Transformer<vreal> &transformer, const scheduler_settings &settings)
			: settings(settings), source(source)//, //transform(transformer)
		{}

		cpu_scheduler(const Module<vreal> &source, const Transformer<vreal> &transformer, vec3<int> dimensions, bool use_threads = true)
			: settings(dimensions, use_threads), source(source)//, //transform(transformer)
		{}		

		const json& operator<<(const json& source) override
		{
			settings << source;
			transform = build_transform(transform);
			
			return source;
		}

		void operator()(float* target, const vec3<float> &origin) const
		{
			if(settings.use_threads)
				schedule_mt(target, origin);
			else
				schedule_st(target, origin);
		}

		auto operator()(const vec3<float> &origin) const
		{
			auto d = this->settings.dimensions;
			//auto target = new float[d.x * d.y * d.z];

			gorynych::aligned_allocator<float, 32> alloc;

			//float* target;// = alloc.allocate(d.x * d.y * d.z);
			float* target = reinterpret_cast<float*>(aligned_malloc(sizeof(float) * d.x * d.y * d.z, 64));
			this->operator()(target, origin);

			return target;
		}
	private:
		static void write_result(const vec3<int> &d, float* stride, size_t remainder, size_t x, const vreal &r)
		{
			if (remainder == 0) // All rows aligned on 32-bit boundaries -> stream
			{
				stream_result(stride, x, r);
			}
			else if (x < (d.x - remainder)) // Not aligned - but still in the "good" range
			{
				store_result(stride, x, r);
			}
			else // Fill remaining columns
			{
				float extracted[dim<vreal>()];
				extract(r, extracted);

				for (size_t i = 0; i < remainder; i++)
					stride[x + i] = extracted[i];
			}
		}

		ANY(dispatch_mask)
			static inline vec3<_float4> build_coords(const _float4 & x, const _float4 & y, const _float4 & z)
		{
			return vec3<_float4>(
				x + _float4(0, 1, 2, 3),
				y,
				z
				);
		}

		ANY(dispatch_mask)
			static vec3<_float8> build_coords(const _float8 &x, const _float8 &y, const _float8 &z)
		{
			return vec3<_float8>(
				x + _float8(0, 1, 2, 3, 4, 5, 6, 7),
				y,
				z
				);
		}

		static vec3<float> build_coords(float x, float y, float z)
		{
			return vec3<float> {
					x,
					y,
					z
			};
		}

		Transformer<vreal> build_transform(Transformer<vreal> transformer)
		{
			cout << "building input transform..." << endl;


			cout << boolalpha << "has seam: " << settings.seamless << endl;


			if (static_cast<bool>(transformer))
			{
				if (settings.scale != vec3<float>(1) && settings.offset != vec3<float>(0))
				{
					cout << "user defined transformer: scaled, biased" << endl;
					return [this, transformer](const auto&c) { return transformer(c) / static_cast<vec3<vreal>>(settings.seam_dimensions) * static_cast<vec3<vreal>>(settings.scale) + static_cast<vec3<vreal>>(settings.offset); };
				}
				if (settings.scale != vec3<float>(1))
				{
					cout << "user defined transformer: scaled" << endl;
					return [this, transformer](const auto&c) { return transformer(c) / static_cast<vec3<vreal>>(settings.seam_dimensions) * static_cast<vec3<vreal>>(settings.scale); };
				}
				if (settings.offset != vec3<float>(0))
				{
					cout << "user defined transformer: biased" << endl;
					return [this, transformer](const auto&c) { return transformer(c) / static_cast<vec3<vreal>>(settings.seam_dimensions) + static_cast<vec3<vreal>>(settings.offset); };
				}
			}
			else
			{
				if (settings.scale != vec3<float>(1) && settings.offset != vec3<float>(0))
				{
					cout << "default transformer: scaled, biased" << endl;
					return [this](const auto&c) { return c / static_cast<vec3<vreal>>(settings.seam_dimensions) * static_cast<vec3<vreal>>(settings.scale) + static_cast<vec3<vreal>>(settings.offset); };
				}
				if (settings.scale != vec3<float>(1))
				{
					cout << "default transformer: scaled" << endl;
					return [this](const auto&c) { return c / static_cast<vec3<vreal>>(settings.seam_dimensions) * static_cast<vec3<vreal>>(settings.scale); };
				}
				if (settings.offset != vec3<float>(0))
				{
					cout << "default transformer: biased" << endl;
					return [this](const auto&c) { return c / static_cast<vec3<vreal>>(settings.seam_dimensions) + static_cast<vec3<vreal>>(settings.offset); };
				}
			}

			cout << "default transformer" << endl;
			return [this](const auto &c) { return c / static_cast<vec3<vreal>>(settings.seam_dimensions); };
		}



		static inline void stream_result(float* stride, size_t x, const float r)
		{
			stride[x] = r;
		}

		static inline void store_result(float* stride, size_t x, const float r)
		{
			stride[x] = r;
		}


//#if defined(COMPILE_SSE2) || defined(COMPILE_SSE3) || defined(COMPILE_SSE4) || defined(COMPILE_SSE4FMA)
		ANY(dispatch_mask)
			static inline void stream_result(float* stride, size_t x, const _float4 &r)
		{
			_mm_stream_ps(stride + x, r.val);
		}

		ANY(dispatch_mask)
		static inline void store_result(float* stride, size_t x, const _float4 &r)
		{
			_mm_storeu_ps(stride + x, r.val);
		}
//#endif
//#if defined(COMPILE_AVX1) || defined(COMPILE_AVX2)
		ANY(dispatch_mask)
			static void stream_result(float* stride, size_t x, const _float8 &r)
		{
			//_mm256_stream_ps(stride + x, r.val);
			_mm256_storeu_ps(stride + x, r.val);
		}

		ANY(dispatch_mask)
			static void store_result(float* stride, size_t x, const _float8 &r)
		{
			_mm256_storeu_ps(stride + x, r.val);
		}
//#endif
		void schedule_st(float *target, const vec3<float> &origin) const
		{
			size_t word		= dim<vreal>();
			auto d			= this->settings.dimensions;
			auto remainder	= d.x % word;
			auto adjust		= remainder > 0 ? word - remainder : 0;
			
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
			_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

//#pragma omp parallel for
			for (int z = 0; z < d.z; z++)
			{
				auto depth = z * d.y;
//#pragma omp parallel for
				for (int y = 0; y < d.y; y++)
				{
					auto stride = target + (depth + y) * d.x;
					
					for (size_t x = 0; x < d.x + adjust; x += word)
					{
						auto coords = transform(build_coords(
							static_cast<vreal>(static_cast<int>(x)),
							static_cast<vreal>(y),
							static_cast<vreal>(z)));

						auto r = source(static_cast<vec3<vreal>>(origin) + coords);

						write_result(d, stride, remainder, x, r);
					}
				}
			}
		}	

		void schedule_mt(float *target, const vec3<float> &origin) const
		{
			size_t word = dim<vreal>();
			auto d = this->settings.dimensions;

			if(d.z >= d.y)
			{
				parallel_for(irange(0, d.z), [&](const int z) {
					_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
					_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
					auto depth = z * d.y;

					for(int y = 0; y < d.y; y++)
					{
						auto stride = target + ((depth + y) * d.x);
						auto remainder = d.x % word;
						auto adjust = remainder > 0 ? word - remainder : 0;
						for (size_t x = 0; x < d.x; x += word) {
							auto coords = transform(build_coords(
									static_cast<vreal>(static_cast<int>(x)),
									static_cast<vreal>(y),
									static_cast<vreal>(z)));


							auto r = source(static_cast<vec3<vreal>>(origin) + coords);

							write_result(d, stride, remainder, x, r);
						}
					}
				});
			}
			else
			{
				for(int z = 0; z < d.z; z++)
				{
					auto depth = z * d.y;

					parallel_for(irange(0, d.y), [&](const int y)
					{
						_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
						_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

						auto stride = target + ((depth + y) * d.x);
						auto remainder = d.x % word;
						auto adjust = remainder > 0 ? word - remainder : 0;
						for (size_t x = 0; x < d.x; x += word) {
							auto coords = transform(build_coords(
									static_cast<vreal>(static_cast<int>(x)),
									static_cast<vreal>(y),
									static_cast<vreal>(z)));


							auto r = source(static_cast<vec3<vreal>>(origin) + coords);

							write_result(d, stride, remainder, x, r);
						}
					});
				}
			}
		}

		auto schedule_ocl() const
		{
			int word	= dim<vreal>();
			auto d		= this->settings.dimensions;
			auto result = make_shared<vector<float>>(d.x * d.y * d.z);

			return result;
		}
	};	
}}