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
#include "../../modules/all.h"
#include "../../util/string.h"

#include <iostream>
#include <sstream>
#include <map>
#include <unordered_set>
#include <exception>
#include "compiler_base.h"

namespace zzsystems { namespace solowej { namespace engine {
    using namespace modules;

    DISPATCHED class compiler_v09 : public compiler_base<dispatch_mask>
    {
        using vreal = typename static_dispatcher<dispatch_mask>::vreal;
        using vint	= typename static_dispatcher<dispatch_mask>::vint;
    public:
        virtual const string version() const override { return "0.9"; }

        compiler_v09(bool aggregate_errors = false)
                : compiler_base<dispatch_mask>(aggregate_errors)
        {
            this->add_module_map<mod_perlin>              ("generator/perlin");
            this->add_module_map<mod_billow>              ("generator/billow");
            this->add_module_map<mod_voronoi>             ("generator/voronoi");
            this->add_module_map<mod_ridged_multifractal> ("generator/ridged");
            this->add_module_map<mod_spheres>             ("generator/spheres");
            this->add_module_map<mod_cylinders>           ("generator/cylinders");
            this->add_module_map<mod_checkerboard>        ("generator/checker");
            this->add_module_map<mod_const>               ("generator/const");

            this->add_module_map<mod_curve>               ("modifier/curve");
            this->add_module_map<mod_rotate>              ("modifier/rotate");
            this->add_module_map<mod_select>              ("modifier/select");
            this->add_module_map<mod_terrace>             ("modifier/terrace");
            this->add_module_map<mod_turbulence>          ("modifier/turbulence");
            this->add_module_map<mod_displace>            ("modifier/displace");

            this->add_module_map<mod_add>                 ("modifier/add");
            this->add_module_map<mod_sub>                 ("modifier/sub");
            this->add_module_map<mod_mul>                 ("modifier/mul");
            this->add_module_map<mod_div>                 ("modifier/div");

            this->add_module_map<mod_abs>                 ("modifier/abs");
            this->add_module_map<mod_min>                 ("modifier/min");
            this->add_module_map<mod_max>                 ("modifier/max");
            this->add_module_map<mod_clamp>               ("modifier/clamp");

            this->add_module_map<mod_blend>               ("modifier/blend");

            this->add_module_map<mod_translate_input>     ("modifier/translate_input");
            this->add_module_map<mod_scale_input>         ("modifier/scale_input");
            this->add_module_map<mod_scale_output>        ("modifier/scale_output");
            this->add_module_map<mod_scale_output_biased> ("modifier/scale_output_biased");
        }
    private:
        // Prepares a flat node list
        // Builds an abstract module tree
        virtual shared_ptr<amt_node> build_amt(compiler_stage<SIMD_T> &stage, const json &source) const override
        {
            shared_ptr<amt_node> entry_point;

            // Fetch entry point
            auto environment        = source["environment"];
            auto raw_entry_point    = environment["entryPoint"];
            string entry_point_name = raw_entry_point != nullptr ? raw_entry_point : "";

            bool has_entry_point    = entry_point_name != "";
            if(!has_entry_point)
                this->emit_error<invalid_entry_point_error>(stage);

            // parse modules partially (flat list)
            if (source["modules"].is_array())
            {
                for (auto module : source["modules"])
                {
                    auto node = make_shared<amt_node>();
                    string type = module["type"];
                    string name = module["module"];

                    node->instance_name 	= module["name"];
                    node->module_path 		= type + "/" + name;
                    node->module_config 	= module["settings"];
                    node->sources_config 	= module["source"];

                    // Check for duplicate declaration
                    if(stage.nodes.find(node->instance_name) != stage.nodes.end())
                        this->emit_error<duplicate_module_error>(stage, node->instance_name);
                    else
                        stage.nodes[node->instance_name] = node;
                }
            }

            // Check if entry node exists
            has_entry_point = has_entry_point && stage.nodes.find(entry_point_name) != stage.nodes.end();

            if(has_entry_point)
                entry_point = stage.nodes[entry_point_name];
            else
                this->emit_error<unresolved_node_reference_error>(stage, entry_point_name);

            // build tree with empty cycle check chain
            build_amt_node(stage, entry_point, unordered_set<shared_ptr<amt_node>>());

            return entry_point;
        }


        // Emits a real functor from abstract module tree node
        virtual Module<vreal> emit_amt_module(compiler_stage<SIMD_T> &stage, shared_ptr<amt_node> node) const override
        {
            auto node_module 	= stage.emitted[node->instance_name];
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
                this->emit_error<invalid_module_error>(stage, ex.what());
            }

            // Set sources
            for (size_t index = 0; index < node->references.size(); index++)
            {
                auto ref_node = node->references[index];

                target_modules->at(index) = emit_amt_module(stage, ref_node);
            }

            // Wrap exec operator
            return [node_module](const auto &coords) { return node_module->operator()(coords); };
        }


