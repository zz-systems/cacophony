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

	Vector3() = default;
	//Vector3(const T* rhs) { v = rhs; };
		
	Vector3(const Vector3<T>& rhs) { v = rhs.v; };
	Vector3(const T& all) : x(all), y(all), z(all) { };
	Vector3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) { };

	template<typename U>
	Vector3(const Vector3<U>& rhs) : x((T)rhs.x), y((T)rhs.y), z((T)rhs.z) { }
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
inline T dot(const Vector3<T>& a, const Vector3<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

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
inline Vector3<T>	operator *(const Matrix3x3<T>& a, const Vector3<T>& b)   { return Vector3<T>(dot(a._0, b), dot(a._1, b), dot(a_2, b)); }

#endif