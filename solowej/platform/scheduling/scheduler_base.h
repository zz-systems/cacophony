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

#include <vector>
#include <memory>

#include "../../../dependencies/gorynych/gorynych/gorynych.h"
#include "../../util/serializable.h"
#include "../../modules/module_base.h"


namespace zzsystems { namespace solowej { namespace platform {
    using namespace gorynych;
    using namespace modules;
    using json = nlohmann::json;

    struct scheduler_config : serializable<json>
    {
        vec3<int> dimensions, seam_dimensions;
        vec3<float> scale, offset;
        bool make_seam;

        virtual void deserialize(const json &source) override
        {
            auto dimensions = source["dimensions"];
            auto scale      = source["scale"];
            auto offset     = source["offset"];

            this->dimensions = this->seam_dimensions = dimensions != nullptr && dimensions.size() >= 3
                                                       ? vec3<int>(dimensions[0].get<int>(), dimensions[1].get<int>(), dimensions[2].get<int>())
                                                       : vec3<int>(128, 128, 1);


            this->scale		 = scale != nullptr && scale.size() >= 3
                                  ? vec3<float>(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>())
                                  : vec3<float>(1);

            this->offset = offset != nullptr && offset.size() >= 3
                           ? vec3<float>(offset[0].get<float>(), offset[1].get<float>(), offset[2].get<float>())
                           : vec3<float>(0);

            this->make_seam     = source.value<bool>("make_seam", true);

            if(make_seam)
            {
                seam_dimensions.x = vmax(seam_dimensions.x - 1, 1);
                seam_dimensions.y = vmax(seam_dimensions.y - 1, 1);
                seam_dimensions.z = vmax(seam_dimensions.z - 1, 1);
            }

            cout << "dimensions: [ " << this->dimensions.x << ", " << this->dimensions.y << ", " << this->dimensions.z << " ]" << endl;
            cout << "scale: [ " << this->scale.x << ", " << this->scale.y << ", " << this->scale.z << " ]" << endl;
            cout << "offset: [ " << this->offset.x << ", " << this->offset.y << ", " << this->offset.z << " ]" << endl;
        }
    };

    DISPATCHED class ALIGNED scheduler_base :
        public gorynych::platform::scheduler_base<capability>,
        public serializable<json>
    {
        using vreal = typename static_dispatcher<capability>::vreal;
        using vint  = typename static_dispatcher<capability>::vint;

    protected:
        scheduler_config config;

    public:

        const scheduler_config &get_config()
        {
            return config;
        }

        virtual void deserialize(const json &source) override
        {
            config << source;

            transform = build_transform(transform);
        }

        virtual float *operator()(const vec3<float> &origin) /*const*/ override
		{
			auto d = config.dimensions;
			//auto target = new float[d.x * d.y * d.z];

			gorynych::aligned_allocator<float, 32> alloc;

			//float* target;// = alloc.allocate(d.x * d.y * d.z);
			float* target = reinterpret_cast<float*>(aligned_malloc(sizeof(float) * d.x * d.y * d.z, 64));
			this->schedule(target, origin);

			return target;
		}

        Module<vreal>		source;
        Transformer<vreal>	transform;

        virtual void set_source(Module<vreal> source)
        {
            this->source = source;
        }

        virtual void set_transform(Transformer<vreal> transformer)
        {
            this->transform = transformer;
        }
    protected:
        Transformer<vreal> build_transform(Transformer<vreal> transformer)
        {
            cout << "building input transform..." << endl;

            cout << boolalpha << "has seam: " << config.make_seam << endl;

            auto scale              = config.scale;
            auto offset             = config.offset;
            auto dimensions         = config.dimensions;
            auto seam_dimensions    = config.seam_dimensions;

            if (static_cast<bool>(transformer))
            {
                if (scale != vec3<float>(1) && offset != vec3<float>(0))
                {
                    cout << "user defined transformer: scaled, biased" << endl;
                    return [seam_dimensions, scale, offset, transformer](const auto&c) { return transformer(c) / static_cast<vec3<vreal>>(seam_dimensions) * static_cast<vec3<vreal>>(scale) + static_cast<vec3<vreal>>(offset); };
                }
                if (scale != vec3<float>(1))
                {
                    cout << "user defined transformer: scaled" << endl;
                    return [seam_dimensions, scale, transformer](const auto&c) { return transformer(c) / static_cast<vec3<vreal>>(seam_dimensions) * static_cast<vec3<vreal>>(scale); };
                }
                if (offset != vec3<float>(0))
                {
                    cout << "user defined transformer: biased" << endl;
                    return [seam_dimensions, offset, transformer](const auto&c) { return transformer(c) / static_cast<vec3<vreal>>(seam_dimensions) + static_cast<vec3<vreal>>(offset); };
                }
            }
            else
            {
                if (scale != vec3<float>(1) && offset != vec3<float>(0))
                {
                    cout << "default transformer: scaled, biased" << endl;
                    return [seam_dimensions, scale, offset](const auto&c) { return c / static_cast<vec3<vreal>>(seam_dimensions) * static_cast<vec3<vreal>>(scale) + static_cast<vec3<vreal>>(offset); };
                }
                if (scale != vec3<float>(1))
                {
                    cout << "default transformer: scaled" << endl;
                    return [seam_dimensions, scale](const auto&c) { return c / static_cast<vec3<vreal>>(seam_dimensions) * static_cast<vec3<vreal>>(scale); };
                }
                if (offset != vec3<float>(0))
                {
                    cout << "default transformer: biased" << endl;
                    return [seam_dimensions, offset](const auto&c) { return c / static_cast<vec3<vreal>>(seam_dimensions) + static_cast<vec3<vreal>>(offset); };
                }
            }

            cout << "default transformer" << endl;
            return [seam_dimensions](const auto &c)  { return c / static_cast<vec3<vreal>>(seam_dimensions); };
        }
    };
}}}