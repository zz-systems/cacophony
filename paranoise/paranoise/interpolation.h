#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

//#include "parallel/x87compat.h"
#include "basetypes.h"
#include "constants.h"

namespace paranoise { namespace interpolation {
	using namespace constants;
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

		auto qa² = q * a²;

		return vfmadd(p, a³, qa²) + vfmadd(r, a, n1);
	}	

	// Linear ==================================================================================================
	
	SIMD_ENABLE_F(TReal)
	inline TReal lerp(const TReal& n0, const TReal& n1, const TReal& a)
	{
		// = n0 - a * n0 => -a * n0 + n0;
		auto n = (fastload<TReal>::_1() - a) * n0;
		//auto n = fmadd(-a, n0, n0);
		return vfmadd(a, n1, n);
		//return (1 - a) * n0 + a * n1;
	}
	// SCurve3 =================================================================================================

	SIMD_ENABLE_F(TReal)
	inline TReal scurve3(const TReal& a)
	{
		// 3a² - 2a³
		return (a * a * (fastload<TReal>::_3() - fastload<TReal>::_2() * a));
	}

	// SCurve5 =================================================================================================
	
	SIMD_ENABLE_F(TReal)
	inline TReal scurve5(const TReal& a)
	{
		auto a3 = a * a * a;
		auto a4 = a3 * a;
		auto a5 = a4 * a;

		auto q = fastload<TReal>::_10() * a3;
		auto p = vfmadd(fastload<TReal>::_15(), a4, q);
		
		//return (6 * a5) - (15 * a4) + (10 * a3);
		return vfmsub(fastload<TReal>::_6(), a5, p);
	}
}};

#endif
