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


#include "vector.h"

namespace zzsystems { namespace math
{
	template<typename T>
	union mat3x2
	{
		vec3<T> v[2];
		struct {
			vec3<T> _0;
			vec3<T> _1;
		};

		mat3x2() = default;
		//vec3(const T* rhs) { v = rhs; };

		mat3x2(const mat3x2<T>& rhs) : _0(rhs._0), _1(rhs._1) {  };
		mat3x2(const vec3<T>& _0, const vec3<T>& _1) : _0(_0), _1(_1) {  };

		/*mat3x2(initializer_list<vec3<T>> list)
			: _0(list[0]),
			  _1(list[1])
		{}

		mat3x2(initializer_list<T> list)
			: _0(list[0], list[1], list[2]),
			  _1(list[3], list[4], list[5])
		{}*/
	};


	//template<typename T>
	//inline vec3<T>	operator +(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z); }
	//template<typename T>
	//inline vec3<T>	operator -(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z); }
	//template<typename T>
	//inline vec3<T>	operator *(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z); }
	//template<typename T>
	//inline vec3<T>	operator /(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z); }

	template<typename T>
	union mat3x3
	{
		vec3<T> v[3];
		struct {
			vec3<T> _0;
			vec3<T> _1;
			vec3<T> _2;
		};

		mat3x3() = default;
		//vec3(const T* rhs) { v = rhs; };

		mat3x3(const mat3x3<T>& rhs) : _0(rhs._0), _1(rhs._1), _2(rhs._2) {  };
		mat3x3(const vec3<T>& _0, const vec3<T>& _1, const vec3<T> &_2) : _0(_0), _1(_1), _2(_2) {  };

		/*mat3x3(initializer_list<vec3<T>> list)
			: _0(list[0]),
			  _1(list[1]),
			  _2(list[2])
		{}

		mat3x3(initializer_list<T> list)
			:	_0(list[0], list[1], list[2]),
				_1(list[3], list[4], list[5]),
				_2(list[6], list[7], list[8])
		{}*/
	};

	ANY(T) BIN_OP(+, mat3x3<T>)
	{
		return mat3x3<T>(a._0 + b._0, a._1 + b._1, a._2 + b._2);
	}

	ANY(T) BIN_OP(-, mat3x3<T>)
	{
		return mat3x3<T>(a._0 - b._0, a._1 - b._1, a._2 - b._2);
	}

	ANY(T) vec3<T> operator *(const mat3x3<T>& a, const vec3<T>& b)
	{
		return vec3<T>(dot(a._0, b), dot(a._1, b), dot(a._2, b)); 
	}	
}}