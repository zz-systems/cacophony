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
	SIMD_ENABLE(TFloat, TInt)
	inline void construct_cube(const Vector3<TFloat>& c, Matrix3x2<TInt> &cube)
	{		
		// Assuming that x > 0 yields either 0 or (+-)1, the result is ANDed with 1
		// => equivalent to (x > 0.0? (int)x: (int)x - 1)
		cube._0.x = TInt(c.x) - TInt((c.x > 0) & 1);
		cube._0.y = TInt(c.y) - TInt((c.y > 0) & 1);
		cube._0.z = TInt(c.z) - TInt((c.z > 0) & 1);

		cube._1.x = cube._0.x + 1;
		cube._1.y = cube._0.y + 1;
		cube._1.z = cube._0.z + 1;
	}

	SIMD_ENABLE(TFloat, TInt)
	inline void map_coord_diff(	const Vector3<TFloat>& c,
								const Matrix3x2<TInt> &cube,
								Quality quality,
								Vector3<TFloat>& s)
	{
		// Map the difference between the coordinates of the input value and the
		// coordinates of the cube's outer-lower-left vertex onto an S-curve.
		s.x = c.x - TFloat(cube._0.x);
		s.y = c.y - TFloat(cube._0.y);
		s.z = c.z - TFloat(cube._0.z);

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
	SIMD_ENABLE(TFloat, TInt)
	inline TFloat calc_noise(const Matrix3x2<TInt> &cube,
							const Vector3<TFloat>& s,
							const TInt& seed, 
		std::function<TFloat(const Vector3<TInt>& c, const TInt& seed)> noisegen)
	{		
		TFloat n0, n1, ix0, ix1, iy0, iy1;

		n0 = noisegen(Vector3<TInt>(cube._0.x, cube._0.y, cube._0.z), seed);
		n1 = noisegen(Vector3<TInt>(cube._1.x, cube._0.y, cube._0.z), seed);

		ix0 = InterpolateLinear(n0, n1, s.x);

		n0 = noisegen(Vector3<TInt>(cube._0.x, cube._1.y, cube._0.z), seed);
		n1 = noisegen(Vector3<TInt>(cube._1.x, cube._1.y, cube._0.z), seed);

		ix1 = InterpolateLinear(n0, n1, s.x);
		iy0 = InterpolateLinear(ix0, ix1, s.y);

		n0 = noisegen(Vector3<TInt>(cube._0.x, cube._0.y, cube._1.z), seed);
		n1 = noisegen(Vector3<TInt>(cube._1.x, cube._0.y, cube._1.z), seed);

		ix0 = InterpolateLinear(n0, n1, s.x);

		n0 = noisegen(Vector3<TInt>(cube._0.x, cube._1.y, cube._1.z), seed);
		n1 = noisegen(Vector3<TInt>(cube._1.x, cube._1.y, cube._1.z), seed);

		ix1 = InterpolateLinear(n0, n1, s.y);
		iy1 = InterpolateLinear(ix0, ix1, s.y);

		return InterpolateLinear(iy0, iy1, s.z);
	}

	SIMD_ENABLE(TFloat, TInt)
	TFloat GradientNoise3D(	const Vector3<TFloat>& input,
							const Vector3<TInt>& nearby, 
							const TInt& seed)
	{
		auto vectorIndices = (X_NOISE_GEN * nearby.x + Y_NOISE_GEN * nearby.y + Z_NOISE_GEN * nearby.z + SEED_NOISE_GEN * seed);
	
		if(std::is_integral<TInt>::value)
			vectorIndices = vectorIndices & 0xffffffff;

		vectorIndices = vectorIndices ^ (vectorIndices >> SHIFT_NOISE_GEN);

		vectorIndices = (vectorIndices & 0xFF);

		vectorIndices = vectorIndices << 2;

		TFloat xvGradient, yvGradient, zvGradient;
		auto	_xvGradient = extract(xvGradient),
				_yvGradient = extract(yvGradient),
				_zvGradient = extract(zvGradient);

		auto _vi = extract(vectorIndices);

		for (size_t i = 0; i < elem_count(xvGradient); i++)
		{
			_xvGradient[i] = RandomVectors[_vi[i]];
			_yvGradient[i] = RandomVectors[_vi[i] + 1];
			_zvGradient[i] = RandomVectors[_vi[i] + 2];
		}
		
		TFloat	xvPoint = input.x - TFloat(nearby.x),
				yvPoint = input.y - TFloat(nearby.y),
				zvPoint = input.z - TFloat(nearby.z);

		return ((xvGradient * xvPoint)
			+ (yvGradient * yvPoint)
			+ (zvGradient * zvPoint)) * 2.12;
	}

	SIMD_ENABLE(TFloat, TInt)
	TFloat GradientCoherentNoise3D(	const Vector3<TFloat>& c,
									const TInt& seed, 
									Quality noiseQuality)
	{		
		Matrix3x2<TInt> cube;
		construct_cube<TFloat, TInt>(c, cube);

		Vector3<TFloat> s;
		map_coord_diff<TFloat, TInt>(c, cube, noiseQuality, s);
		
		return calc_noise<TFloat, TInt>(cube, s, seed,
			[&](const Vector3<TInt>& s, const TInt& seed) { return GradientNoise3D(c, s, seed); });
	}	

	SIMD_ENABLE_I(TInt)
	TInt IntValueNoise3D(const Vector3<TInt>& c, const TInt& seed)
	{
		TInt n = (
			X_NOISE_GEN		 * c.x
			+ Y_NOISE_GEN    * c.y
			+ Z_NOISE_GEN    * c.z
			+ SEED_NOISE_GEN * seed)
			& 0x7fffffff;

		n = (n >> 13) ^ n;

		return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	}

	SIMD_ENABLE(TFloat, TInt)
	TFloat ValueNoise3D(const Vector3<TInt>& c, const TInt& seed)
	{
		return 1.0 - (TFloat(IntValueNoise3D(c, seed)) / 1073741824.0);
	}

	SIMD_ENABLE(TFloat, TInt)
	TFloat ValueCoherentNoise3D(const Vector3<TFloat>& c, const TInt& seed, Quality noiseQuality)
	{
		Matrix3x2<TInt> cube;
		construct_cube<TFloat, TInt>(c, cube);
		
		Vector3<TFloat> s;
		map_coord_diff<TFloat, TInt>(c, cube, noiseQuality, s);

		return calc_noise<TFloat, TInt>(cube, s, seed, make_function(ValueNoise3D<TFloat, TInt>));
	}
}}

#endif