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

#include "../../dependencies/json/src/json.hpp"
#include "../math/vector.h"
#include "../util/serializable.h"

namespace zzsystems { namespace solowej {
    using namespace gorynych;

        struct scheduler_settings :
                serializable<nlohmann::json>
        {
            vec3<int> dimensions, seam_dimensions;
            vec3<float> scale, offset;
            bool use_threads;
            bool seamless;

            scheduler_settings() = default;

            scheduler_settings(vec3<int> dimensions, bool use_threads = true)
                    : dimensions(dimensions), use_threads(use_threads)
            {}

            scheduler_settings(const scheduler_settings& rhs)
                    : dimensions(rhs.dimensions), seam_dimensions(rhs.seam_dimensions), use_threads(rhs.use_threads)
            {}

            const nlohmann::json& operator<<(const nlohmann::json& source) override
            {
                auto dimensions = source["dimensions"];
                auto scale  = source["scale"];
                auto offset = source["offset"];

                this->dimensions = this->seam_dimensions = dimensions != nullptr && dimensions.size() >= 3
                                   ? vec3<int>(dimensions[0].get<int>(), dimensions[1].get<int>(), dimensions[2].get<int>())
                                   : vec3<int>(128, 128, 1);



                this->scale		 = scale != nullptr && scale.size() >= 3
                                      ? vec3<float>(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>())
                                      : vec3<float>(1);

                this->offset = offset != nullptr && offset.size() >= 3
                               ? vec3<float>(offset[0].get<float>(), offset[1].get<float>(), offset[2].get<float>())
                               : vec3<float>(0);

                use_threads = source.value<bool>("use_threads", true);
                seamless = source.value<bool>("seamless", true);

                if(seamless)
                {
                    seam_dimensions.x = vmax(seam_dimensions.x - 1, 1);
                    seam_dimensions.y = vmax(seam_dimensions.y - 1, 1);
                    seam_dimensions.z = vmax(seam_dimensions.z - 1, 1);
                }

                cout << "dimensions: [ " << this->dimensions.x << ", " << this->dimensions.y << ", " << this->dimensions.z << " ]" << endl;
                cout << "scale: [ " << this->scale.x << ", " << this->scale.y << ", " << this->scale.z << " ]" << endl;
                cout << "offset: [ " << this->offset.x << ", " << this->offset.y << ", " << this->offset.z << " ]" << endl;

                cout << "multithreaded: " << boolalpha << use_threads << endl;
                return source;
            }
        };

}}