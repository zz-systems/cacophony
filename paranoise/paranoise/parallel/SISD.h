#pragma once
#ifndef INTRINSIC_H 
#define INTRINSIC_H

#include "../base.h"

namespace zzsystems { namespace simdal {

	// Load/Store ===============================================================================================
		
	inline int32_t* extract(int32_t& src)		{ return &src; }
	inline float* extract(float& src)		{ return &src; }
	inline double* extract(double& src)		{ return &src; }

	inline int vgather(const int* memloc, int index)
	{
		return memloc[index];
	}

	inline float vgather(const float* memloc, int index)
	{
		return memloc[index];
	}
}}

#endif 