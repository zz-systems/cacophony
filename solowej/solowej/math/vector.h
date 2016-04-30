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

namespace zzsystems { namespace math {
	using namespace gorynych;

	template<typename T>
	union vec3
	{
		T v[3];

		struct {
			T x;
			T y;
			T z;
		};

		vec3() = default;
		//vec3(const T* rhs) { v = rhs; };

		vec3(const vec3<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z)
		{};
		vec3(const T& all) : x(all), y(all), z(all)
		{ };
		vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) { };

		/*vec3(initializer_list<T> list) : x(list[0]), y(list[1]), z(list[2])
		{}*/

		template<typename U>
		vec3(const vec3<U>& rhs)
			: x(static_cast<T>(rhs.x))
			, y(static_cast<T>(rhs.y))
			, z(static_cast<T>(rhs.z))
		{ }

		explicit operator bool() const
		{
			return is_set(x) && is_set(y) && is_set(z);
		}
		
	};
	
	ANY(T) BIN_OP(+, vec3<T>)
	{
		BODY(vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z));
	}

	ANY(T) BIN_OP(-, vec3<T>)
	{
		BODY(vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z));
	}

	ANY(T) BIN_OP(*, vec3<T>)
	{
		BODY(vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z));
	}

	ANY(T) BIN_OP(/, vec3<T>)
	{
		BODY(vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z));
	}


	template<typename T, typename U> inline vec3<T> operator +(const vec3<T> &a, const U &b) { return a + static_cast<vec3<T>>(b); }
	template<typename T, typename U> inline vec3<T> operator +(const U &a, const vec3<T> &b) { return static_cast<vec3<T>>(a) + b; }
	template<typename T, typename U> inline vec3<T> operator -(const vec3<T> &a, const U &b) { return a - static_cast<vec3<T>>(b); }
	template<typename T, typename U> inline vec3<T> operator -(const U &a, const vec3<T> &b) { return static_cast<vec3<T>>(a) - b; }
	template<typename T, typename U> inline vec3<T> operator *(const vec3<T> &a, const U &b) { return a * static_cast<vec3<T>>(b); }
	template<typename T, typename U> inline vec3<T> operator *(const U &a, const vec3<T> &b) { return static_cast<vec3<T>>(a) * b; }
	template<typename T, typename U> inline vec3<T> operator /(const vec3<T> &a, const U &b) { return a / static_cast<vec3<T>>(b); }
	template<typename T, typename U> inline vec3<T> operator /(const U &a, const vec3<T> &b) { return static_cast<vec3<T>>(a) / b; }

	template<typename T> inline vec3<T> &operator +=(vec3<T>& a, const T& b) { return a = (a + b); }
	template<typename T> inline vec3<T> &operator -=(vec3<T>& a, const T& b) { return a = (a - b); }
	template<typename T> inline vec3<T> &operator *=(vec3<T>& a, const T& b) { return a = (a * b); }
	template<typename T> inline vec3<T> &operator /=(vec3<T>& a, const T& b) { return a = (a / b); }

	template<typename T, typename U> inline vec3<T> &operator +=(vec3<T>& a, const U& b) { return a = (a + b); }
	template<typename T, typename U> inline vec3<T> &operator -=(vec3<T>& a, const U& b) { return a = (a - b); }
	template<typename T, typename U> inline vec3<T> &operator *=(vec3<T>& a, const U& b) { return a = (a * b); }
	template<typename T, typename U> inline vec3<T> &operator /=(vec3<T>& a, const U& b) { return a = (a / b); }

	template<typename T>
	inline vec3<T>	operator <(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x < b.x, a.y < b.y, a.z < b.z); }
	template<typename T>
	inline vec3<T>	operator <=(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x <= b.x, a.y <= b.y, a.z <= b.z); }
	template<typename T>
	inline vec3<T>	operator >(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x > b.x, a.y > b.y, a.z > b.z); }
	template<typename T>
	inline vec3<T>	operator >=(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x >= b.x, a.y >= b.y, a.z >= b.z); }
	template<typename T>
	inline vec3<T>	operator ==(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x == b.x, a.y == b.y, a.z == b.z); }
	template<typename T>
	inline vec3<T>	operator !=(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x != b.x, a.y != b.y, a.z != b.z); }

	template<typename T>
	inline vec3<T>	operator &(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x < b.x, a.y < b.y, a.z < b.z); }
	template<typename T>
	inline vec3<T>	operator |(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x > b.x, a.y > b.y, a.z > b.z); }
	template<typename T>
	inline vec3<T>	operator ^(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x == b.x, a.y == b.y, a.z == b.z); }


	template<typename T>
	inline vec3<T>	&operator +=(vec3<T>& a, const vec3<T>& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
	template<typename T>
	inline vec3<T>	&operator -=(vec3<T>& a, const vec3<T>& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
	template<typename T>
	inline vec3<T>	&operator *=(vec3<T>& a, const vec3<T>& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; return a; }
	template<typename T>
	inline vec3<T>	&operator /=(vec3<T>& a, const vec3<T>& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; }


	template<typename T>
	inline vec3<T>	&operator &=(vec3<T>& a, const vec3<T>& b) { a.x &= b.x; a.y &= b.y; a.z &= b.z; return a; }
	template<typename T>
	inline vec3<T>	&operator |=(vec3<T>& a, const vec3<T>& b) { a.x |= b.x; a.y |= b.y; a.z |= b.z; return a; }
	template<typename T>
	inline vec3<T>	&operator ^=(vec3<T>& a, const vec3<T>& b) { a.x ^= b.x; a.y ^= b.y; a.z ^= b.z; return a; }

	template<typename T>
	inline T dot(const vec3<T>& a, const vec3<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; } // fmadd?
}
namespace gorynych
{
	using namespace math;

	ANY(T)
		UN_FUNC(clamp_int32, vec3<T>)
	{
		return vec3<T>(clamp_int32<T>(a.x), clamp_int32<T>(a.y), clamp_int32<T>(a.z));
	}

	ANY2(TCondition, TProcess)
	inline vec3<TProcess> vsel(const vec3<TCondition> &condition, const vec3<TProcess> &choice1, const vec3<TProcess> &choice2)
	{
		return vec3<TProcess>(
			vsel<TCondition, TProcess>(condition.x, choice1.x, choice2.x),
			vsel<TCondition, TProcess>(condition.y, choice1.y, choice2.y),
			vsel<TCondition, TProcess>(condition.z, choice1.z, choice2.z));
	}


	ANY2(T, U)
	UN_FUNC(vtrunc, vec3<T>)
	{
		BODY(vec3<T>(vtrunc<T, U>(a.x), vtrunc<T, U>(a.y), vtrunc<T, U>(a.z)));
	}
}}