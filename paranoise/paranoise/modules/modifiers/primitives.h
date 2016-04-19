#pragma once
#ifndef PARANOISE_MODULE_MODULES_H
#define PARANOISE_MODULE_MODULES_H

#include "../dependencies.h"

namespace zzsystems { namespace paranoise {	namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	/*SIMD_ENABLE_F(vreal)
		inline vreal blend(vreal v0, vreal v1, vreal alpha)
	{
		return lerp(v0, v1, (alpha + 1.0) / 2.0);
	}*/

	// blend input values according to blend factor alpha
	/*SIMD_ENABLE_F(vreal)
		inline vreal blend(vec3<vreal> coords,
			Module<vreal> v0,
			Module<vreal> v1,
			Module<vreal> alpha)
	{
		return blend(v0(coords), v1(coords), alpha(coords));
	}*/

	// Translate the source module's input
	/*SIMD_ENABLE_F(vreal)
		inline vreal translate(const vec3<vreal>& coords, const vec3<vreal>& translation, Module<vreal> source)
	{
		return source(coords + translation);
	}*/

	// Scale the source module's input
	/*SIMD_ENABLE_F(vreal)
		inline vreal scale_input(const vec3<vreal>& coords, const vec3<vreal>& scale, Module<vreal> source)
	{
		return source(coords * scale);
	}*/

	// Scale the source module's output and add a bias
	/*SIMD_ENABLE_F(vreal)
		inline vreal scale_output(const vec3<vreal>& coords, const vreal& scale, Module<vreal> source)
	{
		return source(coords) * scale;
	}*/

	// Scale the source module's output and add a bias
	//SIMD_ENABLE_F(vreal)
	//	inline vreal scale_output_biased(const vec3<vreal>& coords, const vreal& scale, const vreal& bias, Module<vreal> source)
	//{
	//	// source(coords) * scale + bias;
	//	return vfmadd(source(coords), scale, bias);
	//}
	
	SIMD_ENABLE_F(vreal)
		inline vreal pow(const Module<vreal>& source,
			const Module<vreal>& exponent,
			const vec3<vreal>& coords)
	{
		return pow(source(coords),
			exponent(coords));
	}

	SIMD_ENABLED
	struct add : cloneable<module_base<SIMD_T>, add<SIMD_T>>
	{
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		add() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) + get_b()(coords);
		}
	};
	SIMD_ENABLED
	struct sub : cloneable<module_base<SIMD_T>, sub<SIMD_T>>
	{
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		sub() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) - get_b()(coords);
		}
	};

	SIMD_ENABLED
	struct mul : cloneable<module_base<SIMD_T>, mul<SIMD_T>>
	{
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		mul() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) * get_b()(coords);
		}
	};

	SIMD_ENABLED
	struct div : cloneable<module_base<SIMD_T>, div<SIMD_T>>
	{
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		div() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) / get_b()(coords);
		}
	};

	SIMD_ENABLED
	struct blend : cloneable<module_base<SIMD_T>, blend<SIMD_T>>
	{
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);
		MODULE_PROPERTY(alpha, 2);

		blend() : cloneable(3) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			//return lerp(v0, v1, (alpha + 1.0) / 2.0);
			return lerp(get_a()(coords),
						get_b()(coords), 
						(get_alpha()(coords) + fastload<vreal>::_1()) / fastload<vreal>::_0()
				);
		}
	};

	SIMD_ENABLED
	struct translate_input : 
		cloneable<module_base<SIMD_T>, translate_input<SIMD_T>>,
		serializable<json>
	{
		vec3<vreal> offset;
		translate_input(const vec3<vreal> &offset = vec3<vreal>(0.0f)) : 
			cloneable(1), 
			offset(offset)
		{}

		translate_input(const translate_input<SIMD_T> &rhs) : 
			cloneable(rhs), offset(rhs.offset)
		{}
		
		const json& operator <<(const json &source) override
		{
			auto so = source["offset"];
// TODO: add "value" default read
			offset = vec3<vreal>(so[0].get<float>(), so[1].get<float>(), so[2].get<float>());

			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords + offset);
		}
	};

	SIMD_ENABLED
	struct scale_input : 
		cloneable<module_base<SIMD_T>, scale_input<SIMD_T>>,
		serializable<json>

	{
		vreal scale;
		scale_input(const vreal &scale = 1.0f) : 
			cloneable(1), scale(scale)
		{}

		scale_input(const scale_input &rhs) :
			cloneable(rhs), scale(rhs.scale)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value("scale", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);
		
		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords * scale);
		}
	};

	SIMD_ENABLED
	struct scale_output : 
		cloneable<module_base<SIMD_T>, scale_output<SIMD_T>>,
		serializable<json>
	{
		vreal scale;
		scale_output(const vreal &scale = 1.0f) : 
			cloneable(1), scale(scale)
		{}

		scale_output(const scale_output &rhs) : 
			cloneable(rhs), scale(rhs.scale)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value("scale", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return get_a()(coords) * scale;
		}
	};

	SIMD_ENABLED
	struct scale_output_biased :
		cloneable<module_base<SIMD_T>, scale_output_biased<SIMD_T>>,
		serializable<json>
	{
		vreal scale, bias;
		scale_output_biased(const vreal &scale = 1.0f, const vreal &bias = 0.0f) :
			cloneable(1), scale(scale), bias(bias)
		{}

		scale_output_biased(const scale_output_biased &rhs) :
			cloneable(rhs), scale(rhs.scale), bias(rhs.bias)
		{}

		const json& operator <<(const json &source) override
		{
			scale = source.value("scale", 1.0f);
			bias = source.value("bias", 0.0f);
			return source;
		}
		
		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vfmadd(get_a()(coords), scale, bias);
		}
	};


	SIMD_ENABLED
	struct min : cloneable<module_base<SIMD_T>, min<SIMD_T>>
	{
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		min() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override 
		{
			return vmin(get_a()(coords), get_b()(coords));
		}
	};

	SIMD_ENABLED
	struct max : cloneable<module_base<SIMD_T>, min<SIMD_T>>
	{
		MODULE_PROPERTY(a, 0);
		MODULE_PROPERTY(b, 1);

		max() : cloneable(2) {}

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vmax(get_a()(coords), get_b()(coords));
		}
	};

	SIMD_ENABLED
		struct clamp :
		cloneable<module_base<SIMD_T>, clamp<SIMD_T>>,
		serializable<json>
	{
		vreal min, max;
		clamp(const vreal &min = -1.0f, const vreal &max = 1.0f) :
			cloneable(1), min(min), max(max)
		{}

		clamp(const clamp &rhs) :
			cloneable(rhs), min(rhs.min), max(rhs.max)
		{}

		const json& operator <<(const json &source) override
		{
			min = source.value("min", -1.0f);
			max = source.value("max", 1.0f);
			return source;
		}

		MODULE_PROPERTY(a, 0);

		vreal operator()(const vec3<vreal> &coords) const override
		{
			return vclamp(get_a()(coords), min, max);
		}
	};
}}}
#endif
