#pragma once
 
#include "stdafx.h"

#include "../paranoise/lib/json.hpp"
#include "../paranoise/base.h"
#include "../paranoise/interpolation.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/engine/all.h"



#ifdef PARANOISELIB_EXPORTS
#define PARANOISEDLL_API __declspec(dllexport) 
#else
#define PARANOISEDLL_API __declspec(dllimport) 
#endif

static zzsystems::paranoise::engine _engine;
	
extern "C" {

	PARANOISEDLL_API void compile_immediate(const char* content);
	PARANOISEDLL_API void compile_file(const char* path);

	PARANOISEDLL_API float* run(const char*id, float origin_x, float origin_y, float origin_z, int *length);
	PARANOISEDLL_API void run_in_place(const char*id, float* target, float origin_x, float origin_y, float origin_z);

	PARANOISEDLL_API void calculate_splatmaps(float *heightmap, float* splatmap, size_t awidth, size_t aheight, size_t hwidth, size_t hheight);
}

template<typename capability, typename vreal, typename vint>
void calc_splatmap(float *heightmap, float* splatmap, int awidth, int aheight, int hwidth, int hheight)
{
	using namespace zzsystems::simdal;
	using namespace zzsystems::paranoise;
	using namespace zzsystems::math;

	auto get_points = [=](const vreal x, const vreal &y) -> vreal
	{
		auto index = static_cast<vint>(vround(vfmadd(static_cast<vreal>(hwidth), y, x)));
		return vgather(heightmap, index);
	};

	auto word = dim<vreal>();
	auto d = vec3<int>(awidth, aheight, 0);

	parallel_for(0, d.y, [&](const auto y)
	{
		_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
		//_MM_SET_DENORMALS_ZERO_MODE(_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);

		auto stride0 = splatmap + ((0 * d.y + y) * d.x);
		auto stride1 = splatmap + ((1 * d.y + y) * d.x);

		auto remainder = d.x % word;
		auto adjust = remainder > 0 ? word - remainder : 0;
		for (size_t x = 0; x < d.x; x += word)
		{
			auto coords = cpu_scheduler<vreal>::build_coords(
				static_cast<vreal>(static_cast<int>(x)),
				static_cast<vreal>(y),
				static_cast<vreal>(0));

			coords = vec3<vreal>(coords.x / (awidth - 1), coords.z / (aheight - 1), 0);

			auto xx = coords.x * hwidth, yy = coords.y * hheight;
			auto rx = vround(xx),
				 ry = vround(yy);

			// 0 0 0  (-1, -1) (0, -1) (1, -1)
			// 0 x 0  (-1, 0)  (0, 0)  (1, 0)	
			// 0 0 0  (-1, 1)  (0, 1)  (1, 1)

			auto c = get_points(xx, yy);
			auto	n = get_points(xx, yy - 1),
				s = get_points(xx, yy + 1),
				w = get_points(xx - 1, yy),
				e = get_points(xx + 1, yy);

			auto xgrad = lerp(vsel(xx < rx, w, e), c, rx - xx);
			auto ygrad = lerp(vsel(yy < ry, n, s), c, ry - yy);

			auto combined = (xgrad + ygrad) / 2;
			auto normalized = vclamp(combined / 1.5f, fastload<vreal>::_0(), fastload<vreal>::_1());

			cpu_scheduler<vreal>::write_result(d, stride0, remainder, x, normalized);
			cpu_scheduler<vreal>::write_result(d, stride1, remainder, x, 1.0f - normalized);
		}
	});

	/*cpu_scheduler<vreal> scheduler;
	scheduler.settings = _engine.settings;
	scheduler.settings.dimensions = { awidth, 0, aheight };
	

	scheduler(splatmap, { 0, 0, 0 });*/

	//var splatMap = new float[terrainData.alphamapResolution, terrainData.alphamapResolution, 2];

	//for (var zRes = 0; zRes < terrainData.alphamapHeight; zRes++)
	//{
	//    for (var xRes = 0; xRes < terrainData.alphamapWidth; xRes++)
	//    {
	//        var normalizedX = (float)xRes / (terrainData.alphamapWidth - 1);
	//        var normalizedZ = (float)zRes / (terrainData.alphamapHeight - 1);

	//        var steepness = terrainData.GetSteepness(normalizedX, normalizedZ);
	//        var steepnessNormalized = Mathf.Clamp(steepness / 1.5f, 0, 1f);

	//        splatMap[zRes, xRes, 0] = 1f - steepnessNormalized;
	//        splatMap[zRes, xRes, 1] = steepnessNormalized;
	//    }
	//}	
}
