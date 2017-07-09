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



#include <fstream>

#include "simd_engine.hpp"

#include "system/runtime_dispatcher.hpp"
#include "json.hpp"

#include "util/serializable.h"
#include "util/cloneable.h"
#include "util/memory.hpp"
#include "util/collections.hpp"
#include "system/remote_activator.hpp"

namespace cacophony { namespace platform
{
	using namespace zacc;

	class engine :
		public engine_base
	{
	private:
		struct dispatcher
        {
			dispatcher() noexcept
            {
                _activator = std::make_unique<system::remote_activator>(ZACC_DYLIBNAME, "cacophony_create_instance", "cacophony_delete_instance");
            }

            template<typename capability> void dispatch_impl()
            {
                if(_engines.count(capability::value) == 0)
                    _engines[capability::value] = _activator->create_instance<capability, engine_base>();
            }

            template<typename capability> void dispatch_impl(const nlohmann::json &source)
            {
                _engines[capability::value]->compile(source);
            }

            template<typename capability> void dispatch_impl(const vec3<float> &origin, float *target)
            {
                log_has_engine<capability>();
                _engines[capability::value]->run(origin, target);
            }

            template<typename capability> void dispatch_impl(const vec3<float> &origin, int *target)
            {
                log_has_engine<capability>();
                _engines[capability::value]->run(origin, target);
            }

        private:
            aligned_map<int, std::shared_ptr<engine_base>> _engines;
			std::shared_ptr<system::remote_activator> _activator;

            template<typename capability> void log_has_engine()
            {
                std::cout << "Has engine: " << std::boolalpha
                          << (_engines.count(capability::value) != 0)
                          << std::endl;
            }
        };

		using engine_dispatcher = system::runtime_dispatcher<dispatcher>;

        engine_dispatcher _dispatcher;
	public:
		std::string file_name;

		engine() noexcept
		{
			// If AVX2 is not available:
			// Disable avx1. Current emulated int8 works like shit. E.g doesnt work at all. Or at least correctly. And is very slow.
			//if(!info.hasAVX2())
			//	info.setFlag(CAPABILITY_AVX1, false);

			_dispatcher.dispatch_some();
		}

		// Compile from json object
		void compile(const nlohmann::json &source)
		{
			//settings << source["environment"]["scheduler"];
			auto environment = source["environment"];

			// No limits
			int feature_set = environment.value("max_feature_set", -1);

			// Restrict with extracted bitmask
            auto flags = zacc::platform::instance().raw();
            zacc::platform::instance().set(flags & feature_set);

			// For each valid branch
			// TODO: downgrade when an unsupported feature is selected
			_dispatcher.dispatch_some(source);
		}

		float* run(const vec3<float> &origin)
		{
			// TODO: downgrade
			//DYNAMIC_DISPATCH_ONE(info,
			//{
			//	return _engines[capability::value]->run(origin);
			//});


			return nullptr;
		}

		void run(const vec3<float> &origin, float *target)
		{
			// TODO: downgrade
			_dispatcher.dispatch_one(origin, target);
		}

		void run(const vec3<float> &origin, int *target)
		{
			// TODO: downgrade
			_dispatcher.dispatch_one(origin, target);
		}
	private:
	};
}}