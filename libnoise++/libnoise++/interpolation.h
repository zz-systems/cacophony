#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "basetypes.h"
#include "intrinsic.h"

namespace noisepp { namespace interpolation {
	using namespace intrinsic;

	// Cubic ===================================================================================================
	
	template<typename T>
	inline T InterpolateCubic(T n0, T n1, T n2, T n3, T a)
	{
		T n0_n1 = n0 - n1;

		T p = (n3 - n2) - n0_n1;
		T q = n0_n1 - p;
		T r = n2 - n0;


		T a² = a * a;
		T a³ = a² * a;

		return (p * a³) + (q * a²) + (r * a) + n1;
	}	

	template<>
	inline double InterpolateCubic(double n0, double n1, double n2, double n3, double a)
	{
		double p = (n3 - n2) - (n0 - n1);
		double q = (n0 - n1) - p;
		double r = n2 - n0;
		double s = n1;
		return p * a * a * a + q * a * a + r * a + s;
	}

	// Linear ==================================================================================================
	
	template<typename T>
	inline T InterpolateLinear(T n0, T n1, T a)
	{
		constexpr T one = ld1<T>(1.0);

		return (one - a) * n0 + a * n1;
	}

	template<>
	inline double InterpolateLinear(double n0, double n1, double a)
	{
		return ((1.0 - a) * n0) + (a * n1);
	}
	// SCurve3 =================================================================================================

	template<typename T> 
	inline T SCurve3(T a)
	{
		constexpr T two		= ld1<T>(2.0);
		constexpr T three	= ld1<T>(3.0);

		return (a * a * (three - two * a));
	}

	template<>
	inline double SCurve3(double a)
	{
		return (a * a * (3.0 - 2.0 * a));
	}

	// SCurve5 =================================================================================================
	
	template<typename T>
	inline T SCurve5(T a)
	{
		constexpr T six		= ld1<T>(6);
		constexpr T ten		= ld1<T>(10);
		constexpr T fifteen = ld1<T>(15);

		T a3 = a * a * a;
		T a4 = a3 * a;
		T a5 = a4 * a;

		return (six * a5) - (fifteen * a4) + (ten * a3);
	}

	template<>
	inline double SCurve5(double a)
	{
		double a3 = a * a * a;
		double a4 = a3 * a;
		double a5 = a4 * a;
		return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
	}
}};

#endif
