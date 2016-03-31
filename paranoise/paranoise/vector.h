#pragma once
#ifndef PARANOISE_VECTOR_H
#define PARANOISE_VECTOR_H

#include "base.h"


namespace zzsystems { namespace paranoise { namespace util {
	using namespace simdal;

	template<typename T>
	struct Vector3
	{
		union {
			T v[3];

			struct {
				T x;
				T y;
				T z;
			};
		};

		inline Vector3() = default;
		//Vector3(const T* rhs) { v = rhs; };

		inline Vector3(const Vector3<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) 
		{};
		inline Vector3(const T& all) : x(all), y(all), z(all)
		{ };
		inline Vector3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) { };

		template<typename U>
		inline Vector3(const Vector3<U>& rhs) 
			: x(static_cast<T>(rhs.x))
			, y(static_cast<T>(rhs.y))
			, z(static_cast<T>(rhs.z))
		{ }

		explicit inline operator bool() const
		{
			return static_cast<bool>(x) && static_cast<bool>(y) && static_cast<bool>(z);
		}
		
		BIN_OP_STUB(+, Vector3<T>, T)
		BIN_OP_STUB(-, Vector3<T>, T)
		BIN_OP_STUB(*, Vector3<T>, T)
		BIN_OP_STUB(/, Vector3<T>, T)		
	};

	/*ANY2(TCondition, TProcess)
		inline Vector3<TProcess> vsel(const Vector3<TCondition> &condition, const Vector3<TProcess> &choice1, const Vector3<TProcess> &choice2)
	{
		return Vector3<TProcess>(
			simdal::vsel<TCondition, TProcess>(condition.x, choice1.x, choice2.x),
			simdal::vsel<TCondition, TProcess>(condition.y, choice1.y, choice2.y),
			simdal::vsel<TCondition, TProcess>(condition.z, choice1.z, choice2.z));
	}*/

	ANY2(T, U)
		UN_FUNC(vtrunc, Vector3<T>)
	{
		BODY(Vector3<T>(vtrunc<T, U>(a.x), vtrunc<T, U>(a.y), vtrunc<T, U>(a.z)));
	}

	ANY(T)
		UN_FUNC(clamp_int32, Vector3<T>)
	{
		return Vector3<T>(simdal::clamp_int32<T>(a.x), simdal::clamp_int32<T>(a.y), simdal::clamp_int32<T>(a.z));
	}

	ANY(T) BIN_OP(+, Vector3<T>)
	{
		BODY(Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z));
	}

	ANY(T) BIN_OP(-, Vector3<T>)
	{
		BODY(Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z));
	}

	ANY(T) BIN_OP(*, Vector3<T>)
	{
		BODY(Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z));
	}

	ANY(T) BIN_OP(/, Vector3<T>)
	{
		BODY(Vector3<T>(a.x / b.x, a.y / b.y, a.z / b.z));
	}

	template<typename T> inline Vector3<T> &operator +=(Vector3<T>& a, const T& b) { a = (a + b); return a; }
	template<typename T> inline Vector3<T> &operator -=(Vector3<T>& a, const T& b) { a = (a - b); return a; }
	template<typename T> inline Vector3<T> &operator *=(Vector3<T>& a, const T& b) { a = (a * b); return a; }
	template<typename T> inline Vector3<T> &operator /=(Vector3<T>& a, const T& b) { a = (a / b); return a; }

	template<typename T>
	inline Vector3<T>	operator <(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x < b.x, a.y < b.y, a.z < b.z); }
	template<typename T>
	inline Vector3<T>	operator <=(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x <= b.x, a.y <= b.y, a.z <= b.z); }
	template<typename T>
	inline Vector3<T>	operator >(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x > b.x, a.y > b.y, a.z > b.z); }
	template<typename T>
	inline Vector3<T>	operator >=(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x >= b.x, a.y >= b.y, a.z >= b.z); }
	template<typename T>
	inline Vector3<T>	operator ==(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x == b.x, a.y == b.y, a.z == b.z); }
	template<typename T>
	inline Vector3<T>	operator !=(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x != b.x, a.y != b.y, a.z != b.z); }

	template<typename T>
	inline Vector3<T>	operator &(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x < b.x, a.y < b.y, a.z < b.z); }
	template<typename T>
	inline Vector3<T>	operator |(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x > b.x, a.y > b.y, a.z > b.z); }
	template<typename T>
	inline Vector3<T>	operator ^(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x == b.x, a.y == b.y, a.z == b.z); }


	template<typename T>
	inline Vector3<T>	&operator +=(Vector3<T>& a, const Vector3<T>& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
	template<typename T>
	inline Vector3<T>	&operator -=(Vector3<T>& a, const Vector3<T>& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
	template<typename T>
	inline Vector3<T>	&operator *=(Vector3<T>& a, const Vector3<T>& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; return a; }
	template<typename T>
	inline Vector3<T>	&operator /=(Vector3<T>& a, const Vector3<T>& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; }


	template<typename T>
	inline Vector3<T>	&operator &=(Vector3<T>& a, const Vector3<T>& b) { a.x &= b.x; a.y &= b.y; a.z &= b.z; return a; }
	template<typename T>
	inline Vector3<T>	&operator |=(Vector3<T>& a, const Vector3<T>& b) { a.x |= b.x; a.y |= b.y; a.z |= b.z; return a; }
	template<typename T>
	inline Vector3<T>	&operator ^=(Vector3<T>& a, const Vector3<T>& b) { a.x ^= b.x; a.y ^= b.y; a.z ^= b.z; return a; }

	template<typename T>
	inline T dot(const Vector3<T>& a, const Vector3<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; } // fmadd?

	

	template<typename T>
	union Matrix3x2
	{
		Vector3<T> v[2];
		struct {
			Vector3<T> _0;
			Vector3<T> _1;
		};

		inline Matrix3x2() = default;
		//Vector3(const T* rhs) { v = rhs; };

		inline Matrix3x2(const Matrix3x2<T>& rhs) : _0(rhs._0), _1(rhs._1) {  };
		inline Matrix3x2(const Vector3<T>& _0, const Vector3<T>& _1) : _0(_0), _1(_1) {  };
	};


	//template<typename T>
	//inline Vector3<T>	operator +(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z); }
	//template<typename T>
	//inline Vector3<T>	operator -(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z); }
	//template<typename T>
	//inline Vector3<T>	operator *(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z); }
	//template<typename T>
	//inline Vector3<T>	operator /(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x / b.x, a.y / b.y, a.z / b.z); }

	template<typename T>
	union Matrix3x3
	{
		Vector3<T> v[3];
		struct {
			Vector3<T> _0;
			Vector3<T> _1;
			Vector3<T> _2;
		};
	};

	template<typename T>
	inline Matrix3x3<T>	operator +(const Matrix3x3<T>& a, const Matrix3x3<T>& b) { return Vector3<T>(a._0 + b._0, a._1 + b._1, a._2 + b._2); }
	template<typename T>
	inline Matrix3x3<T>	operator -(const Matrix3x3<T>& a, const Matrix3x3<T>& b) { return Vector3<T>(a._0 - b._0, a._1 - b._1, a._2 - b._2); }
	template<typename T>
	inline Vector3<T>	operator *(const Matrix3x3<T>& a, const Vector3<T>& b) { return Vector3<T>(dot(a._0, b), dot(a._1, b), dot(a._2, b)); }
}}}
#endif