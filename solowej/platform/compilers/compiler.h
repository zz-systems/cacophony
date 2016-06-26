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

#include "../dependencies.h"
#include "../../modules/all.h"
#include "../../util/string.h"

#include <iostream>
#include <sstream>
#include <map>
#include <unordered_set>
#include <exception>

#include "compiler_base.h"
#include "compiler_v09.h"

namespace zzsystems { namespace solowej { namespace platform {

	DISPATCHED class compiler
	{
		using vreal = typename static_dispatcher<capability>::vreal;
		using vint	= typename static_dispatcher<capability>::vint;

	public:
		compiler(bool aggregate_errors = false)
		{
			register_compiler_version<compiler_v09>(aggregate_errors);
		}

		virtual Module<vreal> compile(istream &source)
		{
			json j(source);
			return compile(j);
		}

		virtual Module<vreal> compile(const json &source)
		{
			auto compiler = get_compiler(source["version"]);
			if(compiler != nullptr)
				return compiler->compile(source);

			// TODO: error  handling
			// Return a dummy for now to prevevnt crashes
			return [](const auto& c){return 0;};
		}

		shared_ptr<compiler_base<capability>> get_compiler(const string &version)
		{
			if(registry.find(version) != registry.end())
				return registry[version];

			return nullptr;
		}

		template<template <typename> class cc, typename... Args>
		void register_compiler_version(Args&&... args)
		{
			auto ptr = make_shared<cc<capability>>(forward<Args>(args)...);

			registry[ptr->version()] = ptr;
		}

	private:
		aligned_map<string, shared_ptr<compiler_base<capability>>> registry;
		bool _aggregate_errors;
	};
}}}