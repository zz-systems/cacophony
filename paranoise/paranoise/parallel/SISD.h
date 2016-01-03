#pragma once
#ifndef INTRINSIC_H 
#define INTRINSIC_H

#include "base.h"
#include "../basetypes.h"

namespace paranoise {	namespace parallel {

	// Load/Store ===============================================================================================
		
	inline int32* extract(int32& src)		{ return &src; }
	inline float* extract(float& src)		{ return &src; }
	inline double* extract(double& src)		{ return &src; }

	// Sizes ====================================================================================================

	inline size_t dim(const int32& val) { return 1; }
	inline size_t dim(const float& val) { return 1; }
	inline size_t dim(const double& val) { return 1; }
}}

#endif 