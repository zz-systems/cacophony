#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H


#include "constants.h"

namespace zzsystems { namespace paranoise { namespace interpolation {
	using namespace constants;
	// Cubic ===================================================================================================
	
	SIMD_ENABLE_F(vreal)
	inline vreal cerp(const vreal& n0, const vreal& n1, const vreal& n2, const vreal& n3, const vreal& a)
	{
		auto n0_n1 = n0 - n1;

		auto p = (n3 - n2) - n0_n1;
		auto q = n0_n1 - p;
		auto r = n2 - n0;

		auto a2 = a * a;
		auto a3 = a2 * a;

		auto qa2 = q * a2;

		return vfmadd(p, a3, qa2) + vfmadd(r, a, n1);
	}	

	// Linear ==================================================================================================
	
	SIMD_ENABLE_F(vreal)
	inline vreal lerp(const vreal& n0, const vreal& n1, const vreal& a)
	{
		// = n0 - a * n0 => -a * n0 + n0;
		auto n = (fastload<vreal>::_1() - a) * n0;
		//auto n = fmadd(-a, n0, n0);
		return vfmadd(a, n1, n);
		//return (1 - a) * n0 + a * n1;
	}
	// SCurve3 =================================================================================================

	SIMD_ENABLE_F(vreal)
	inline vreal scurve3(const vreal& a)
	{
		// 3a² - 2a³
		return (a * a * (fastload<vreal>::_3() - fastload<vreal>::_2() * a));
	}

	// SCurve5 =================================================================================================
	
	SIMD_ENABLE_F(vreal)
	inline vreal scurve5(const vreal& a)
	{
		auto a3 = a * a * a;
		auto a4 = a3 * a;
		auto a5 = a4 * a;

		auto q = fastload<vreal>::_10() * a3;
		auto p = vfmadd(fastload<vreal>::_15(), a4, q);
		
		//return (6 * a5) - (15 * a4) + (10 * a3);
		return vfmsub(fastload<vreal>::_6(), a5, p);
	}
}}};

#endif
