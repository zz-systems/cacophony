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
#include "../modules/all.h"

#include <iostream>
#include <map>
namespace zzsystems { namespace solowej {
	using namespace std;
	using namespace modules;

	SIMD_ENABLED class parser 
	{
		using tmodules = map <string, shared_ptr<module_base<vreal, vint>>>;

		map<string, tmodules> _module_resolver =
		{
			{ "generator", tmodules
				{
					{ "perlin",		make_shared<perlin<vreal, vint>>() },
					{ "billow",		make_shared<billow<vreal, vint>>() },
					{ "voronoi",	make_shared<voronoi<vreal, vint>>() },
					{ "ridged",		make_shared<ridged_multifractal<vreal, vint>>() },
					{ "spheres",	make_shared<spheres<vreal, vint>>()},
					{ "cylinders",	make_shared<cylinders<vreal, vint>>()}
				}
			},
			{ "modifier", tmodules{
				{ "curve", make_shared<curve<vreal, vint>>() },
				{ "rotate", make_shared<rotate<vreal, vint>>() },
				{ "select", make_shared<select<vreal, vint>>() },
				{ "terrace", make_shared<terrace<vreal, vint>>() },
				{ "turbulence", make_shared<turbulence<vreal, vint>>() },

				{ "add", make_shared<add<vreal, vint>>() },
				{ "sub", make_shared<sub<vreal, vint>>() },
				{ "mul", make_shared<mul<vreal, vint>>() },
				{ "div", make_shared<div<vreal, vint>>() },

				{ "min", make_shared<min<vreal, vint>>() },
				{ "max", make_shared<max<vreal, vint>>() },
				{ "clamp", make_shared<clamp<vreal, vint>>() },

				{ "blend", make_shared<blend<vreal, vint>>() },

				{ "translate_input",		make_shared<translate_input<vreal, vint>>() },
				{ "scale_input",			make_shared<scale_input<vreal, vint>>() },
				{ "scale_output",			make_shared<scale_output<vreal, vint>>() },
				{ "scale_output_biased",	make_shared<scale_output_biased<vreal, vint>>() }
			}
			}
		};

		public:
			Module<vreal> parse(istream &source)
			{
				json j(source);

				return parse(j);
			}

			Module<vreal> parse(const json &source)
			{
				string version = source["version"];
				if (version == "0.9")
				{
					return parse_v09(source);
				}

				return nullptr;
			}
		private:
			Module<vreal> parse_v09(const json &j)
			{
				map<string, shared_ptr<module_base<vreal, vint>>> modules;

				if (j["modules"].is_array())
				{
					for (auto module : j["modules"])
					{
						const string instance_name = module["name"];
						const string module_type = module["type"];
						const string module_name = module["module"];

						if (_module_resolver[module_type].find(module_name) == _module_resolver[module_type].end())
							continue;

						auto skeleton = _module_resolver[module_type][module_name]->clone();
						modules[instance_name] = skeleton;
					}

					for (auto module : j["modules"])
					{
						string instance_name = module["name"];
						if (modules.find(instance_name) == modules.end())
							continue;

						auto instance = modules[instance_name];

						auto settings = module["settings"];
						auto sources = module["source"];

						*instance << settings;

						if (sources != nullptr && sources.is_array())
						{
							auto target = instance->get_modules();
							target->resize(sources.size());

							for (size_t index = 0; index < sources.size(); index++)
							{
								auto ref_name = sources[index];
								auto ref = modules[ref_name];

								target->at(index) = [ref](const auto &c) { return ref->operator()(c);};
							}
						}
					}
				}

				string entry_point_name = j["environment"]["entryPoint"];
				auto entry_point = modules[entry_point_name];
				return [entry_point](const auto &c) { return entry_point->operator()(c);};
			}
	};
}}
