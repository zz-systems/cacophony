//
// Vectorized and parallelized version of libnoise using the zacc SIMD toolkit
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

#include "dependencies.hpp"

#include "../../dependencies/json/src/json.hpp"

namespace cacophony { namespace modules
{
	using namespace zacc;
	using namespace zacc::math;

	using json = nlohmann::json;

	DISPATCHED
		using Module = std::function<zfloat(const vec3<zfloat>&)>;

	DISPATCHED
		using Transformer = std::function<vec3<zfloat>(const vec3<zfloat>&)>;

	DISPATCHED
		using SeededModule = std::function<zfloat(const vec3<zint>&, const zint& seed)>;

#define MODULE_PROPERTY(name, index) \
	const Module<branch> &get_##name() const { assert(this->get_modules()->size() > index); return this->get_modules()->at(index);} \
	void set_##name(const Module<branch> &value) { assert(this->get_modules()->size() > index); this->get_modules()->at(index) = value; }

#define MODULE(class_name) \
	DISPATCHED class alignas(branch::alignment) class_name : \
		public cloneable<module_base<branch>, class_name<branch>>

#define BASE(class_name) cloneable<module_base<branch>, class_name<branch>>

	DISPATCHED
	class alignas(branch::alignment) module_base :
		public serializable<json>
	{
		typedef module_base<branch> self_t;
	public:
		const size_t required_module_count;

		module_base(const size_t n_modules = 0) 
			: required_module_count(n_modules),
			  _modules(zacc::make_shared<std::vector<Module<branch>>>(n_modules))
		{}

		module_base(const module_base& other)
			: required_module_count(other.required_module_count),
			  _modules(zacc::make_shared<std::vector<Module<branch>>>( other._modules->begin(), other._modules->end()))
		{}

		virtual ~module_base()
		{}

		virtual zfloat operator()(const vec3<zfloat> &coords) const = 0;
		
		virtual std::shared_ptr<module_base<branch>> clone() const = 0;

		virtual inline operator Module<branch>() const
		{ 
			return [this](const auto &c)
			{
				return this->operator()(c);
			};
		}

		virtual bool is_valid()
		{
			return true;
		}

		inline std::shared_ptr<std::vector<Module<branch>>> get_modules() const
		{
			return _modules;
		}
	protected:
		std::shared_ptr<std::vector<Module<branch>>> _modules;
	};
}}