#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "basetypes.h"
#include "intrinsic.h"

namespace noisepp { namespace interpolation {
	using namespace intrinsic;

	// Cubic ===================================================================================================
	
	SIMD_ENABLE_F(TFloat)
	inline TFloat InterpolateCubic(const TFloat& n0, const TFloat& n1, const TFloat& n2, const TFloat& n3, const TFloat& a)
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
	
	SIMD_ENABLE_F(TFloat)
	inline TFloat InterpolateLinear(const TFloat& n0, const TFloat& n1, const TFloat& a)
	{
		return (1.0 - a) * n0 + a * n1;
	}
	// SCurve3 =================================================================================================

	SIMD_ENABLE_F(TFloat)
	inline TFloat SCurve3(const TFloat& a)
	{
		return (a * a * (3 - 2 * a));
	}

	// SCurve5 =================================================================================================
	
	SIMD_ENABLE_F(TFloat)
	inline TFloat SCurve5(const TFloat& a)
	{
		auto a3 = a * a * a;
		auto a4 = a3 * a;
		auto a5 = a4 * a;

		return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
	}
}};

#endif
