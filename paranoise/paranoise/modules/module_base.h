#pragma once
#ifndef PARANOISE_MODULES_MODULE_BASE
#define PARANOISE_MODULES_MODULE_BASE
#include "../base.h"
#include <vector>
#include <memory>

#include <cassert>

namespace zzsystems { namespace paranoise { namespace modules
{
	using namespace std;
	using namespace util;

	SIMD_ENABLE_F(TReal)
		using Module = function<TReal(const Vector3<TReal>&)>;

	SIMD_ENABLE_F(TReal)
		using Transformer = function<Vector3<TReal>(const Vector3<TReal>&)>;

	SIMD_ENABLE(TReal, TInt)
		using SeededModule = function<TReal(const Vector3<TReal>&, const TInt& seed)>;

#define MODULE_PROPERTY(name, index) \
	const Module<vreal> &get_##name() const { assert(_modules>size() > index); return _modules->at(index);} \
	void set_##name(const Module<vreal> &value) { assert(_modules>size() > index); _modules->at(index) = value; }

	SIMD_ENABLED
	class module_base
	{
		typedef module_base<vreal, vint> self_t;
	public:
		module_base(const size_t n_modules = 0) 
			: _modules(make_shared<vector<Module<vreal>>>(n_modules))
		{}

		module_base(const module_base& other)
			: _modules(other._modules)
		{}

		virtual ~module_base()
		{}

		virtual vreal operator()(const Vector3<vreal> &coords) const = 0;

		operator Module<vreal>() const 
		{ 
			return [this](const auto &c) { return this->operator()(c); }; 
		}

	protected:
		shared_ptr<vector<Module<vreal>>> _modules;
	};
}}}
#endif

