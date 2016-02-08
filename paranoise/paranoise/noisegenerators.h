#pragma once
#ifndef PARANOISE_NOISE_GENERATORS
#define PARANOISE_NOISE_GENERATORS

//#include "parallel/x87compat.h"

#include "interpolation.h"
#include "vectortable.h"
#include "vector.h"

#include <iostream>
#include <functional>

#define NOISE_VERSION 2

namespace paranoise {	namespace generators {
	using namespace interpolation;
	//using namespace x87compat;

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
		
	namespace detail {
		// Create a unit-length cube aligned along an integer boundary.  This cube
		// surrounds the input point.
		SIMD_ENABLE(TReal, TInt)
		inline void construct_cube(const Vector3<TReal>& c, Matrix3x2<TInt> &cube)
		{		
			// Assuming that x > 0 yields either 0 or (+-)1, the result is ANDed with 1
			// => equivalent to (x > 0.0? (int)x: (int)x - 1)
			
			cube._0.x = (TInt)c.x - sel(c.x > 0, ZERO<TInt>(), ONE<TInt>());
			cube._0.y = (TInt)c.y - sel(c.y > 0, ZERO<TInt>(), ONE<TInt>());
			cube._0.z = (TInt)c.z - sel(c.z > 0, ZERO<TInt>(), ONE<TInt>());

			cube._1.x = cube._0.x + 1;
			cube._1.y = cube._0.y + 1;
			cube._1.z = cube._0.z + 1;
		}

