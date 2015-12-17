#pragma once
#ifndef BASETYPES_H
#define BASETYPES_H

namespace noisepp {
	typedef unsigned	int		uint;

	typedef unsigned	long long	uint64;
	typedef unsigned	int		uint32;
	typedef unsigned	short	uint16;
	typedef unsigned	char	uint8;

	typedef signed		long long	int64;
	typedef signed		int		int32;	
	typedef signed		short	int16;
	typedef signed		char	int8;

	template<typename T>	
	union Vector3D
	{
		T v[3];
		struct {
			T x;
			T y;
			T z;
		};

		Vector3D() = default;
		Vector3D(const T& x, const T& y, const T& z) : x(x), y(y), z(z) { };
	};	

	template<typename T>
	union Cube3D
	{
		Vector3D<T> v[2];
		struct {
			Vector3D<T> _0;
			Vector3D<T> _1;
		};
	};
};

#endif