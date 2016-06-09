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
#include "../util/string.h"

#include <iostream>
#include <sstream>
#include <map>
#include <unordered_set>
#include <exception>

namespace zzsystems { namespace solowej {
	using namespace modules;

	struct amt_node
	{
		std::string instance_name, module_type, module_name;

		amt_node() = default;
		amt_node(string instance_name, string module_type, string module_name) :
			instance_name(instance_name),
			module_type(module_type),
			module_name(module_name)
		{}

		nlohmann::json module_config, sources_config;

		std::vector<shared_ptr<amt_node>> references;
		std::vector<shared_ptr<amt_node>> referenced_in;
	};

	struct compilation_error : public std::runtime_error
	{
		compilation_error() : runtime_error("Compilation error")
		{}

		compilation_error(string message) : runtime_error("Compilation error: " + message)
		{}
	};

	struct aggregate_error : public std::exception
	{
		vector<shared_ptr<exception>> errors;

		aggregate_error() : std::exception()
		{}

		void push_back(shared_ptr<exception> ex)
		{
			errors.push_back(ex);
		}
	};

	struct invalid_module_error : public compilation_error
	{
		invalid_module_error(string module_name) : compilation_error("Faulty module [" + module_name + "]")
		{}

		invalid_module_error(string module_name, string reason) : compilation_error("Faulty module [" + module_name + "]: " + reason)
		{}
	};

	struct unresolved_module_reference_error : public compilation_error
	{
		unresolved_module_reference_error(string module_name) : compilation_error("Unresolved module reference [" + module_name + "]")
		{}

        unresolved_module_reference_error(string module_name, string module_type) : compilation_error("Unresolved module reference [" + module_type + "][" + module_name + "]")
        {}
	};

	struct unresolved_node_reference_error : public compilation_error
	{
		unresolved_node_reference_error(string module_name) : compilation_error("Unresolved node reference [" + module_name + "]")
		{}
	};

	struct duplicate_module_error : public compilation_error
	{
		duplicate_module_error(string module_name) : compilation_error("Duplicate module [" + module_name + "] definition")
		{}
	};

	struct invalid_entry_point_error : public compilation_error
	{
		invalid_entry_point_error() : compilation_error("Entry point not found")
		{}

		invalid_entry_point_error(string entry_point) : compilation_error("Entry point not found [" + entry_point + "]")
		{}
	};

	struct cyclic_module_ref_error : public compilation_error
	{
		cyclic_module_ref_error() : compilation_error("Cyclic reference found")
		{}

		cyclic_module_ref_error(string module_name) : compilation_error("Cyclic reference found for [" + module_name + "]")
		{}
	};

