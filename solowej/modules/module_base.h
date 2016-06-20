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

#include <type_traits>
#include <exception>
#include <vector>
#include <memory>
#include <functional>

#include "../../dependencies/json/src/json.hpp"

namespace zzsystems { namespace solowej { namespace modules
{
	using namespace gorynych;

	using json = nlohmann::json;

	VECTORIZED_F
		using Module = std::function<vreal(const vec3<vreal>)>;

	VECTORIZED_F
		using Transformer = std::function<vec3<vreal>(const vec3<vreal>)>;

	VECTORIZED
		using SeededModule = std::function<vreal(const vec3<vreal>&, const vint& seed)>;

#define MODULE_PROPERTY(name, index) \
	const Module<vreal> &get_##name() const { assert(this->get_modules()->size() > index); return this->get_modules()->at(index);} \
	void set_##name(const Module<vreal> &value) { assert(this->get_modules()->size() > index); this->get_modules()->at(index) = value; }

#define MODULE(class_name) \
	VECTORIZED class ALIGNED class_name : \
		public cloneable<module_base<SIMD_T>, class_name<SIMD_T>>

#define BASE(class_name) cloneable<module_base<SIMD_T>, class_name<SIMD_T>>

	VECTORIZED
	class ALIGNED module_base :
		public serializable<json>
	{
		typedef module_base<vreal, vint> self_t;
	public:
		const size_t required_module_count;

		module_base(const size_t n_modules = 0) 
			: _modules(make_shared<vector<Module<vreal>>>(n_modules)),
			  required_module_count(n_modules)
		{}

		module_base(const module_base& other)
			: _modules(gorynych::make_shared<vector<Module<vreal>>>( other._modules->begin(), other._modules->end())),
			  required_module_count(other.required_module_count)
		{}

		virtual ~module_base()
		{}

		virtual vreal operator()(const vec3<vreal> &coords) const = 0;
		
		virtual shared_ptr<module_base<vreal, vint>> clone() const = 0;

		virtual inline operator Module<vreal>() const
		{ 
			return [this](const auto c)
			{
				return this->operator()(c);
			};
		}

		virtual bool is_valid()
		{
			return true;
		}

		inline shared_ptr<vector<Module<vreal>>> get_modules() const
		{
			return _modules;
		}
	protected:
		shared_ptr<vector<Module<vreal>>> _modules;
	};
}}}