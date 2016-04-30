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

#include <vector>
#include <memory>
#include "../math/vector.h"
#include "../../submodules/json/src/json.hpp"

namespace zzsystems { namespace solowej { namespace modules
{
	using namespace std;
	using namespace math;
	using json = nlohmann::json;

	SIMD_ENABLED_F
		using Module = function<vreal(const vec3<vreal>&)>;

	SIMD_ENABLED_F
		using Transformer = function<vec3<vreal>(const vec3<vreal>&)>;

	SIMD_ENABLE(vreal, vint)
		using SeededModule = function<vreal(const vec3<vreal>&, const vint& seed)>;

#define MODULE_PROPERTY(name, index) \
	const Module<vreal> &get_##name() const { assert(_modules->size() > index); return _modules->at(index);} \
	void set_##name(const Module<vreal> &value) { assert(_modules->size() > index); _modules->at(index) = value; }

#define MODULE(class_name) \
	SIMD_ENABLED class class_name : \
		public cloneable<module_base<SIMD_T>, class_name<SIMD_T>>

	SIMD_ENABLED
	class module_base : 
		public serializable<json>
	{
		typedef module_base<vreal, vint> self_t;
	public:
		module_base(const size_t n_modules = 0) 
			: _modules(make_shared<vector<Module<vreal>>>(n_modules))
		{}

		module_base(const module_base& other)
			: _modules( make_shared<vector<Module<vreal>>>( other._modules->begin(), other._modules->end()))
		{}

		virtual ~module_base()
		{}

		virtual vreal operator()(const vec3<vreal> &coords) const = 0;
		
		virtual shared_ptr<module_base<vreal, vint>> clone() const = 0;

		virtual operator Module<vreal>() const 
		{ 
			return [this](const auto &c) { return this->operator()(c); }; 
		}

		shared_ptr<vector<Module<vreal>>> get_modules() const
		{
			return _modules;
		}
	protected:
		shared_ptr<vector<Module<vreal>>> _modules;
	};
}}}