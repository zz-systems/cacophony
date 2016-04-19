#pragma once
#ifndef PARANOISE_VECTOR_H
#define PARANOISE_VECTOR_H

#include "base.h"


namespace zzsystems { namespace math {
	using namespace simdal;

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
			return static_cast<bool>(x) && static_cast<bool>(y) && static_cast<bool>(z);
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
namespace simdal
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
}
}
#endif