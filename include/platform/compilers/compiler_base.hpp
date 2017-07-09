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

#include "modules/all.hpp"
#include "util/collections.hpp"
#include "platform/dependencies.hpp"
#include "platform/compilers/compiler_errors.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <unordered_set>
#include <exception>

namespace cacophony { namespace platform {
        using namespace modules;

        /// @brief abstract module tree node
        /// Intermediate representation of the computation flow
        struct amt_node
        {
            std::string instance_name, module_path;

            amt_node() = default;

            amt_node(std::string instance_name, std::string module_path) :
                    instance_name(instance_name),
                    module_path(module_path) { }

            nlohmann::json module_config, sources_config;

            std::vector<std::shared_ptr<amt_node>> references;
            std::vector<std::shared_ptr<amt_node>> referenced_in;
        };

        /// @brief object exchanged between different compiler stages
        DISPATCHED
        struct compiler_stage
        {
            compiler_stage() = default;

            /// Flat node list
            std::map<std::string, std::shared_ptr<amt_node>> nodes;

            /// Flat emitted module list (e.g prepared modules which need to be brought in proper order)
            zacc::aligned_map<std::string, std::shared_ptr<module_base<branch>>> emitted;

            /// Error aggregator
            aggregate_error errors;
        };

        DISPATCHED class compiler_base
        {
        public:
            virtual Module<branch> compile(const json &source) const = 0;

        protected:
            compiler_base(bool aggregate_errors) : _aggregate_errors(aggregate_errors)
            {}

            std::map<std::string, std::shared_ptr<module_base<branch>>> _module_skeletons;

            virtual std::shared_ptr<amt_node>    build_amt(compiler_stage<branch> &stage, const json &source) const = 0;
            virtual Module<branch>           emit_amt_module(compiler_stage<branch> &stage, std::shared_ptr<amt_node> node) const = 0;

            virtual void build_amt_node(compiler_stage<branch> &stage, std::shared_ptr<amt_node> node,
                                        std::unordered_set<std::shared_ptr<amt_node>> cycle_check // each stage should have it's own chain
                                        ) const = 0;

            virtual std::shared_ptr<module_base<branch>> find_module(compiler_stage<branch> &stage, std::shared_ptr<amt_node> node) const = 0;


            bool _aggregate_errors; ///< If set, the compiler tries to continue and collects occurring errors
        };
}}
