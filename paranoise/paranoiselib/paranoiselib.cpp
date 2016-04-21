// paranoiselib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "paranoiselib.h"

static zzsystems::paranoise::engine _engine;

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

		*length = result->size();
		return &result->at(0);
	}
}