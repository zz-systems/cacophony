#pragma once
#ifndef PARANOISE_SCHEDULER
#define PARANOISE_SCHEDULER

#include "basetypes.h"

#include <ppl.h>
#include <vector>

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

	SIMD_ENABLE_F(TReal)
		inline Vector3<TReal> build_coords(float x, float y)
	{
		return Vector3<TReal> {
			TReal(x, x + 1.0f, x + 2.0f, x + 3.0f),
			TReal(y),
			0
		};
	}

	template<>
	inline Vector3<float4> build_coords(float x, float y)
	{
		return Vector3<float4> {
			float4(x, x + 1.0f, x + 2.0f, x + 3.0f),
				float4(y),
				0
		};
	}

	template<>
	inline Vector3<float8> build_coords(float x, float y)
	{
		return Vector3<float8> {
			float8(x, x + 1.0f, x + 2.0f, x + 3.0f, x + 4.0f, x + 5.0f, x + 6.0f, x + 7.0f),
				float8(y),
				0
		};
	}

	/*if (word == 4)
	{
		float _x = x;

		coords.x = TReal(_x, _x + 1.0f, _x + 2.0f, _x + 3.0f);
		coords.y = (TReal)y;
	}*/
	/*else if (word == 8)
	{
	coords = Vector3<TReal>{
	TReal(x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6, x + 7),
	TReal(y),
	TReal(z)
	};
	}*/

	template<>
	inline Vector3<float> build_coords(float x, float y)
	{
		return Vector3<float> {
			x,
			y,
			0
		};
	}
	using vector2D = std::vector<std::vector<float>>;

	SIMD_ENABLE_F(TReal)
	auto schedule2D(const Module<TReal>& source, const Transformer<TReal>& transform, const scheduler_settings& settings)
	{
		
		int word = sizeof(TReal) >> 2;

		auto d = settings.dimensions;

		auto result = std::make_shared<vector2D>();

		result->resize(d.y);
		if (settings.use_threads)
		{
			//for (auto y = 0; y < d.y; y++)			
			parallel_for(0, d.y, [&](auto y)
			{
				result->at(y).resize(d.x);
				for (auto x = 0; x < d.x; x += word)
				{
					Vector3<TReal> coords = transform(build_coords<TReal>(x, y));

					auto chunk = source(coords);
					auto r = extract(chunk);

					for (int i = 0; i < word; i++)
					{
						result->at(y).at(x + i) = r[i];
					}
				}
			}
			);
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