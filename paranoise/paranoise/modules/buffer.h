#pragma once
#ifndef PARANOISE_MODULES_BUFFER
#define PARANOISE_MODULES_BUFFER

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace math;

	SIMD_ENABLED
	struct buffer : 
		cloneable<module_base<SIMD_T>, buffer<SIMD_T>>,
		serializable<json>
	{
		MODULE_PROPERTY(source, 0)

		buffer(const Module<vreal> &source)
			: cloneable(1), _is_cached(false)
		{}

		buffer(const buffer<vreal, vint> &source)
			: cloneable(source), _is_cached(source._is_cached)
		{}

		vreal operator()(const vec3<vreal>& coords) const override
		{
			auto eq = coords == _buf_coords;
			if(!(_is_cached && static_cast<bool>(eq.x) && static_cast<bool>(eq.y) && static_cast<bool>(eq.z)))
			{
				_buffered = source(coords);
				_buf_coords = coords;
			}

			_is_cached = true;
			return _buffered;			
		}		

	private:
		mutable vec3<vreal> _buf_coords;
		mutable vreal _buffered;
		mutable bool _is_cached;
	};

}}}
#endif