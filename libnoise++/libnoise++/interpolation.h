#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "basetypes.h"
#include "intrinsic.h"

namespace noisepp { namespace interpolation {
	using namespace intrinsic;

	// Cubic ===================================================================================================
	
	template<typename T>
	inline T InterpolateCubic(const T& n0, const T& n1, const T& n2, const T& n3, const T& a)
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
	inline double InterpolateCubic(const double& n0, const double& n1, const double& n2, const double& n3, const double& a)
	{
		double p = (n3 - n2) - (n0 - n1);
		double q = (n0 - n1) - p;
		double r = n2 - n0;
		double s = n1;
		return p * a * a * a + q * a * a + r * a + s;
	}

	// Linear ==================================================================================================
	
	template<typename T>
	inline T InterpolateLinear(const T& n0, const T& n1, const T& a)
	{
		return (1.0 - a) * n0 + a * n1;
	}
	// SCurve3 =================================================================================================

	template<typename T> 
	inline T SCurve3(const T& a)
	{
		return (a * a * (3 - 2 * a));
	}

	// SCurve5 =================================================================================================
	
	template<typename T>
	inline T SCurve5(const T& a)
	{
		T a3 = a * a * a;
		T a4 = a3 * a;
		T a5 = a4 * a;

		return (T(6) * a5) - (T(15) * a4) + (T(10) * a3);
	}

	template<>
	inline double SCurve5(const double& a)
	{
		double a3 = a * a * a;
		double a4 = a3 * a;
		double a5 = a4 * a;
		return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
	}
}};

#endif
