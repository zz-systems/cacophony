#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

//#include "parallel/x87compat.h"
#include "basetypes.h"

namespace paranoise { namespace interpolation {
	//using namespace parallel;
	//using namespace x87compat;
	// Cubic ===================================================================================================
	
	SIMD_ENABLE_F(TReal)
	inline TReal cerp(const TReal& n0, const TReal& n1, const TReal& n2, const TReal& n3, const TReal& a)
	{
		auto n0_n1 = n0 - n1;

		auto p = (n3 - n2) - n0_n1;
		auto q = n0_n1 - p;
		auto r = n2 - n0;


		auto a² = a * a;
		auto a³ = a² * a;

		return (p * a³) + (q * a²) + (r * a) + n1;
	}	

	// Linear ==================================================================================================
	
	SIMD_ENABLE_F(TReal)
	inline TReal lerp(const TReal& n0, const TReal& n1, const TReal& a)
	{
		return (1 - a) * n0 + a * n1;
	}
	// SCurve3 =================================================================================================

	SIMD_ENABLE_F(TReal)
	inline TReal scurve3(const TReal& a)
	{
		return (a * a * (3 - 2 * a));
	}

	// SCurve5 =================================================================================================
	
	SIMD_ENABLE_F(TReal)
	inline TReal scurve5(const TReal& a)
	{
		auto a3 = a * a * a;
		auto a4 = a3 * a;
		auto a5 = a4 * a;

		return (6 * a5) - (15 * a4) + (10 * a3);
	}
}};

#endif
