#pragma once
#ifndef PARANOISE_SCHEDULER
#define PARANOISE_SCHEDULER


#include <ppl.h>
#include <vector>

#include "../base.h"
#include "../util/serializable.h"
#include "../parallel/all.h"
#include "../modules/module_base.h"

namespace zzsystems { namespace paranoise { namespace scheduler {
	using namespace concurrency;
	using namespace simdal;
	using namespace math;
	using namespace modules;

	struct scheduler_settings :
		serializable<json>
	{
		vec3<int> dimensions;
		vec3<float> scale, offset;
		bool use_threads;


		scheduler_settings() = default;

		scheduler_settings(vec3<int> dimensions, bool use_threads = true)
			: dimensions(dimensions), use_threads(use_threads)
		{}

		scheduler_settings(const scheduler_settings& rhs)
			: dimensions(rhs.dimensions), use_threads(rhs.use_threads)
		{}

		const json& operator<<(const json& source) override
		{
			auto dimensions = source["dimensions"];
			auto scale  = source["scale"];
			auto offset = source["offset"];

			this->dimensions = dimensions != nullptr && dimensions.size() >= 3
				? vec3<int>(dimensions[0], dimensions[1], dimensions[2])
				: vec3<int>(128, 128, 1);
			
			this->scale		 = scale != nullptr && scale.size() >= 3
				? vec3<float>(scale[0], scale[1], scale[2])
				: vec3<float>(1);
						
			this->offset = offset != nullptr && offset.size() >= 3
				? vec3<float>(offset[0], offset[1], offset[2])
				: vec3<float>(0); 

			use_threads = source.value("use_threads", true);

			return source;
		}
	};

	SIMD_ENABLE_F(vreal)
	class cpu_scheduler
		: serializable<json>
	{
	public:
		scheduler_settings	settings;
		Module<vreal>			source;
		Transformer<vreal>	transform;

		cpu_scheduler() : source(nullptr), transform(nullptr)
		{}

		cpu_scheduler(const Module<vreal> &source, const Transformer<vreal> &transformer)
			: source(source), transform(transformer)
		{}

		cpu_scheduler(const Module<vreal> &source, const Transformer<vreal> &transformer, const scheduler_settings &settings)
			: settings(settings), source(source), transform(transformer)
		{}

		cpu_scheduler(const Module<vreal> &source, const Transformer<vreal> &transformer, vec3<int> dimensions, bool use_threads = true)
			: settings(dimensions, use_threads), source(source), transform(transformer)
		{}		

		const json& operator<<(const json& source) override
		{
			settings << source;
			transform = build_transform(transform);
			
			return source;
		}

		auto operator()() const
		{
			if (settings.use_threads)
				return schedule_mt();
			else
				return schedule_st();
		}
	private:
		Transformer<vreal> build_transform(Transformer<vreal> transformer)
		{
			auto dimensions = static_cast<vec3<vreal>>(settings.dimensions);
			auto scale = static_cast<vec3<vreal>>(settings.scale);
			auto offset = static_cast<vec3<vreal>>(settings.offset);

			if (static_cast<bool>(transformer))
			{
				if (settings.scale != vec3<float>(1) && settings.offset != vec3<float>(0))
					return [=](const auto&c) { return transformer(c) / dimensions * scale + offset; };
				if (settings.scale != vec3<float>(1))
					return [=](const auto&c) { return transformer(c) / dimensions * scale; };
				if (settings.offset != vec3<float>(0))
					return [=](const auto&c) { return transformer(c) / dimensions + offset; };
			}
			else
			{
				if (settings.scale != vec3<float>(1) && settings.offset != vec3<float>(0))
					return [=](const auto&c) { return c / dimensions * scale + offset; };
				if (settings.scale != vec3<float>(1))
					return [=](const auto&c) { return c / dimensions * scale; };
				if (settings.offset != vec3<float>(0))
					return [=](const auto&c) { return c / dimensions + offset; };
			}

			return [=](const auto&c) { return c / dimensions; };
		}

		ANY(featuremask)
			static inline vec3<_float4> build_coords(const _float4 & x, const _float4 & y, const _float4 & z)
		{
			return vec3<_float4>(
				x + _float4(0, 1, 2, 3),
				y,
				z
				);
		}

		ANY(featuremask)
			static inline vec3<_float8> build_coords(const _float8 &x,const _float8 &y, const _float8 &z)
		{
			return vec3<_float8>(
				x + _float8(0, 1, 2, 3, 4, 5, 6, 7),
				y,
				z
				);
		}

		static inline vec3<float> build_coords(float x, float y, float z)
		{
			return vec3<float> {
				x,
				y,
				z
			};
		}

		ANY(featuremask)
			static inline void stream_result(float* stride, size_t x, const _float4 &r)
		{
			_mm_stream_ps(stride + x, r.val);
		}

		ANY(featuremask)
			static inline void stream_result(float* stride, size_t x, const _float8 &r)
		{
			_mm256_stream_ps(stride + x, r.val);
		}

		static inline void stream_result(float* stride, size_t x, const float r)
		{
			stride[x] = r;
		}
		
		auto schedule_st() const
		{
			size_t word = dim<vreal>();
			auto d = this->settings.dimensions;
			auto result = make_shared<vector<float>>(d.x * d.y * d.z);
			
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
			//_MM_SET_DENORMALS_ZERO_MODE(_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);
			for (int z = 0; z < d.z; z++)
			{
				auto depth = z * d.y;
				for (int y = 0; y < d.y; y++)
				{
					auto stride = &result->at((depth + y) * d.x);

					for (size_t x = 0; x < d.x; x += word)
					{
						auto coords = transform(build_coords(
							static_cast<vreal>(static_cast<int>(x)),
							static_cast<vreal>(y),
							static_cast<vreal>(z)));

						auto r = source(coords);
						stream_result(stride, x, r);
					}
				}
			}
			
			return result;
		}

		auto schedule_mt() const
		{
			size_t word = dim<vreal>();
			auto d = this->settings.dimensions;
			auto result = make_shared<vector<float>>(d.x * d.y * d.z);

			parallel_for(0, d.z, [&](const auto z)
			{
				auto depth = z * d.y;

				parallel_for(0, d.y, [&](const auto y)
				{
					_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
					//_MM_SET_DENORMALS_ZERO_MODE(_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);

					auto stride = &result->at((depth + y) * d.x);

					for (size_t x = 0; x < d.x; x += word)
					{
						auto coords = transform(build_coords(
							static_cast<vreal>(static_cast<int>(x)),
							static_cast<vreal>(y),
							static_cast<vreal>(z)));

						
						auto r = source(coords);
						stream_result(stride, x, r);
					}
				});
			});		

			return result;
		}

		auto schedule_ocl() const
		{
			int word	= dim<vreal>();
			auto d		= this->settings.dimensions;
			auto result = make_shared<vector<float>>(d.x * d.y * d.z);

			return result;
		}
	};	
}}}
#endif