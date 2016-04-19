#pragma once
#ifndef PARANOISE_MODULES_ROTATE
#define PARANOISE_MODULES_ROTATE

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED
	class rotate : 
		public cloneable<module_base<SIMD_T>, rotate<SIMD_T>>,
		public serializable<json>
	{
	public:
		mat3x3<vreal> rot;
		vec3<float>  angles;

		rotate() : rotate({ 0.0f, 0.0f, 0.0f }) {}		

		rotate(const vec3<float> &angles) : 
			cloneable(1), 
			angles(angles)
		{
			build_rot_matrix(angles);
		}

		rotate(const rotate<vreal, vint> &rhs) :
			cloneable(rhs), 
			rot(rhs.rot), 
			angles(rhs.angles)
		{}

		const json& operator <<(const json &source) override
		{
			if (source["angles"] != nullptr && source["angles"].is_object())
			{
				angles.x = source.value("x", 0.0f);
				angles.y = source.value("y", 0.0f);
				angles.z = source.value("z", 0.0f);
			}

			build_rot_matrix(angles);
			return source;
		}

		MODULE_PROPERTY(source, 0)

		vreal operator()(const vec3<vreal>& coords) const override
		{
			return get_source()(rot * coords);
		}
	private:
		void build_rot_matrix(const vec3<float>& angles)
		{
			auto cos = vec3<vreal>(::cos(angles.x), ::cos(angles.y), ::cos(angles.z));
			auto sin = vec3<vreal>(::sin(angles.x), ::sin(angles.y), ::sin(angles.z));

			rot._0 = vec3<vreal>{ sin.x * sin.y * sin.z + cos.y * cos.z,	cos.x * sin.z,		sin.y * cos.z - cos.y * sin.x * sin.z };
			rot._1 = vec3<vreal>{ sin.y * sin.x * cos.z - cos.y * sin.z,	cos.x * cos.z,		-cos.y * sin.x * cos.z - sin.y * sin.z };
			rot._2 = vec3<vreal>{ -sin.y * cos.x,							sin.x,				cos.y * cos.x };
		}
	};
}}}
#endif