		SIMD_ENABLE(TReal, TInt)
		inline void map_coord_diff(	const Vector3<TReal>& c,
									const Matrix3x2<TInt> &cube,
									Quality quality,
									Vector3<TReal>& s)
		{
			// Map the difference between the coordinates of the input value and the
			// coordinates of the cube's outer-lower-left vertex onto an S-curve.
			s.x = c.x - TReal(cube._0.x);
			s.y = c.y - TReal(cube._0.y);
			s.z = c.z - TReal(cube._0.z);

			/*auto best = quality == Quality::Best;
			auto fast = quality == Quality::Standard;

			s.x = sel(best, scurve5(s.x), sel(fast, scurve3(s.x), s.x));
			s.y = sel(best, scurve5(s.y), sel(fast, scurve3(s.y), s.y));
			s.z = sel(best, scurve5(s.z), sel(fast, scurve3(s.z), s.z));*/

			switch (quality) {
			case Quality::Fast:
				break;
			case Quality::Standard:
				s.x = scurve3(s.x);
				s.y = scurve3(s.y);
				s.z = scurve3(s.z);
				break;
			case Quality::Best:
				s.x = scurve5(s.x);
				s.y = scurve5(s.y);
				s.z = scurve5(s.z);
				break;
			}
		}		
	}
	/*namespace original
	{
		float _ValueNoise3D(int x, int y, int z, int seed);
		float _ValueCoherentNoise3D(double x, double y, double z, int seed,
			Quality noiseQuality);
		int _IntValueNoise3D(int x, int y, int z, int seed);
		float _GradientNoise3D(float fx, float fy, float fz, int ix,
			int iy, int iz, int seed);
		float _GradientCoherentNoise3D(float x, float y, float z, int seed, Quality noiseQuality);
		float _GradientCoherentNoise3D(float x, float y, float z, int seed, Quality noiseQuality)
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
			float xs = 0, ys = 0, zs = 0;
			switch (noiseQuality) {
			case Quality::Fast:
				xs = (x - (float)x0);
				ys = (y - (float)y0);
				zs = (z - (float)z0);
				break;
			case Quality::Standard:
				xs = scurve3(x - (float)x0);
				ys = scurve3(y - (float)y0);
				zs = scurve3(z - (float)z0);
				break;
			case Quality::Best:
				xs = scurve5(x - (float)x0);
				ys = scurve5(y - (float)y0);
				zs = scurve5(z - (float)z0);
				break;
			}

			// Now calculate the noise values at each vertex of the cube.  To generate
			// the coherent-noise value at the input point, interpolate these eight
			// noise values using the S-curve value as the interpolant (trilinear
			// interpolation.)
			float n0, n1, ix0, ix1, iy0, iy1;
			n0 = _GradientNoise3D(x, y, z, x0, y0, z0, seed);
			n1 = _GradientNoise3D(x, y, z, x1, y0, z0, seed);
			ix0 = lerp(n0, n1, xs);
			n0 = _GradientNoise3D(x, y, z, x0, y1, z0, seed);
			n1 = _GradientNoise3D(x, y, z, x1, y1, z0, seed);
			ix1 = lerp(n0, n1, xs);
			iy0 = lerp(ix0, ix1, ys);
			n0 = _GradientNoise3D(x, y, z, x0, y0, z1, seed);
			n1 = _GradientNoise3D(x, y, z, x1, y0, z1, seed);
			ix0 = lerp(n0, n1, xs);
			n0 = _GradientNoise3D(x, y, z, x0, y1, z1, seed);
			n1 = _GradientNoise3D(x, y, z, x1, y1, z1, seed);
			ix1 = lerp(n0, n1, xs);
			iy1 = lerp(ix0, ix1, ys);

			return lerp(iy0, iy1, zs);
		}

		float _GradientNoise3D(float fx, float fy, float fz, int ix,
			int iy, int iz, int seed)
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

			float xvGradient = RandomVectors[(vectorIndex << 2)];
			float yvGradient = RandomVectors[(vectorIndex << 2) + 1];
			float zvGradient = RandomVectors[(vectorIndex << 2) + 2];

			// Set up us another vector equal to the distance between the two vectors
			// passed to this function.
			float xvPoint = (fx - (float)ix);
			float yvPoint = (fy - (float)iy);
			float zvPoint = (fz - (float)iz);

			// Now compute the dot product of the gradient vector with the distance
			// vector.  The resulting value is gradient noise.  Apply a scaling value
			// so that this noise value ranges from -1.0 to 1.0.
			return ((xvGradient * xvPoint)
				+ (yvGradient * yvPoint)
				+ (zvGradient * zvPoint)) * 2.12;
		}

		int _IntValueNoise3D(int x, int y, int z, int seed)
		{
			// All constants are primes and must remain prime in order for this noise
			// function to work correctly.
			int n = (
				X_NOISE_GEN    * x
				+ Y_NOISE_GEN    * y
				+ Z_NOISE_GEN    * z
				+ SEED_NOISE_GEN * seed)
				& 0x7fffffff;
			n = (n >> 13) ^ n;
			//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
			return (n * (n * n * 1087 + 2749) + 3433) & 0x7fffffff;
		}

		float _ValueCoherentNoise3D(double x, double y, double z, int seed,
			Quality noiseQuality)
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
			float xs = 0, ys = 0, zs = 0;
			switch (noiseQuality) {
			case Quality::Fast:
				xs = (x - (float)x0);
				ys = (y - (float)y0);
				zs = (z - (float)z0);
				break;
			case Quality::Standard:
				xs = scurve3(x - (float)x0);
				ys = scurve3(y - (float)y0);
				zs = scurve3(z - (float)z0);
				break;
			case Quality::Best:
				xs = scurve5(x - (float)x0);
				ys = scurve5(y - (float)y0);
				zs = scurve5(z - (float)z0);
				break;
			}

			// Now calculate the noise values at each vertex of the cube.  To generate
			// the coherent-noise value at the input point, interpolate these eight
			// noise values using the S-curve value as the interpolant (trilinear
			// interpolation.)
			float n0, n1, ix0, ix1, iy0, iy1;
			n0 = _ValueNoise3D(x0, y0, z0, seed);
			n1 = _ValueNoise3D(x1, y0, z0, seed);
			ix0 = lerp(n0, n1, xs);
			n0 = _ValueNoise3D(x0, y1, z0, seed);
			n1 = _ValueNoise3D(x1, y1, z0, seed);
			ix1 = lerp(n0, n1, xs);
			iy0 = lerp(ix0, ix1, ys);
			n0 = _ValueNoise3D(x0, y0, z1, seed);
			n1 = _ValueNoise3D(x1, y0, z1, seed);
			ix0 = lerp(n0, n1, xs);
			n0 = _ValueNoise3D(x0, y1, z1, seed);
			n1 = _ValueNoise3D(x1, y1, z1, seed);
			ix1 = lerp(n0, n1, xs);
			iy1 = lerp(ix0, ix1, ys);
			return lerp(iy0, iy1, zs);
		}

		float _ValueNoise3D(int x, int y, int z, int seed)
		{
			return 1.0 - ((float)_IntValueNoise3D(x, y, z, seed) / 1073741824.0);
		}
	}*/
	SIMD_ENABLE(TReal, TInt)
	inline TReal GradientNoise3D(	const Vector3<TReal>& input,
							const Vector3<TInt>& nearby, 
							const TInt& seed)
	{
		auto vi = (X_NOISE_GEN * nearby.x + Y_NOISE_GEN * nearby.y + Z_NOISE_GEN * nearby.z + SEED_NOISE_GEN * seed);
	
		vi &= (TInt) 0xffffffff;
		vi ^= (vi >> SHIFT_NOISE_GEN);
		vi &= (TInt) 0xFF;
		vi <<= 2;

		Vector3<TReal> grad;
		Vector3<float*> vgrad = { extract(grad.x), extract(grad.y), extract(grad.z) };

		auto vvi = extract(vi);

		for (size_t i = 0; i < dim<TReal>(); i++)
		{
			vgrad.x[i] = RandomVectors[vvi[i]];
			vgrad.y[i] = RandomVectors[vvi[i] + 1];
			vgrad.z[i] = RandomVectors[vvi[i] + 2];
		}
		
		
		return dot(grad, input - (Vector3<TReal>)nearby) * 2.12f;
	}

