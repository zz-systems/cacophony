#pragma once
#ifndef PARANOISE_VECTOR_H
#define PARANOISE_VECTOR_H

template<typename T>
union Vector3
{
	T v[3];
	struct {
		T x;
		T y;
		T z;
	};

	inline Vector3() = default;
	//Vector3(const T* rhs) { v = rhs; };
		
	inline Vector3(const Vector3<T>& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; };
	inline Vector3(const T& all) : x(all), y(all), z(all) { };
	inline Vector3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) { };

	template<typename U>
	inline Vector3(const Vector3<U>& rhs) : x((T)rhs.x), y((T)rhs.y), z((T)rhs.z) { }
};

template<typename T>
inline Vector3<T>	operator +(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z); }
template<typename T>
inline Vector3<T>	operator -(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z); }
template<typename T>
inline Vector3<T>	operator *(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z); }
template<typename T>
inline Vector3<T>	operator /(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(a.x / b.x, a.y / b.y, a.z / b.z); }

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
inline T dot(const Vector3<T>& a, const Vector3<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

template<typename T, typename U>
inline Vector3<T> truncate(const Vector3<T> &val) { return Vector3<T>(truncate<T, U>(val.x), truncate<T, U>(val.y), truncate<T, U>(val.z)); }

template<typename T>
inline Vector3<T> clamp_int32(const Vector3<T> &val) { return Vector3<T>(clamp_int32<T>(val.x), clamp_int32<T>(val.y), clamp_int32<T>(val.z)); }

template<typename T>
union Matrix3x2
{
	Vector3<T> v[2];
	struct {
		Vector3<T> _0;
		Vector3<T> _1;
	};
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
inline Vector3<T>	operator *(const Matrix3x3<T>& a, const Vector3<T>& b)   { return Vector3<T>(dot(a._0, b), dot(a._1, b), dot(a._2, b)); }

#endif