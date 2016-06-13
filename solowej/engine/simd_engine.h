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

#include <fstream>
#include "../math/vector.h"
#include "../util/serializable.h"
#include "../../dependencies/json/src/json.hpp"

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