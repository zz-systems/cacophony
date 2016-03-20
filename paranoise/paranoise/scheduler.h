#pragma once
#ifndef PARANOISE_SCHEDULER
#define PARANOISE_SCHEDULER

#include "basetypes.h"

#include <ppl.h>
#include <vector>
#include <fstream>
#include "parallel/all.h"

namespace paranoise { namespace scheduler {
	using namespace concurrency;
	using namespace x87compat;
	using namespace parallel;

	struct scheduler_settings
	{
		Vector3<int> dimensions;

		int seed;
		bool use_threads;

		scheduler_settings(Vector3<int> dimensions, int seed = 0, bool use_threads = true)
			: dimensions(dimensions), seed(seed), use_threads(use_threads)
		{}
	};

	SIMD_ENABLE_F(TReal)
	std::shared_ptr<std::vector<std::vector<std::vector<float>>>> schedule3D(const Module<TReal>& source, const Transformer<TReal>& transform, const scheduler_settings& settings)
	{
		int word = sizeof(TReal) >> 2;	
		auto d = settings.dimensions;

		auto result = new std::vector<std::vector<std::vector<float>>>();
		result->resize(d.z);

		for (auto z = 0; z < d.z; z++)
		{
			result->at(z).resize(d.y);
			for (auto y = 0; y < d.y; y++)
			{
				result->at(z).at(y).resize(d.x);
				for (auto x = 0; x < d.x / word; x++)
				{
					Vector3<TReal> coords;

					if (word == 4)
					{
						coords.x = TReal(x, x + 1, x + 2, x + 3);
						coords.y = (TReal) y;
						coords.z = (TReal) z;					
					}
					/*else if (word == 8)
					{
						coords = Vector3<TReal>{
									TReal(x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6, x + 7),
									TReal(y),
									TReal(z)
						};
					}*/

					auto chunk = source(transform(coords));
				
					for (int i = 0; i < word; i++)
					{
						result->at(z).at(y).at(x + i) = chunk.values[i];
					}
				}
			}
		}

		return result;
	}

	/*template<typename TReal>
	inline Vector3<TReal> build_coords(float x, float y)
	{
		return Vector3<TReal> {
			TReal(x),
			TReal(y),
			TReal(0.0f)
		};
	}*/
	
	//template<typename TReal, typename featuremask, enable_if_t<is_same<TReal, _float4>::value, TReal>>
	template<typename featuremask>
	inline Vector3<_float4> build_coords(_float4 x, _float4 y)
	{
		return Vector3<_float4>(
			x + _float4(0, 1, 2, 3),
			y,
			fastload<_float4>::_0()
		);
	}

	//template<typename TReal, typename featuremask, enable_if_t<is_same<TReal, _float8>::value, TReal>>
	template<typename featuremask>
	inline Vector3<_float8> build_coords(_float8 x, _float8 y)
	{
		return Vector3<_float8>(
			x + _float8(0, 1, 2, 3, 4, 5, 6, 7),
			y,
			fastload<_float8>::_0()
		);
	}
	
	//template<typename TReal, typename featuremask, enable_if_t<is_same<TReal, float>::value, TReal>>
	//template<>
	inline Vector3<float> build_coords(float x, float y)
	{
		return Vector3<float> {
			x,
			y,
			0
		};
	}
	using vector2D = std::vector<std::vector<float>>;

	/*template <typename TReal>
	inline void stream_result(float* stride, int x, const TReal &r);*/

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

	SIMD_ENABLE_F(TReal)
	auto schedule2D(const Module<TReal>& source, const Transformer<TReal>& transform, const scheduler_settings& settings)
	{		
		int word = dim<TReal>();
		auto d = settings.dimensions;
		auto result = make_shared<vector<float>>(d.x * d.y * d.z);					
		
		if (settings.use_threads)
		{
			parallel_for(0, d.y, [&](const auto y)
			{
				_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);				
								
				auto stride = &result->at(y * d.x);

				for (auto x = 0; x < d.x; x += word)
				{
					auto coords = transform(build_coords(
						static_cast<TReal>(x), 
						static_cast<TReal>(y)));

					auto r = source(coords);
					stream_result(stride, x, r);
				}
			});
		}
		else
		{
			_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

			for (int y = 0; y < d.y; y++)
			{
				auto stride = &result->at(y * d.x);

				for (auto x = 0; x < d.x; x += word)
				{
					auto coords = transform(build_coords(
						static_cast<TReal>(x), 
						static_cast<TReal>(y)));
					auto r = source(coords);
					stream_result(stride, x, r);
				}
			};
		}

		return result;
	}

	/*SIMD_ENABLE_F(TReal)
		inline schedule_thread_body(const std::shared_ptr<vector2D> &result, )
	{
		result->at(y).resize(d.x);
		for (auto x = 0; x < d.x / word; x++)
		{
			Vector3<TReal> coords = transform(build_coords<TReal>((float)x / d.x, (float)y / d.y));

			auto chunk = source(coords);

			for (int i = 0; i < word; i++)
			{
				result->at(y).at(x + i) = extract(chunk)[i];
			}
		}
	}*/
}}
#endif