	VECTORIZED class compiler
	{
		using tmodules          = std::map <string, std::shared_ptr<module_base<vreal, vint>>>;
		using emitted_modules   = gorynych::aligned_map<string, gorynych::shared_ptr<module_base<vreal, vint>>>;

		std::map<string, tmodules> _module_resolver =
		{
			{ "generator", tmodules
				{
					{ "perlin",		gorynych::make_shared<mod_perlin<vreal, vint>>() },
					{ "billow",		gorynych::make_shared<mod_billow<vreal, vint>>() },
					{ "voronoi",	gorynych::make_shared<mod_voronoi<vreal, vint>>() },
					{ "ridged",		gorynych::make_shared<mod_ridged_multifractal<vreal, vint>>() },
					{ "spheres",	gorynych::make_shared<mod_spheres<vreal, vint>>()},
					{ "cylinders",	gorynych::make_shared<mod_cylinders<vreal, vint>>()},
					{ "checker",	gorynych::make_shared<mod_checkerboard<vreal, vint>>()},
					{ "const",		gorynych::make_shared<mod_const<vreal, vint>>()},
				}
			},
			{ "modifier", tmodules{
				{ "curve", 		gorynych::make_shared<mod_curve<vreal, vint>>() },
				{ "rotate", 	gorynych::make_shared<mod_rotate<vreal, vint>>() },
				{ "select", 	gorynych::make_shared<mod_select<vreal, vint>>() },
				{ "terrace", 	gorynych::make_shared<mod_terrace<vreal, vint>>() },
				{ "turbulence", gorynych::make_shared<mod_turbulence<vreal, vint>>() },
				{ "displace", 	gorynych::make_shared<mod_displace<vreal, vint>>() },


				{ "add", gorynych::make_shared<mod_add<vreal, vint>>() },
				{ "sub", gorynych::make_shared<mod_sub<vreal, vint>>() },
				{ "mul", gorynych::make_shared<mod_mul<vreal, vint>>() },
				{ "div", gorynych::make_shared<mod_div<vreal, vint>>() },

				{ "abs", gorynych::make_shared<mod_abs<vreal, vint>>() },
				{ "min", gorynych::make_shared<mod_min<vreal, vint>>() },
				{ "max", gorynych::make_shared<mod_max<vreal, vint>>() },
				{ "clamp", gorynych::make_shared<mod_clamp<vreal, vint>>() },

				{ "blend", gorynych::make_shared<mod_blend<vreal, vint>>() },

				{ "translate_input",		gorynych::make_shared<mod_translate_input<vreal, vint>>() },
				{ "scale_input",			gorynych::make_shared<mod_scale_input<vreal, vint>>() },
				{ "scale_output",			gorynych::make_shared<mod_scale_output<vreal, vint>>() },
				{ "scale_output_biased",	gorynych::make_shared<mod_scale_output_biased<vreal, vint>>() }
			}
			}
		};

		bool _aggregate_errors;

		public:

			compiler(bool aggregate_errors = true) : _aggregate_errors(aggregate_errors)
			{}

			Module<vreal> compile(istream &source)
			{
				json j(source);

				return compile(j);
			}

			Module<vreal> compile(const json &source)
			{
				string version = source["version"];
				if (version == "0.9")
				{
					return compile_v09(source);
				}

				return nullptr;
			}

            template<template<typename, typename> class module>
            void add_module_map(const string &type, const string &name)
            {
                _module_resolver[type][name] = gorynych::make_shared<module<vreal, vint>>();
            }

            template<template<typename, typename> class module>
            void add_module_map(const string &path)
            {
                auto fragments = split(path, '/');

                _module_resolver[fragments[0]][fragments[1]] = gorynych::make_shared<module<vreal, vint>>();
            }

		private:
			template<typename error, typename... Args>
			void emit_error(aggregate_error &errors, Args &&... args) const
			{
                auto err = std::make_shared<error>(std::forward<Args>(args)...);
                cerr << err->what() << endl;

                errors.push_back(err);

                if(!_aggregate_errors)
                    throw *err;
			}


            // Prepares a flat node list
            // Builds an abstract module tree
			virtual shared_ptr<amt_node> build_amt(const json &source, emitted_modules &emitted, aggregate_error &errors) const
			{
				std::map<string, shared_ptr<amt_node>> nodes;
				shared_ptr<amt_node> entry_point;

                // Fetch entry point
				auto environment        = source["environment"];
				auto raw_entry_point    = environment["entryPoint"];
				string entry_point_name = raw_entry_point != nullptr ? raw_entry_point : "";

				bool has_entry_point    = entry_point_name != "";
				if(!has_entry_point)
					emit_error<invalid_entry_point_error>(errors);

				// parse modules partially (flat list)
				if (source["modules"].is_array())
				{
					for (auto module : source["modules"])
					{
						auto node = make_shared<amt_node>();

						node->instance_name 	= module["name"];
						node->module_type 		= module["type"];
						node->module_name 		= module["module"];
						node->module_config 	= module["settings"];
						node->sources_config 	= module["source"];

                        // Check for duplicate declaration
						if(nodes.find(node->instance_name) != nodes.end())
							emit_error<duplicate_module_error>(errors, node->instance_name);
						else
							nodes[node->instance_name] = node;
					}
				}

                // Check if entry node exists
				has_entry_point = has_entry_point && nodes.find(entry_point_name) != nodes.end();

				if(has_entry_point)
					entry_point = nodes[entry_point_name];
				else
					emit_error<unresolved_node_reference_error>(errors, entry_point_name);

				// build tree with empty cycle check chain
				build_amt_node(entry_point, nodes, unordered_set<shared_ptr<amt_node>>(), emitted, errors);

				return entry_point;
			}


            // Emits a real functor from abstract module tree node
			virtual Module<vreal> emit_amt_module(shared_ptr<amt_node> node, emitted_modules &emitted, aggregate_error &errors) const
			{
				auto node_module 	= emitted[node->instance_name];
				auto target_modules = node_module->get_modules();

                // Pass module config to module-level parser
				try
				{
#ifdef __GNUC__
					node_module->operator<<(node->module_config);
#else
					*node_module << node->module_config;
#endif
				}
				catch(exception ex)
				{
					emit_error<invalid_module_error>(errors, ex.what());
				}

                // Set sources
				for (size_t index = 0; index < node->references.size(); index++)
				{
					auto ref_node = node->references[index];

					target_modules->at(index) = emit_amt_module(ref_node, emitted, errors);
				}

                // Wrap exec operator
				return [node_module](const auto &coords) { return node_module->operator()(coords); };
			}


            // Builds a node tree from a flat config
            // Checks for cyclic references
            // Checks for existing refereces
			virtual void build_amt_node(shared_ptr<amt_node> node,
								std::map<string, shared_ptr<amt_node>> &nodes,
								unordered_set<shared_ptr<amt_node>> cycle_check, // each stage should have it's own chain
								emitted_modules &emittable,
								aggregate_error &errors) const
			{
                // Check for cyclic references.
                // In case a cycle is present: abort. Otherwise the tree will grow indefinitely
                if(cycle_check.find(node) != cycle_check.end())
                {
                    emit_error<cyclic_module_ref_error>(errors, node->instance_name);

                    return; // don't even try to go deeper (cycle!)
                }
                else
                {
                    cycle_check.insert(node);
                }


                auto skeleton = find_module(node, errors);

                // Check if the source count matches the module requirements
				bool has_sources = node->sources_config != nullptr && node->sources_config.is_array();
				if(has_sources && skeleton->required_module_count > 0 && node->sources_config.size() < skeleton->required_module_count)
				{
					emit_error<invalid_module_error>(errors, node->instance_name, "input module count mismatch");
				}
				else
				{
					emittable[node->instance_name] = skeleton->clone();
				}

                // Resolve sources
				if (has_sources)
				{
					for (string ref_name : node->sources_config)
					{
                        // Check if the referenced node exists
						if (nodes.find(ref_name) == nodes.end())
						{
							emit_error<unresolved_node_reference_error>(errors, ref_name);
							continue;
						}

                        // Set node references
						auto ref_node = nodes[ref_name];

						ref_node->referenced_in.push_back(node);
						node->references.push_back(ref_node);

                        // Build subtree
                        build_amt_node(ref_node, nodes, cycle_check, emittable, errors);
					}
				}
			}

            // Checks if the requested module skeleton [type, name] exists
			virtual shared_ptr<module_base<vreal, vint>> find_module(shared_ptr<amt_node> node, aggregate_error &errors) const
			{
                auto type_branch = _module_resolver.find(node->module_type);

                if(type_branch == _module_resolver.end())
                {
                    emit_error<unresolved_module_reference_error>(errors, node->module_name, node->module_type);
                    return nullptr;
                }

                auto name_branch = type_branch->second.find(node->module_name);

                if(name_branch == type_branch->second.end())
                {
                    emit_error<unresolved_module_reference_error>(errors, node->module_name);
                    return nullptr;
                }


				return name_branch->second;
			};

			virtual Module<vreal> compile_v09(const json &source) const
			{
				gorynych::aligned_map<string, gorynych::shared_ptr<module_base<vreal, vint>>> modules;

				aggregate_error errors;

				auto entry_point    = build_amt(source, modules, errors);
				auto result         = emit_amt_module(entry_point, modules, errors);

				if(errors.errors.size() > 0)
					throw errors;

				return result;
			}
	};
}}
