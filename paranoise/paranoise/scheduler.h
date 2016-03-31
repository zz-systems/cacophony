#pragma once
#ifndef PARANOISE_SCHEDULER
#define PARANOISE_SCHEDULER

#include "base.h"

#include <ppl.h>
#include <vector>
#include <fstream>
#include "parallel/all.h"
#include "modules/module_base.h"

namespace zzsystems { namespace paranoise { namespace scheduler {
	using namespace concurrency;
	using namespace simdal;
	using namespace util;
	using namespace modules;

	struct scheduler_settings
	{
		Vector3<int> dimensions;

		int seed;
		bool use_threads;

		scheduler_settings(Vector3<int> dimensions, int seed = 0, bool use_threads = true)
			: dimensions(dimensions), seed(seed), use_threads(use_threads)
		{}
	};


	//template<typename TReal, typename featuremask, enable_if_t<is_same<TReal, _float4>::value, TReal>>
	template<typename featuremask>
	inline Vector3<_float4> build_coords(_float4 x, _float4 y, _float4 z)
	{
		return Vector3<_float4>(
			x + _float4(0, 1, 2, 3),
			y,
			z
			);
	}

	//template<typename TReal, typename featuremask, enable_if_t<is_same<TReal, _float8>::value, TReal>>
	template<typename featuremask>
	inline Vector3<_float8> build_coords(_float8 x, _float8 y, _float8 z)
	{
		return Vector3<_float8>(
			x + _float8(0, 1, 2, 3, 4, 5, 6, 7),
			y,
			z
			);
	}

	//template<typename TReal, typename featuremask, enable_if_t<is_same<TReal, float>::value, TReal>>
	//template<>
	inline Vector3<float> build_coords(float x, float y, float z)
	{
		return Vector3<float> {
			x,
			y,
			z
		};
	}

	SIMD_ENABLE_F(TReal)
	auto schedule(const Module<TReal>& source, const Transformer<TReal>& transform, const scheduler_settings& settings)
	{
		int word = dim<TReal>();
		auto d = settings.dimensions;
		auto result = make_shared<vector<float>>(d.x * d.y * d.z);

		if (settings.use_threads)
		{
			parallel_for(0, d.z, [&](const auto z)
			{
				auto depth = z * d.y;

				parallel_for(0, d.y, [&](const auto y)
				{
					_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);					

					auto stride = &result->at((depth + y) * d.x);

					for (auto x = 0; x < d.x; x += word)
					{
						auto coords = transform(build_coords(
							static_cast<TReal>(x),
							static_cast<TReal>(y),
							static_cast<TReal>(z)));

						auto r = source(coords);
						stream_result(stride, x, r);
					}
				});
			});
		}
		else
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			for (int z = 0; z < d.z; z++)
			{
				auto depth = z * d.y;
				for (int y = 0; y < d.y; y++)
				{
					auto stride = &result->at((depth + y) * d.x);

					for (auto x = 0; x < d.x; x += word)
					{
						auto coords = transform(build_coords(
							static_cast<TReal>(x),
							static_cast<TReal>(y), 
							static_cast<TReal>(z)));

						auto r = source(coords);
						stream_result(stride, x, r);
					}
				}
			}
		}

		return result;
	}

		

	template <typename featuremask>
	inline void stream_result(float* stride, int x, const _float4 &r)
	{
		_mm_stream_ps(stride + x, r.val);
	}

	template <typename featuremask>
	inline void stream_result(float* stride, int x, const _float8 &r)
	{
		_mm256_stream_ps(stride + x, r.val);
	}

	//template <>
	inline void stream_result(float* stride, int x, const float &r)
	{
		stride[x] = r;
	}
}}}
#endif