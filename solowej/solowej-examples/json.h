#pragma once

//#include "../gorynych/gorynych/gorynych.h"
//#include "../solowej/modules/all.h"
#include "../solowej/util/color.h"
#include "../solowej/engine/parser.h"
#include <fstream>

namespace zzsystems { namespace solowej { namespace examples {
	using namespace modules;
	using namespace math;
	using namespace gorynych;
	using namespace util;

	SIMD_ENABLED
		Module<vreal> generate_from_json()
	{
		parser<vreal, vint> p;

		ifstream f;
		f.open("perlin_test.json");

		auto m = p.parse(f);
		return m;
	}
}}}