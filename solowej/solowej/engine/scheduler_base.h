//
// Created by szuyev on 16.05.16.
//

#pragma once

#include "../../json/src/json.hpp"
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