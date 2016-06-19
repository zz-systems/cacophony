//
// Created by szuyev on 17.06.16.
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

namespace zzsystems { namespace solowej { namespace engine {
        using namespace modules;

        /// @brief abstract module tree node
        /// Intermediate representation of the computation flow
        struct amt_node
        {
            std::string instance_name, module_path;

            amt_node() = default;

            amt_node(string instance_name, string module_path) :
                    instance_name(instance_name),
                    module_path(module_path) { }

            nlohmann::json module_config, sources_config;

            std::vector<shared_ptr<amt_node>> references;
            std::vector<shared_ptr<amt_node>> referenced_in;
        };

        /// @name error codes & exceptions
        /// @{

        /// @brief The error codes the compiler can emit through the C-API
        enum compiler_error_codes
        {
            NONE,                   ///< No error. Dummy 0
            AGGREGATE_ERROR,        ///< Aggregate error. Check other bits.
            INVALID_MODULE,         ///< Invalid module configuration
            UNRESOLVED_MODULE_REF,  ///< Unresolved module reference (e.g wrong module name, type)
            UNRESOLVED_NODE_REF,    ///< Unresolved node reference (e.g wrong name)
            DUPLICATE_NODE,         ///< Duplicate node (e.g the same name twice)
            INVALID_ENTRY_POINT,    ///< Invalid entry point: No entry point found
            CYCLIC_MODULE_REF       ///< A cycle was found.
        };

        /// @brief basic compilation error
        struct compilation_error : public std::runtime_error
        {
            compilation_error() : runtime_error("Compilation error") { }

            compilation_error(string message) : runtime_error("Compilation error: " + message) { }

            virtual int error_code() const { return NONE; }
        };

        /// @brief Aggregate error. Check nested errors
        struct aggregate_error : public std::exception
        {
            vector<shared_ptr<exception>> errors;
            vector<shared_ptr<compilation_error>> compilation_errors;

            aggregate_error() : std::exception() { }

            void push_back(shared_ptr<exception> ex)
            {
                errors.push_back(ex);
            }

//            void push_back(shared_ptr<compilation_error> ex)
//            {
//                compilation_errors.push_back(ex);
//            }

            virtual int error_code() const
            {
                int result = AGGREGATE_ERROR;
                for (auto err : compilation_errors)
                    result |= err->error_code();

                return result;
            }
        };

        /// @brief Invalid module configuration error
        struct invalid_module_error : public compilation_error
        {
            invalid_module_error(string module_name) : compilation_error("Faulty module [" + module_name + "]") { }

            invalid_module_error(string module_name, string reason) : compilation_error(
                    "Faulty module [" + module_name + "]: " + reason) { }

            virtual int error_code() const
            {
                return INVALID_MODULE;
            }
        };

        /// @brief Unresolved module reference (e.g wrong module name, type)
        struct unresolved_module_reference_error : public compilation_error
        {
            unresolved_module_reference_error(string module_name) : compilation_error(
                    "Unresolved module reference [" + module_name + "]") { }

            unresolved_module_reference_error(string module_name, string module_type) : compilation_error(
                    "Unresolved module reference [" + module_type + "][" + module_name + "]") { }

            virtual int error_code() const
            {
                return UNRESOLVED_MODULE_REF;
            }
        };

        /// @brief Unresolved node reference (e.g wrong name)
        struct unresolved_node_reference_error : public compilation_error
        {
            unresolved_node_reference_error(string module_name) : compilation_error(
                    "Unresolved node reference [" + module_name + "]") { }

            virtual int error_code() const
            {
                return UNRESOLVED_NODE_REF;
            }
        };

        /// @brief Duplicate node (e.g the same name twice)
        struct duplicate_module_error : public compilation_error
        {
            duplicate_module_error(string module_name) : compilation_error(
                    "Duplicate module [" + module_name + "] definition") { }

            virtual int error_code() const
            {
                return DUPLICATE_NODE;
            }
        };

        /// @brief Invalid entry point: No entry point found
        struct invalid_entry_point_error : public compilation_error
        {
            invalid_entry_point_error() : compilation_error("Entry point not found") { }

            invalid_entry_point_error(string entry_point) : compilation_error(
                    "Entry point not found [" + entry_point + "]") { }

            virtual int error_code() const
            {
                return INVALID_ENTRY_POINT;
            }
        };

        /// @brief A cycle was found.
        struct cyclic_module_ref_error : public compilation_error
        {
            cyclic_module_ref_error() : compilation_error("Cyclic reference found") { }

            cyclic_module_ref_error(string module_name) : compilation_error(
                    "Cyclic reference found for [" + module_name + "]") { }

            virtual int error_code() const
            {
                return CYCLIC_MODULE_REF;
            }
        };

        /// @}


        /// @brief object exchanged between different compiler stages
        VECTORIZED
        struct compiler_stage
        {
            compiler_stage() = default;

            /// Flat node list
            std::map<string, shared_ptr<amt_node>> nodes;

            /// Flat emitted module list (e.g prepared modules which need to be brought in proper order)
            gorynych::aligned_map<string, gorynych::shared_ptr<module_base<vreal, vint>>> emitted;

            /// Error aggregator
            aggregate_error errors;
        };

        DISPATCHED class compiler_base
        {
            using vreal = typename static_dispatcher<dispatch_mask>::vreal;
            using vint	= typename static_dispatcher<dispatch_mask>::vint;
        public:
            virtual Module<vreal> compile(const json &source) const = 0;

//            template<template <typename, typename> class module>
//            void add_module_map(const string &path)
//            {
//                _module_skeletons[path] = gorynych::make_shared<module<SIMD_T>>();
//            }

            virtual const string version() const = 0;
        protected:
            compiler_base(bool aggregate_errors) : _aggregate_errors(aggregate_errors)
            {}

            std::map<string, shared_ptr<module_base<SIMD_T>>> _module_skeletons;

//            template<typename error, typename... Args>
//            void emit_error(aggregate_error &errors, Args &&... args) const
//            {
//                auto err = std::make_shared<error>(std::forward<Args>(args)...);
//                cerr << err->what() << endl;
//
//                errors.push_back(err);
//
//                if(!_aggregate_errors)
//                    throw *err;
//            }

            virtual shared_ptr<amt_node>    build_amt(compiler_stage<SIMD_T> &stage, const json &source) const = 0;
            virtual Module<vreal>           emit_amt_module(compiler_stage<SIMD_T> &stage, shared_ptr<amt_node> node) const = 0;

            virtual void build_amt_node(compiler_stage<SIMD_T> &stage, shared_ptr<amt_node> node,
                                        unordered_set<shared_ptr<amt_node>> cycle_check // each stage should have it's own chain
                                        ) const = 0;

            virtual shared_ptr<module_base<vreal, vint>> find_module(compiler_stage<SIMD_T> &stage, shared_ptr<amt_node> node) const = 0;


            bool _aggregate_errors; ///< If set, the compiler tries to continue and collects occurring errors
        };
    }}
}