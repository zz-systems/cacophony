#pragma once

#include "../paranoise/parallel/all.h"
#include "../paranoise/modules/all.h"
#include "../paranoise/color.h"
#include "../paranoise/engine/parser.h"
#include <fstream>

namespace zzsystems { namespace paranoise { namespace examples {
	using namespace modules;
	using namespace math;
	using namespace simdal;
	using namespace util;

	SIMD_ENABLE(vreal, vint)
		Module<vreal> generate_from_json()
	{
		parser<vreal, vint> p;

		ifstream f;
		f.open("perlin_test.json");

		auto m = p.parse(f);
		return m;
	}
}}}