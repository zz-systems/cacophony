#pragma once
#ifndef PARANOISE_MODULES_BUFFER
#define PARANOISE_MODULES_BUFFER

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	SIMD_ENABLED
	struct buffer
	{
		Module<vreal> source;

		buffer(const Module<vreal> &source)
			: source(source), _is_cached(false)
		{}

		buffer(const buffer<vreal, vint> &source)
			: source(source.source), _is_cached(source._is_cached)
		{}

		inline vreal operator()(const Vector3<vreal>& coords)
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

		inline operator Module<vreal>() const { return [this](const auto &c) { return this->operator()(c); }; }

	private:
		Vector3<vreal> _buf_coords;
		vreal _buffered;
		bool _is_cached;
	};

}}}
#endif