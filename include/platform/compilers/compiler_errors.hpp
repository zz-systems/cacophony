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

#include <exception>
#include <string>

namespace cacophony { namespace platform {

    /// @name error codes & exceptions
    /// @{

    /// @brief The error codes the compiler can emit through the C-API
    enum compiler_error_codes {
        NONE,                   ///< No error. Dummy 0
        GENERIC_ERROR,          ///< Generic unknown error
        AGGREGATE_ERROR,        ///< Aggregate error. Check other bits.
        INVALID_MODULE,         ///< Invalid module configuration
        UNRESOLVED_MODULE_REF,  ///< Unresolved module reference (e.g wrong module name, type)
        UNRESOLVED_NODE_REF,    ///< Unresolved node reference (e.g wrong name)
        DUPLICATE_NODE,         ///< Duplicate node (e.g the same name twice)
        INVALID_ENTRY_POINT,    ///< Invalid entry point: No entry point found
        CYCLIC_MODULE_REF       ///< A cycle was found.
    };

    /// @brief basic compilation error
    struct compilation_error : public std::runtime_error {
        compilation_error() : runtime_error("Compilation error") {}

        compilation_error(std::string message) : runtime_error("Compilation error: " + message) {}

        virtual int error_code() const { return GENERIC_ERROR; }
    };

    /// @brief Aggregate error. Check nested errors
    struct aggregate_error : public std::exception {
        std::vector <std::shared_ptr<exception>> errors;
        std::vector <std::shared_ptr<compilation_error>> compilation_errors;

        aggregate_error() : std::exception() {}

        void push_back(std::shared_ptr <std::exception> ex) {
            errors.push_back(ex);
        }

//            void push_back(shared_ptr<compilation_error> ex)
//            {
//                compilation_errors.push_back(ex);
//            }

        virtual int error_code() const {
            int result = AGGREGATE_ERROR;
            for (auto err : compilation_errors)
                result |= err->error_code();

            return result;
        }
    };

    /// @brief Invalid module configuration error
    struct invalid_module_error : public compilation_error {
        invalid_module_error(std::string module_name) : compilation_error("Faulty module [" + module_name + "]") {}

        invalid_module_error(std::string module_name, std::string reason) : compilation_error(
                "Faulty module [" + module_name + "]: " + reason) {}

        virtual int error_code() const {
            return INVALID_MODULE;
        }
    };

    /// @brief Unresolved module reference (e.g wrong module name, type)
    struct unresolved_module_reference_error : public compilation_error {
        unresolved_module_reference_error(std::string module_name) : compilation_error(
                "Unresolved module reference [" + module_name + "]") {}

        unresolved_module_reference_error(std::string module_name, std::string module_type) : compilation_error(
                "Unresolved module reference [" + module_type + "][" + module_name + "]") {}

        virtual int error_code() const {
            return UNRESOLVED_MODULE_REF;
        }
    };

    /// @brief Unresolved node reference (e.g wrong name)
    struct unresolved_node_reference_error : public compilation_error {
        unresolved_node_reference_error(std::string module_name) : compilation_error(
                "Unresolved node reference [" + module_name + "]") {}

        virtual int error_code() const {
            return UNRESOLVED_NODE_REF;
        }
    };

    /// @brief Duplicate node (e.g the same name twice)
    struct duplicate_module_error : public compilation_error {
        duplicate_module_error(std::string module_name) : compilation_error(
                "Duplicate module [" + module_name + "] definition") {}

        virtual int error_code() const {
            return DUPLICATE_NODE;
        }
    };

    /// @brief Invalid entry point: No entry point found
    struct invalid_entry_point_error : public compilation_error {
        invalid_entry_point_error() : compilation_error("Entry point not found") {}

        invalid_entry_point_error(std::string entry_point) : compilation_error(
                "Entry point not found [" + entry_point + "]") {}

        virtual int error_code() const {
            return INVALID_ENTRY_POINT;
        }
    };

    /// @brief A cycle was found.
    struct cyclic_module_ref_error : public compilation_error {
        cyclic_module_ref_error() : compilation_error("Cyclic reference found") {}

        cyclic_module_ref_error(std::string module_name) : compilation_error(
                "Cyclic reference found for [" + module_name + "]") {}

        virtual int error_code() const {
            return CYCLIC_MODULE_REF;
        }
    };

    /// @}
}}