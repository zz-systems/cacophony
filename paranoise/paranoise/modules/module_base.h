#pragma once
#ifndef PARANOISE_MODULES_MODULE_BASE
#define PARANOISE_MODULES_MODULE_BASE
#include "../base.h"
#include <vector>
#include <memory>
#include "../vector.h"
#include "../math/matrix.h"

#include <cassert>
#include "../lib/json.hpp"

namespace zzsystems { namespace paranoise { namespace modules
{
	using namespace std;
	using namespace math;
	using json = nlohmann::json;

	SIMD_ENABLE_F(vreal)
		using Module = function<vreal(const vec3<vreal>&)>;

	SIMD_ENABLE_F(vreal)
		using Transformer = function<vec3<vreal>(const vec3<vreal>&)>;

	SIMD_ENABLE(vreal, vint)
		using SeededModule = function<vreal(const vec3<vreal>&, const vint& seed)>;

#define MODULE_PROPERTY(name, index) \
	const Module<vreal> &get_##name() const { assert(_modules->size() > index); return _modules->at(index);} \
	void set_##name(const Module<vreal> &value) { assert(_modules->size() > index); _modules->at(index) = value; }

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
<<<<<<< HEAD
			: _modules(make_shared<vector<Module<vreal>>>(other._modules->begin(), other._modules->end()))
=======
			: _modules( make_shared<vector<Module<vreal>>>( other._modules->begin(), other._modules->end()))
>>>>>>> 2c614baf9ceae55d5ef72429490ddf6860612141
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
#endif

