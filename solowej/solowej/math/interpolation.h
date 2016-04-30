//
// Vectorized and parallelized version of libnoise using the gorynych SIMD toolkit
// Copyright (C) 2015-2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Original libnoise: 
// Copyright (C) 2003, 2004 Jason Bevins
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "../../submodules/gorynych/gorynych/gorynych.h"

namespace zzsystems { namespace solowej { namespace math {
	using namespace gorynych;
	// Cubic ===================================================================================================
	
	SIMD_ENABLED_F
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
	
	SIMD_ENABLED_F
	inline vreal lerp(const vreal& n0, const vreal& n1, const vreal& a)
	{
		// = n0 - a * n0 => -a * n0 + n0;
		auto n = (cfl<vreal>::_1() - a) * n0;
		//auto n = fmadd(-a, n0, n0);
		return vfmadd(a, n1, n);
		//return (1 - a) * n0 + a * n1;
	}
	// SCurve3 =================================================================================================

	SIMD_ENABLED_F
	inline vreal scurve3(const vreal& a)
	{
		// 3a² - 2a³
		return (a * a * (cfl<vreal>::_3() - cfl<vreal>::_2() * a));
	}

	// SCurve5 =================================================================================================
	
	SIMD_ENABLED_F
	inline vreal scurve5(const vreal& a)
	{
		auto a3 = a * a * a;
		auto a4 = a3 * a;
		auto a5 = a4 * a;

		auto q = cfl<vreal>::_10() * a3;
		auto p = vfmadd(cfl<vreal>::_15(), a4, q);
		
		//return (6 * a5) - (15 * a4) + (10 * a3);
		return vfmsub(cfl<vreal>::_6(), a5, p);
	}
}}};