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

#include "json.hpp"

#include "math/fundamental.hpp"
#include "math/linear/generic_matrix.hpp"
#include "math/linear/specialized_matrix.hpp"

#include "util/serializable.h"

namespace cacophony { namespace platform
{
    using namespace zacc;
    using namespace zacc::math;
    using json = nlohmann::json;

    struct scheduler_config : serializable<json> {
        vec3<int> dimensions, seam_dimensions;
        vec3<float> scale, offset;
        size_t num_threads;
        bool make_seam;

        virtual void deserialize(const json &source) override {
            auto dimensions = source["dimensions"];
            auto scale = source["scale"];
            auto offset = source["offset"];

            this->dimensions = this->seam_dimensions = dimensions != nullptr && dimensions.size() >= 3
                                                       ? vec3<int>(dimensions[0].get<int>(),
                                                                   dimensions[1].get<int>(),
                                                                   dimensions[2].get<int>())
                                                       : vec3<int>(128, 128, 1);


            this->scale = scale != nullptr && scale.size() >= 3
                          ? vec3<float>(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>())
                          : vec3<float>(1);

            this->offset = offset != nullptr && offset.size() >= 3
                           ? vec3<float>(offset[0].get<float>(), offset[1].get<float>(), offset[2].get<float>())
                           : vec3<float>(0);


            num_threads = source.value<size_t>("num_threads", -1);
            if (num_threads < 1)
                num_threads = zacc::platform::instance().num_threads();

            this->make_seam = source.value<bool>("make_seam", true);
            if (make_seam) {
                seam_dimensions.x = std::max(seam_dimensions.x - 1, 1);
                seam_dimensions.y = std::max(seam_dimensions.y - 1, 1);
                seam_dimensions.z = std::max(seam_dimensions.z - 1, 1);
            }
        }
    };
}}