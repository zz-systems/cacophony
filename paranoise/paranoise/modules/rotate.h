#pragma once
#ifndef PARANOISE_MODULES_ROTATE
#define PARANOISE_MODULES_ROTATE

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE(TReal, TInt)
	struct rotate
	{
		Matrix3x3<TReal> rot;
		Vector3<float>  angles;

		rotate() : rotate({ 0.0f, 0.0f, 0.0f }) {}
		rotate(const Vector3<float> &angles) : angles(angles)
		{
			auto cos = Vector3<TReal>( ::cos(angles.x), ::cos(angles.y), ::cos(angles.z) );
			auto sin = Vector3<TReal>( ::sin(angles.x), ::sin(angles.y), ::sin(angles.z) );

			rot._0 = Vector3<TReal>{ sin.x * sin.y * sin.z + cos.y * cos.z,	cos.x * sin.z,		sin.y * cos.z - cos.y * sin.x * sin.z };
			rot._1 = Vector3<TReal>{ sin.y * sin.x * cos.z - cos.y * sin.z,	cos.x * cos.z,		-cos.y * sin.x * cos.z - sin.y * sin.z };
			rot._2 = Vector3<TReal>{-sin.y * cos.x,							sin.x,				cos.y * cos.x						   };
		}

		rotate(const rotate<TReal, TInt> &rhs): rotate(rhs.angles)
		{}

		inline TReal operator()(const Vector3<TReal>& coords, const Module<TReal>& source) const
		{
			return source(rot * coords);
		}

		//inline operator Module<TReal>() const { return [this](const auto &c) { return this->operator()(c); }; }
	};

	// inline operator (Module<TReal>)() { return operator(); }
}}
#endif