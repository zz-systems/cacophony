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
#include "platform/dependencies.hpp"
#include "util/memory.hpp"
#include "util/collections.hpp"
#include "system/entrypoint.hpp"

namespace cacophony { namespace platform
{
	using namespace zacc;

	class engine :
		public engine_base
	{
	private:
		struct engine_branch_impl
        {
            engine_branch_impl(engine* engine) noexcept
                    : _parent(engine)
            {}
            DISPATCHED void dispatch_impl()
            {
                _parent->_engines[branch::capability::value] = std::static_pointer_cast<simd_engine<branch>>(
                        zacc::system::resolve_entrypoint<branch>()
                );
            }

            DISPATCHED void dispatch_impl(const nlohmann::json &source)
            {
                _parent->_engines[branch::capability::value]->compile(source);
            }

            DISPATCHED void dispatch_impl(const vec3<float> &origin, float *target)
            {
                log_has_engine<branch>();
                _parent->_engines[branch::capability::value]->run(origin, target);
            }

            DISPATCHED void dispatch_impl(const vec3<float> &origin, int *target)
            {
                log_has_engine<branch>();
                _parent->_engines[branch::capability::value]->run(origin, target);
            }

        private:
            const std::shared_ptr<engine> _parent;

            DISPATCHED void log_has_engine()
            {
                std::cout << "Has engine: " << std::boolalpha
                          << (_parent->_engines.count(branch::capability::value) != 0)
                          << std::endl;
            }
        };

		using engine_branch = runtime_dispatcher<engine_branch_impl>;

        engine_branch _branch;
	public:
		std::string file_name;

		aligned_map<int, std::shared_ptr<engine_base>> _engines;

		engine() noexcept
            : _branch(this)
		{
			// If AVX2 is not available:
			// Disable avx1. Current emulated int8 works like shit. E.g doesnt work at all. Or at least correctly. And is very slow.
			//if(!info.hasAVX2())
			//	info.setFlag(CAPABILITY_AVX1, false);

            _branch.dispatch_some();
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
            _branch.dispatch_some(source);
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
            _branch.dispatch_one(origin, target);
		}

		void run(const vec3<float> &origin, int *target)
		{
			// TODO: downgrade
            _branch.dispatch_one(origin, target);
		}
	private:
	};
}}