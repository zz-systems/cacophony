#pragma once
#ifndef NOISEPP_NOISE_GENERATORS
#define NOISEPP_NOISE_GENERATORS

#include "interpolation.h"
#include "vectortable.h"
#include <iostream>
#include <functional>

#define NOISE_VERSION 2

namespace noisepp {	namespace generators {
	using namespace interpolation;

	enum Quality {
		Fast,
		Standard,
		Best
	};

	template<typename T>
	std::function<
		typename std::enable_if<std::is_function<T>::value, T>::type
	> make_function(T *t) {
		return{ t };
	}

	// These constants control certain parameters that all coherent-noise
	// functions require.
#if (NOISE_VERSION == 1)
	// Constants used by the original version of libnoise.
	// Because X_NOISE_GEN is not relatively prime to the other values, and
	// Z_NOISE_GEN is close to 256 (the number of random gradient vectors),
	// patterns show up in high-frequency coherent noise.
	const int X_NOISE_GEN = 1;
	const int Y_NOISE_GEN = 31337;
	const int Z_NOISE_GEN = 263;
	const int SEED_NOISE_GEN = 1013;
	const int SHIFT_NOISE_GEN = 13;
#else
	// Constants used by the current version of libnoise.
	const int X_NOISE_GEN = 1619;
	const int Y_NOISE_GEN = 31337;
	const int Z_NOISE_GEN = 6971;
	const int SEED_NOISE_GEN = 1013;
	const int SHIFT_NOISE_GEN = 8;
#endif
		
	// Create a unit-length cube aligned along an integer boundary.  This cube
	// surrounds the input point.
	template<typename FLOAT, typename INT>
	inline void construct_cube(const Vector3D<FLOAT>& c, Cube3D<INT> &cube)
	{		
		// Assuming that x > 0 yields either 0 or (+-)1, the result is ANDed with 1
		// => equivalent to (x > 0.0? (int)x: (int)x - 1)
		cube._0.x = INT(c.x) - INT((c.x > 0) & 1);
		cube._0.y = INT(c.y) - INT((c.y > 0) & 1);
		cube._0.z = INT(c.z) - INT((c.z > 0) & 1);

		cube._1.x = cube._0.x + 1;
		cube._1.y = cube._0.y + 1;
		cube._1.z = cube._0.z + 1;
	}

	template<typename FLOAT, typename INT>
	inline void map_coord_diff(	const Vector3D<FLOAT>& c,
								const Cube3D<INT> &cube,
								Quality quality,
								Vector3D<FLOAT>& s)
	{
		// Map the difference between the coordinates of the input value and the
		// coordinates of the cube's outer-lower-left vertex onto an S-curve.
		s.x = c.x - FLOAT(cube._0.x);
		s.y = c.y - FLOAT(cube._0.y);
		s.z = c.z - FLOAT(cube._0.z);

		switch (quality) {
		case Quality::Fast:
			break;
		case Quality::Standard:
			s.x = SCurve3(s.x);
			s.y = SCurve3(s.y);
			s.z = SCurve3(s.z);
			break;
		case Quality::Best:
			s.x = SCurve5(s.x);
			s.y = SCurve5(s.y);
			s.z = SCurve5(s.z);
			break;
		}
	}

	// Now calculate the noise values at each vertex of the cube.  To generate
	// the coherent-noise value at the input point, interpolate these eight
	// noise values using the S-curve value as the interpolant (trilinear
	// interpolation.)
	template<typename FLOAT, typename INT>
	inline FLOAT calc_noise(const Cube3D<INT> &cube,
							const Vector3D<FLOAT>& s,
							const INT& seed, 
		std::function<FLOAT(const Vector3D<INT>& c, const INT& seed)> noisegen)
	{		
		FLOAT n0, n1, ix0, ix1, iy0, iy1;

		n0 = noisegen(Vector3D<INT>(cube._0.x, cube._0.y, cube._0.z), seed);
		n1 = noisegen(Vector3D<INT>(cube._1.x, cube._0.y, cube._0.z), seed);

		ix0 = InterpolateLinear(n0, n1, s.x);

		n0 = noisegen(Vector3D<INT>(cube._0.x, cube._1.y, cube._0.z), seed);
		n1 = noisegen(Vector3D<INT>(cube._1.x, cube._1.y, cube._0.z), seed);

		ix1 = InterpolateLinear(n0, n1, s.x);
		iy0 = InterpolateLinear(ix0, ix1, s.y);

		n0 = noisegen(Vector3D<INT>(cube._0.x, cube._0.y, cube._1.z), seed);
		n1 = noisegen(Vector3D<INT>(cube._1.x, cube._0.y, cube._1.z), seed);

		ix0 = InterpolateLinear(n0, n1, s.x);

		n0 = noisegen(Vector3D<INT>(cube._0.x, cube._1.y, cube._1.z), seed);
		n1 = noisegen(Vector3D<INT>(cube._1.x, cube._1.y, cube._1.z), seed);

		ix1 = InterpolateLinear(n0, n1, s.y);
		iy1 = InterpolateLinear(ix0, ix1, s.y);

		return InterpolateLinear(iy0, iy1, s.z);
	}

