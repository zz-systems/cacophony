//
// Vectorized and parallelized version of libnoise using the gorynych SIMD toolkit
// Copyright (C) 2015-2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Original libnoise: 
// Copyright (C) 2003, 2004 Jason Bevins
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "../engine/engine.h"


#ifdef WIN32
#ifdef SOLOWEJ_EXPORTS
#define SOLOWEJDLL_API __declspec(dllexport) 
#else
#define SOLOWEJDLL_API __declspec(dllimport) 
#endif
#else
#define SOLOWEJDLL_API
#endif

extern "C" {

	SOLOWEJDLL_API const char* solowej_get_error();
	SOLOWEJDLL_API zzsystems::solowej::engine* solowej_get_engine(const std::string& instance_key);
	SOLOWEJDLL_API int solowej_compile_immediate(const char* instance_key, const char* content);
	SOLOWEJDLL_API int solowej_compile_file(const char* instance_key, const char* path);
	SOLOWEJDLL_API int solowej_run(const char*instance_key, float* target, float origin_x, float origin_y, float origin_z);
}