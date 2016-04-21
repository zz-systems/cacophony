#pragma once
 
#include "stdafx.h"

#include "../paranoise/lib/json.hpp"
#include "../paranoise/parallel/all.h"
#include "../paranoise/engine/all.h"


#ifdef PARANOISELIB_EXPORTS
#define PARANOISEDLL_API __declspec(dllexport) 
#else
#define PARANOISEDLL_API __declspec(dllimport) 
#endif


	
extern "C" {

	PARANOISEDLL_API void compile_immediate(const char* content);
	PARANOISEDLL_API void compile_file(const char* path);

	PARANOISEDLL_API float* run(const char*id, float origin_x, float origin_y, float origin_z, int *length);
}
