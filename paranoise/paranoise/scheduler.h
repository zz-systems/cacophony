#pragma once
#ifndef PARANOISE_SCHEDULER
#define PARANOISE_SCHEDULER

#include "basetypes.h"

#include <ppl.h>
#include <vector>

namespace paranoise { namespace scheduler {

	struct scheduler_settings
	{
		Vector3<int> dimensions;
		int seed;
		bool use_threads = true;
	};

	SIMD_ENABLE_F(TReal)
	std::vector<std::vector<std::vector<float>>> *schedule(const Module<TReal>& source, const Transformer<TReal>& transform, const scheduler_settings& settings)
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
}}
#endif