	SIMD_ENABLE(TReal, TInt)
	inline TReal GradientCoherentNoise3D(	const Vector3<TReal>& c,
									const TInt& seed, 
									Quality quality)
	{		
		Matrix3x2<TInt> cube;
		detail::construct_cube(c, cube);

		Vector3<TReal> s;
		detail::map_coord_diff<TReal, TInt>(c, cube, quality, s);	

		// Now calculate the noise values at each vertex of the cube.  To generate
		// the coherent-noise value at the input point, interpolate these eight
		// noise values using the S-curve value as the interpolant (trilinear
		// interpolation.)
		TReal n0, n1, ix0, ix1, iy0, iy1;

		n0 = GradientNoise3D(c, { cube._0.x, cube._0.y, cube._0.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._0.y, cube._0.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = GradientNoise3D(c, { cube._0.x, cube._1.y, cube._0.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._1.y, cube._0.z }, seed);

		ix1 = lerp(n0, n1, s.x);
		iy0 = lerp(ix0, ix1, s.y);

		n0 = GradientNoise3D(c, { cube._0.x, cube._0.y, cube._1.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._0.y, cube._1.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = GradientNoise3D(c, { cube._0.x, cube._1.y, cube._1.z }, seed);
		n1 = GradientNoise3D(c, { cube._1.x, cube._1.y, cube._1.z }, seed);

		ix1 = lerp(n0, n1, s.x);
		iy1 = lerp(ix0, ix1, s.y);

		return lerp(iy0, iy1, s.z);
	}	

	SIMD_ENABLE_I(TInt)
	inline TInt IntValueNoise3D(const Vector3<TInt>& c, const TInt& seed)
	{
		TInt n = (
			X_NOISE_GEN		 * c.x
			+ Y_NOISE_GEN    * c.y
			+ Z_NOISE_GEN    * c.z
			+ SEED_NOISE_GEN * seed)
			& 0x7fffffff;

		n = (n >> 13) ^ n;

		//return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
		return (n * (n * n * 1087 + 2749) + 3433) & 0x7fffffff;
	}

	SIMD_ENABLE(TReal, TInt)
	inline TReal ValueNoise3D(const Vector3<TInt>& c, const TInt& seed)
	{
		return ONE<TReal>() - (TReal(IntValueNoise3D(c, seed)) / 1073741824.0f);
	}

	SIMD_ENABLE(TReal, TInt)
	inline TReal ValueCoherentNoise3D(const Vector3<TReal>& c, const TInt& seed, Quality quality)
	{
		Matrix3x2<TInt> cube;
		detail::construct_cube(c, cube);

		Vector3<TReal> s;
		detail::map_coord_diff<TReal, TInt>(c, cube, quality, s);

		TReal n0, n1, ix0, ix1, iy0, iy1;

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._0.y, cube._0.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._0.y, cube._0.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._1.y, cube._0.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._1.y, cube._0.z }, seed);

		ix1 = lerp(n0, n1, s.x);
		iy0 = lerp(ix0, ix1, s.y);

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._0.y, cube._1.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._0.y, cube._1.z }, seed);

		ix0 = lerp(n0, n1, s.x);

		n0 = ValueNoise3D<TReal, TInt>({ cube._0.x, cube._1.y, cube._1.z }, seed);
		n1 = ValueNoise3D<TReal, TInt>({ cube._1.x, cube._1.y, cube._1.z }, seed);

		ix1 = lerp(n0, n1, s.y);
		iy1 = lerp(ix0, ix1, s.y);

		return lerp(iy0, iy1, s.z);
	}
		
}}

#endif