        // Builds a node tree from a flat config
        // Checks for cyclic references
        // Checks for existing refereces
        virtual void build_amt_node(compiler_stage<SIMD_T> &stage, shared_ptr<amt_node> node,
                                    unordered_set<shared_ptr<amt_node>> cycle_check // each stage should have it's own chain
        ) const override
        {
            // Check for cyclic references.
            // In case a cycle is present: abort. Otherwise the tree will grow indefinitely
            if(cycle_check.find(node) != cycle_check.end())
            {
                this->emit_error<cyclic_module_ref_error>(stage, node->instance_name);

                return; // don't even try to go deeper (cycle!)
            }
            else
            {
                cycle_check.insert(node);
            }


            auto skeleton = find_module(stage, node);

            // Check if the source count matches the module requirements
            bool has_sources = node->sources_config != nullptr && node->sources_config.is_array();
            if(has_sources && skeleton->required_module_count > 0 && node->sources_config.size() < skeleton->required_module_count)
            {
                this->emit_error<invalid_module_error>(stage, node->instance_name, "input module count mismatch");
            }
            else
            {
                stage.emitted[node->instance_name] = skeleton->clone();
            }

            // Resolve sources
            if (has_sources)
            {
                for (string ref_name : node->sources_config)
                {
                    // Check if the referenced node exists
                    if (stage.nodes.find(ref_name) == stage.nodes.end())
                    {
                        this->emit_error<unresolved_node_reference_error>(stage, ref_name);
                        continue;
                    }

                    // Set node references
                    auto ref_node = stage.nodes[ref_name];

                    ref_node->referenced_in.push_back(node);
                    node->references.push_back(ref_node);

                    // Build subtree
                    build_amt_node(stage, ref_node, cycle_check);
                }
            }
        }

        // Checks if the requested module skeleton [type, name] exists
        virtual shared_ptr<module_base<vreal, vint>> find_module(compiler_stage<SIMD_T> &stage, shared_ptr<amt_node> node)
        const override
        {
            auto iter = this->_module_skeletons.find(node->module_path);
            if(iter != this->_module_skeletons.end())
                return iter->second;
            else
            {
                this->emit_error<unresolved_module_reference_error>(stage, node->module_path);
                return nullptr;
            }
        };

        virtual Module<vreal> compile(const json &source) const override
        {
            compiler_stage<SIMD_T> stage;

            auto entry_point    = build_amt(stage, source);
            auto result         = emit_amt_module(stage, entry_point);

            if(stage.errors.errors.size() > 0)
                throw stage.errors;

            return result;
        }

        template<typename error, typename... Args>
        void emit_error(compiler_stage<SIMD_T> &stage, Args &&... args) const
        {
            auto err = std::make_shared<error>(std::forward<Args>(args)...);
            cerr << err->what() << endl;

            stage.errors.push_back(err);

            if(!this->_aggregate_errors)
                throw *err;
        }

        template<template <typename, typename> class module>
        void add_module_map(const string &path)
        {
            this->_module_skeletons[path] = gorynych::make_shared<module<SIMD_T>>();
        }
    };
}}}