	template<typename FLOAT, typename INT>
	FLOAT GradientNoise3D(	const Vector3D<FLOAT>& input,
							const Vector3D<INT>& nearby, 
							const INT& seed)
	{
		auto vectorIndices = (X_NOISE_GEN * nearby.x + Y_NOISE_GEN * nearby.y + Z_NOISE_GEN * nearby.z + SEED_NOISE_GEN * seed);
	
		if(std::is_integral<INT>::value)
			vectorIndices = vectorIndices & 0xffffffff;
		vectorIndices = vectorIndices ^ (vectorIndices >> SHIFT_NOISE_GEN);

		vectorIndices = (vectorIndices & 0xFF);

		vectorIndices = vectorIndices << 2;

		FLOAT xvGradient, yvGradient, zvGradient;
		auto	_xvGradient = extract(xvGradient),
				_yvGradient = extract(yvGradient),
				_zvGradient = extract(zvGradient);

		auto _vi = extract(vectorIndices);

		for (int i = 0; i < elem_count(xvGradient); i++)
		{
			_xvGradient[i] = RandomVectors[_vi[i]];
			_yvGradient[i] = RandomVectors[_vi[i] + 1];
			_zvGradient[i] = RandomVectors[_vi[i] + 2];
		}
		
		FLOAT	xvPoint = input.x - FLOAT(nearby.x),
				yvPoint = input.y - FLOAT(nearby.y),
				zvPoint = input.z - FLOAT(nearby.z);

		return ((xvGradient * xvPoint)
			+ (yvGradient * yvPoint)
			+ (zvGradient * zvPoint)) * 2.12;
	}

	template<typename FLOAT, typename INT>
	FLOAT GradientCoherentNoise3D(	const Vector3D<FLOAT>& c,
									const INT& seed, 
									Quality noiseQuality)
	{		
		Cube3D<INT> cube;
		construct_cube<FLOAT, INT>(c, cube);

		Vector3D<FLOAT> s;
		map_coord_diff<FLOAT, INT>(c, cube, noiseQuality, s);
		
		return calc_noise<FLOAT, INT>(cube, s, seed,
			[&](const Vector3D<INT>& s, const INT& seed) { return GradientNoise3D(c, s, seed); });
	}	

	template<typename INT>
	INT IntValueNoise3D(const Vector3D<INT>& c, const INT& seed)
	{
		INT n = (
			X_NOISE_GEN		 * c.x
			+ Y_NOISE_GEN    * c.y
			+ Z_NOISE_GEN    * c.z
			+ SEED_NOISE_GEN * seed)
			& 0x7fffffff;

		n = (n >> 13) ^ n;

		return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	}

	template<typename FLOAT, typename INT>
	FLOAT ValueNoise3D(const Vector3D<INT>& c, const INT& seed)
	{
		return 1.0 - (FLOAT(IntValueNoise3D(c, seed)) / 1073741824.0);
	}

	template<typename FLOAT, typename INT>
	FLOAT ValueCoherentNoise3D(const Vector3D<FLOAT>& c, const INT& seed, Quality noiseQuality)
	{
		Cube3D<INT> cube;
		construct_cube<FLOAT, INT>(c, cube);
		
		Vector3D<FLOAT> s;
		map_coord_diff<FLOAT, INT>(c, cube, noiseQuality, s);

		return calc_noise<FLOAT, INT>(cube, s, seed, make_function(ValueNoise3D<FLOAT, INT>));
	}
}}

#endif