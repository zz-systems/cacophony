//
// Created by szuyev on 16.05.16.
//

#pragma once

#include <fstream>
#include "../math/vector.h"
#include "../util/serializable.h"
#include "../../json/src/json.hpp"

namespace zzsystems { namespace solowej {
    using namespace math;

    class simd_engine : serializable<nlohmann::json>
    {
    public:
        const nlohmann::json &operator<<(const nlohmann::json &source) override
        {
            // Build modules
            compile(source);

            return source;
        }

        // COmpile from file
        virtual void compile_file(const std::string &path)
        {
            std::ifstream file(path);
            nlohmann::json source(file);

            compile(source);
        }
        // Compile from immediate string
        virtual void compile_imm_str(const std::string &content)
        {
            auto source = nlohmann::json::parse(content);

            compile(source);
        }

        // COmpile from json object
        virtual void compile(const nlohmann::json &source) = 0;

        // Run - generating an array inside
        virtual float* run(const vec3<float> &origin) = 0;

        // Run in place
        virtual void run(const vec3<float> &origin, float *target) = 0;
    };
}}