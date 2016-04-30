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

#include "dependencies.h"
#include "parser.h"
#include "scheduler.h"
#include <fstream>
namespace zzsystems { namespace solowej
{
	using namespace gorynych;
	using namespace scheduler;

	class engine :
		public serializable<nlohmann::json>
	{		
	public:
		system_info info;
		string file_name;
		scheduler_settings settings;

		engine() = default;

		const nlohmann::json &operator<<(const nlohmann::json &source) override
		{
			auto environment = source["environment"];

			// No limits
			int feature_set = environment.value("max_feature_set", -1);

			// Restrict with extracted bitmask
			info.feature_flags &= feature_set;	

			// Build modules
			compile(source);

			return source;
		}
		
		// COmpile from file
		void compile_file(const string &path)
		{
			ifstream file(path);
			json source(file);
			
			*this << source;
		}

		// Compile from immediate string
		void compile_imm_str(const string &content)
		{
			json source = json::parse(content);

			*this << source;
		}

		// COmpile from json object
		void compile(const nlohmann::json &source)
		{
			// Get scheduler type
			settings << source["environment"]["scheduler"];

			// For each valid branch
			SIMD_BUILD(info,
			{
				cout << ">>building " << static_dispatcher<capability>::unit_name() << " branch" << endl;

				auto& scheduler		= (scheduler_cache<vreal, vint>());
				scheduler.source	= (compile_module<vreal, vint>(source));
				scheduler.transform = [](const auto&c) {return c;};

				scheduler << source["environment"]["scheduler"];
			});
		}

		float* run(const vec3<float> &origin)
		{
			SIMD_DISPATCH(info,
			{
				return (exec<capability, vreal, vint>(origin));
			});
		}

		void run(float *target, const vec3<float> &origin)
		{
			SIMD_DISPATCH(info,
			{
				return (exec<capability, vreal, vint>(target, origin));
			});
		}
	private:
		SIMD_ENABLED
		auto compile_module(const nlohmann::json &source)
		{
			parser<SIMD_T> p;
			return p.parse(source);
		}

		SIMD_ENABLED
		cpu_scheduler<vreal> &scheduler_cache()
		{
			static cpu_scheduler<vreal> scheduler;

			return scheduler;
		}


		template<typename capability, typename vreal, typename vint>
		auto exec(const vec3<float> &origin)
		{
			cout << ">>dispatch: using " << static_dispatcher<capability>::unit_name() << " branch" << endl;
			
			return scheduler_cache<vreal, vint>()(origin);
		}

		template<typename capability, typename vreal, typename vint>
		void exec(float *target, const vec3<float> &origin)
		{
			cout << ">>dispatch: using " << static_dispatcher<capability>::unit_name() << " branch" << endl;

			scheduler_cache<vreal, vint>()(target, origin);
		}
	};
}}