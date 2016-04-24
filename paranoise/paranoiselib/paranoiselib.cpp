// paranoiselib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "paranoiselib.h"
#include "../paranoise/parallel/all.h"

extern "C" {
	void compile_immediate(const char* content)
	{
		_engine.compile_imm_str(content);
	}

	void compile_file(const char* path)
	{
		_engine.compile_file(path);
	}

	float* run(const char* id, float origin_x, float origin_y, float origin_z, int* length)
	{
		auto result = _engine.run(zzsystems::math::vec3<float>(origin_x, origin_y, origin_z));

		auto d = _engine.settings.dimensions;

		*length = d.x * d.y * d.z;
		return result;
	}

	void run_in_place(const char*id, float* target, float origin_x, float origin_y, float origin_z)
	{
		_engine.run(target, zzsystems::math::vec3<float>(origin_x, origin_y, origin_z));
	}

	void calculate_splatmaps(float *heightmap, float* splatmap, size_t awidth, size_t aheight, size_t hwidth, size_t hheight)
	{
		using namespace zzsystems::simdal;
		using namespace zzsystems::paranoise;
		
		SIMD_DISPATCH(_engine.info, 
		{
			(calc_splatmap<capability, vreal, vint>(heightmap, splatmap, awidth, aheight, hwidth, hheight));
		});
	}
}