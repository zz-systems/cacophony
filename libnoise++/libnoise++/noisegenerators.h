#pragma once
#ifndef NOISEPP_NOISE_GENERATORS
#define NOISEPP_NOISE_GENERATORS

#include "interpolation.h"
#include "vectortable.h"

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

	template<typename SIMD_FLOAT, typename SIMD_INT>
	SIMD_FLOAT GradientNoise3D(SIMD_FLOAT fx, SIMD_FLOAT fy, SIMD_FLOAT fz, SIMD_INT ix, SIMD_INT iy, SIMD_INT iz, SIMD_INT seed)
	{
		constexpr SIMD_INT x_noise = ld1<SIMD_INT>(X_NOISE_GEN);
		constexpr SIMD_INT y_noise = ld1<SIMD_INT>(Y_NOISE_GEN);
		constexpr SIMD_INT z_noise = ld1<SIMD_INT>(Z_NOISE_GEN);
		constexpr SIMD_INT s_noise = ld1<SIMD_INT>(SEED_NOISE_GEN);

		constexpr SIMD_INT min_one	= ld1<SIMD_INT>(0xffffffff);
		constexpr SIMD_INT FF		= ld1<SIMD_INT>(0xFF);

		constexpr SIMD_FLOAT scale	= ld1<SIMD_FLOAT>(2.12);

		SIMD_INT vectorIndices = (x_noise * ix + y_noise * iy + z_noise * iz + s_noise * seed) & min_one;

		vectorIndices = ((vectorIndices ^ (vectorIndices >> SHIFT_NOISE_GEN)) & FF) << 2;

		SIMD_FLOAT xvGradient, yvGradient, zvGradient;

		int _vi = (int32*)vectorIndices;

		auto	_xvGradient = (double*)xvGradient,
				_yvGradient = (double*)yvGradient,
				_zvGradient = (double*)zvGradient;

		for (int i = 0; i < sizeof(SIMD_INT) / sizeof(int32); i++)
		{
			_xvGradient[i] = RandomVectors[_vi[i]];
			_yvGradient[i] = RandomVectors[_vi[i] + 1];
			_zvGradient[i] = RandomVectors[_vi[i] + 2];
		}

		SIMD_FLOAT	xvPoint = fx - cast<SIMD_FLOAT>(ix),
			yvPoint = fy - cast<SIMD_FLOAT>(iy),
			zvPoint = fz - cast<SIMD_FLOAT>(iz);

		return ((xvGradient * xvPoint)
			+ (yvGradient * yvPoint)
			+ (zvGradient * zvPoint)) * scale;
	}

	template<>
	double GradientNoise3D(double fx, double fy, double fz, int ix, int iy, int iz, int seed)
	{
		// Randomly generate a gradient vector given the integer coordinates of the
		// input value.  This implementation generates a random number and uses it
		// as an index into a normalized-vector lookup table.
		int vectorIndex = (
			X_NOISE_GEN    * ix
			+ Y_NOISE_GEN    * iy
			+ Z_NOISE_GEN    * iz
			+ SEED_NOISE_GEN * seed)
			& 0xffffffff;
		vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
		vectorIndex &= 0xff;

		double xvGradient = RandomVectors[(vectorIndex << 2)];
		double yvGradient = RandomVectors[(vectorIndex << 2) + 1];
		double zvGradient = RandomVectors[(vectorIndex << 2) + 2];

		// Set up us another vector equal to the distance between the two vectors
		// passed to this function.
		double xvPoint = (fx - (double)ix);
		double yvPoint = (fy - (double)iy);
		double zvPoint = (fz - (double)iz);

		// Now compute the dot product of the gradient vector with the distance
		// vector.  The resulting value is gradient noise.  Apply a scaling value
		// so that this noise value ranges from -1.0 to 1.0.
		return ((xvGradient * xvPoint)
			+ (yvGradient * yvPoint)
			+ (zvGradient * zvPoint)) * 2.12;
	}

	template<typename SIMD_FLOAT, typename SIMD_INT>
	SIMD_FLOAT GradientCoherentNoise3D(SIMD_FLOAT x, SIMD_FLOAT y, SIMD_FLOAT z, SIMD_INT seed, Quality noiseQuality)
	{
		/*constexpr*/ SIMD_FLOAT zero	= ld1<SIMD_FLOAT>(0.0);
		/*constexpr*/ SIMD_INT one		= ld1<SIMD_INT>(1.0);

		// TODO: template
		SIMD_INT x0 = cast<SIMD_INT>(x) + cast<SIMD_INT>(x > zero);
		SIMD_INT y0 = cast<SIMD_INT>(y) + cast<SIMD_INT>(y > zero);
		SIMD_INT z0 = cast<SIMD_INT>(z) + cast<SIMD_INT>(z > zero);
		
		SIMD_INT x1 = x0 + one;
		SIMD_INT y1 = y0 + one;
		SIMD_INT z1 = z0 + one;

		SIMD_FLOAT xs, ys, zs;

		switch (noiseQuality) {
		case Quality::Fast:
			xs = x - cast<SIMD_FLOAT>(x0);
			ys = y - cast<SIMD_FLOAT>(y0);
			zs = z - cast<SIMD_FLOAT>(z0);
			break;
		case Quality::Standard:
			xs = SCurve3(x - cast<SIMD_FLOAT>(x0));
			ys = SCurve3(y - cast<SIMD_FLOAT>(y0));
			zs = SCurve3(z - cast<SIMD_FLOAT>(z0));
			break;
		case Quality::Best:
			xs = SCurve5(x - cast<SIMD_FLOAT>(x0));
			ys = SCurve5(y - cast<SIMD_FLOAT>(y0));
			zs = SCurve5(z - cast<SIMD_FLOAT>(z0));
			break;
		}

		// Now calculate the noise values at each vertex of the cube.  To generate
		// the coherent-noise value at the input point, interpolate these eight
		// noise values using the S-curve value as the interpolant (trilinear
		// interpolation.)
		SIMD_FLOAT n0, n1, ix0, ix1, iy0, iy1;

		n0 = GradientNoise3D(x, y, z, x0, y0, z0, seed);
		n1 = GradientNoise3D(x, y, z, x1, y0, z0, seed);
		ix0 = InterpolateLinear(n0, n1, xs);
		n0 = GradientNoise3D(x, y, z, x0, y1, z0, seed);
		n1 = GradientNoise3D(x, y, z, x1, y1, z0, seed);
		ix1 = InterpolateLinear(n0, n1, xs);
		iy0 = InterpolateLinear(ix0, ix1, ys);
		n0 = GradientNoise3D(x, y, z, x0, y0, z1, seed);
		n1 = GradientNoise3D(x, y, z, x1, y0, z1, seed);
		ix0 = InterpolateLinear(n0, n1, xs);
		n0 = GradientNoise3D(x, y, z, x0, y1, z1, seed);
		n1 = GradientNoise3D(x, y, z, x1, y1, z1, seed);
		ix1 = InterpolateLinear(n0, n1, xs);
		iy1 = InterpolateLinear(ix0, ix1, ys);

		return InterpolateLinear(iy0, iy1, zs);
	}

	template<>
	double GradientCoherentNoise3D(double x, double y, double z, int seed, Quality noiseQuality)
	{
		// Create a unit-length cube aligned along an integer boundary.  This cube
		// surrounds the input point.
		int x0 = (x > 0.0 ? (int)x : (int)x - 1);
		int x1 = x0 + 1;
		int y0 = (y > 0.0 ? (int)y : (int)y - 1);
		int y1 = y0 + 1;
		int z0 = (z > 0.0 ? (int)z : (int)z - 1);
		int z1 = z0 + 1;

		// Map the difference between the coordinates of the input value and the
		// coordinates of the cube's outer-lower-left vertex onto an S-curve.
		double xs = 0, ys = 0, zs = 0;
		switch (noiseQuality) {
		case Quality::Fast:
			xs = (x - (double)x0);
			ys = (y - (double)y0);
			zs = (z - (double)z0);
			break;
		case Quality::Standard:
			xs = SCurve3(x - (double)x0);
			ys = SCurve3(y - (double)y0);
			zs = SCurve3(z - (double)z0);
			break;
		case Quality::Best:
			xs = SCurve5(x - (double)x0);
			ys = SCurve5(y - (double)y0);
			zs = SCurve5(z - (double)z0);
			break;
		}

		// Now calculate the noise values at each vertex of the cube.  To generate
		// the coherent-noise value at the input point, interpolate these eight
		// noise values using the S-curve value as the interpolant (trilinear
		// interpolation.)
		double n0, n1, ix0, ix1, iy0, iy1;
		n0 = GradientNoise3D(x, y, z, x0, y0, z0, seed);
		n1 = GradientNoise3D(x, y, z, x1, y0, z0, seed);
		ix0 = InterpolateLinear(n0, n1, xs);
		n0 = GradientNoise3D(x, y, z, x0, y1, z0, seed);
		n1 = GradientNoise3D(x, y, z, x1, y1, z0, seed);
		ix1 = InterpolateLinear(n0, n1, xs);
		iy0 = InterpolateLinear(ix0, ix1, ys);
		n0 = GradientNoise3D(x, y, z, x0, y0, z1, seed);
		n1 = GradientNoise3D(x, y, z, x1, y0, z1, seed);
		ix0 = InterpolateLinear(n0, n1, xs);
		n0 = GradientNoise3D(x, y, z, x0, y1, z1, seed);
		n1 = GradientNoise3D(x, y, z, x1, y1, z1, seed);
		ix1 = InterpolateLinear(n0, n1, xs);
		iy1 = InterpolateLinear(ix0, ix1, ys);

		return InterpolateLinear(iy0, iy1, zs);
	}

	

	template<typename SIMD_INT>
	SIMD_INT IntValueNoise3D(SIMD_INT x, SIMD_INT y, SIMD_INT z, SIMD_INT seed)
	{

	}

	template<>
	int IntValueNoise3D(int x, int y, int z, int seed)
	{

	}

	template<typename SIMD_FLOAT, typename SIMD_INT>
	SIMD_FLOAT ValueCoherentNoise3D(SIMD_FLOAT x, SIMD_FLOAT y, SIMD_FLOAT z, SIMD_INT seed, Quality noiseQuality)
	{

	}

	template<>
	double ValueCoherentNoise3D(double x, double y, double z, int seed, Quality noiseQuality)
	{

	}

	template<typename SIMD_INT>
	SIMD_INT ValueNoise3D(SIMD_INT x, SIMD_INT y, SIMD_INT z, SIMD_INT seed)
	{

	}

	template<>
	int ValueNoise3D(int x, int y, int z, int seed)
	{

	}

	template<typename SIMD_FLOAT>
	SIMD_FLOAT AdjustToInt32(SIMD_FLOAT n)
	{

	}
}}

#endif