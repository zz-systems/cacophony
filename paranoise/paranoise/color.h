#pragma once
#ifndef PARANOISE_COLOR_H
#define PARANOISE_COLOR_H

#include "basetypes.h"

namespace paranoise { namespace util {

	ALIGN(32) union color_t
	{
		uint8 components[4];
		struct { uint8 a, r, g, b; };
	};



}}

#endif