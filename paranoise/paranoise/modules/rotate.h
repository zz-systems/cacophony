#pragma once
#ifndef PARANOISE_MODULES_ROTATE
#define PARANOISE_MODULES_ROTATE

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	SIMD_ENABLED
	class rotate : public module_base<vreal, vint>
	{
	public:
		Matrix3x3<vreal> rot;
		Vector3<float>  angles;

		rotate() : rotate({ 0.0f, 0.0f, 0.0f }) {}
		rotate(const Vector3<float> &angles) : module_base(1), angles(angles)
		{
			auto cos = Vector3<vreal>( ::cos(angles.x), ::cos(angles.y), ::cos(angles.z) );
			auto sin = Vector3<vreal>( ::sin(angles.x), ::sin(angles.y), ::sin(angles.z) );

			rot._0 = Vector3<vreal>{ sin.x * sin.y * sin.z + cos.y * cos.z,	cos.x * sin.z,		sin.y * cos.z - cos.y * sin.x * sin.z };
			rot._1 = Vector3<vreal>{ sin.y * sin.x * cos.z - cos.y * sin.z,	cos.x * cos.z,		-cos.y * sin.x * cos.z - sin.y * sin.z };
			rot._2 = Vector3<vreal>{-sin.y * cos.x,							sin.x,				cos.y * cos.x						   };
		}

		rotate(const rotate<vreal, vint> &rhs): rotate(rhs.angles)
		{}

		MODULE_PROPERTY(source, 0)

		vreal operator()(const Vector3<vreal>& coords) const override
		{
			return (*get_source())(rot * coords);
		}
	};
}}}
#endif