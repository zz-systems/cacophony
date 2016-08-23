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
#include <fstream>
#include <type_traits>

#include "../../../dependencies/gorynych/gorynych/gorynych.h"

//#include "../library/dispatch.h"
#include "engine_base.h"
#include "../compilers/compiler.h"
#include "../scheduling/scheduler.h"

namespace zzsystems { namespace solowej { namespace platform
{
	DISPATCHED class ALIGNED simd_engine : public engine_base
	{
		using vreal = typename static_dispatcher<capability>::vreal;
		using vint  = typename static_dispatcher<capability>::vint;
	public:
		// COmpile from json object
		virtual void compile(const nlohmann::json &source) override
		{
			cout << ">>compiling " << static_dispatcher<capability>::unit_name() << " branch" << endl;

			_scheduler << source["environment"]["scheduler"];

			_scheduler.set_source(_compiler.compile(source));
			//_scheduler.transform = [](const auto&c) {return c;};

			//return _scheduler.get_config();
		}

		// Run - generating an array inside
		virtual float* run(const vec3<float> &origin) override
		{
			cout << ">>executing " << static_dispatcher<capability>::unit_name() << " branch" << endl;
			return _scheduler(origin);
		}

		// Run in place
		virtual void run(const vec3<float> &origin, float *target) override
		{
			cout << ">>executing " << static_dispatcher<capability>::unit_name() << " branch" << endl;
			_scheduler.schedule(target, origin);
		}

		// Run in place
		virtual void run(const vec3<float> &origin, int *target) override
		{
			cout << ">>executing " << static_dispatcher<capability>::unit_name() << " branch" << endl;
			_scheduler.schedule(target, origin);
		}
	private:
		scheduler<capability> _scheduler;
		compiler<capability> _compiler;
	};

	namespace detail
	{
		template<typename capability>
		std::shared_ptr<engine_base> get_engine()
		{
			//throw std::runtime_error("Engine not implemented");
			return nullptr;
		}

#define BRANCH_DEF(branch) template<> std::shared_ptr<engine_base> get_engine<capability_##branch>();
				STATIC_DISPATCH_SOME_RAW();
#undef BRANCH_DEF
	}

	class engine :
		public engine_base
	{
	private:

	public:
		gorynych::system_info info;
		std::string file_name;

		aligned_map<int, shared_ptr<engine_base>> _engines;

		engine()
		{
			// If AVX2 is not available:
			// Disable avx1. Current emulated int8 works like shit. E.g doesnt work at all. Or at least correctly. And is very slow.
			if(!info.hasAVX2())
				info.setFlag(CAPABILITY_AVX1, false);

			DYNAMIC_DISPATCH_SOME(info,
			{
				_engines[capability::value] = detail::get_engine<capability>();
			});
		}

		// COmpile from json object
		void compile(const nlohmann::json &source)
		{
			//settings << source["environment"]["scheduler"];
			auto environment = source["environment"];

			// No limits
			int feature_set = environment.value("max_feature_set", -1);

			// Restrict with extracted bitmask
			info.feature_flags &= feature_set;

			// For each valid branch
			// TODO: downgrade when an unsupported feature is selected
			//DYNAMIC_DISPATCH_SOME(info,
			//{
			// 	if(_engines[capability::value] != nullptr)
			//		 _engines[capability::value]->compile(source);
			 	//find_engine(info)
			//});

			_engine = find_engine(info);
			_engine->compile(source);
		}

		float* run(const vec3<float> &origin)
		{
			// TODO: downgrade
			// DYNAMIC_DISPATCH_ONE(info,
			// {
			// 	return _engines[capability::value]->run(origin);
			// });

			// return nullptr;

			return _engine->run(origin);
		}

		void run(const vec3<float> &origin, float *target)
		{
			// TODO: downgrade
			// DYNAMIC_DISPATCH_ONE(info,
			// {
			// 	_engines[capability::value]->run(origin, target);
			// });

			_engine->run(origin, target);
		}

		void run(const vec3<float> &origin, int *target)
		{
			// TODO: downgrade
			// DYNAMIC_DISPATCH_ONE(info,
			// {
			// 	_engines[capability::value]->run(origin, target);
			// });

			_engine->run(origin, target);
		}

		shared_ptr<engine_base> find_engine(const system_info &info)
		{
			int capability = info.feature_flags;
			auto ii = info;
			shared_ptr<engine_base> engine = nullptr;
			
			do
			{
				DYNAMIC_DISPATCH_ONE(ii, 
				{
					engine = _engines[capability::value];					
				});

				if(engine == nullptr)
				{
					cerr << "[Warning] Engine for [" << system_info::getName((capabilities)hibit(ii.feature_flags)) << "] branch not found" << endl;

					ii.feature_flags = unsetHighestBit(ii.feature_flags);
				}
			}
			while(engine == nullptr);

			return engine;
		}
	private:

		template<typename T> T unsetHighestBit(T val) 
		{
			for(uint32_t i = sizeof(T) * numeric_limits<char>::digits - 1; i >= 0; i--) 
			{
				if(val & (1 << i)) 
				{
					val &= ~(1 << i);
					break;
				}
			}
			return val;
		}

		int hibit(unsigned int n) {
			n |= (n >>  1);
			n |= (n >>  2);
			n |= (n >>  4);
			n |= (n >>  8);
			n |= (n >> 16);
			return n - (n >> 1);
		}

		shared_ptr<engine_base> _engine;
	};
}}}