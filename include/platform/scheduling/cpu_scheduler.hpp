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

#include <type_traits>

namespace cacophony { namespace platform {
    using namespace zacc;
    using namespace modules;

    DISPATCHED class alignas(branch::alignment) cpu_scheduler :
        public scheduler_base<branch>
    {
        using base_t = scheduler_base<branch>;
    public:
        virtual void schedule(float *target, const vec3<float> &origin) /*const*/ override
        {
            //execute<float>(target, origin);

            auto d          = this->config().dimensions;
            auto start      = this->transform(static_cast<vec3<zfloat>>(origin));
            auto real_size  = d.x * d.y * d.z;

            std::fill(target, target + real_size, -10);

            zacc::generate<zfloat>(target, target + real_size, [this, &start](auto i)
            {
                auto point = this->transform(this->build_coords(i));

                return this->source(point + start);
            });
        }

        virtual void schedule(int *target, const vec3<float> &origin) /*const*/ override
        {
            //execute<int>(target, origin);

            auto d          = this->config().dimensions;
            auto start      = this->transform(static_cast<vec3<zfloat>>(origin));
            auto real_size  = d.x * d.y * d.z;

            std::fill(target, target + real_size, -10);

            zacc::generate<zint>(target, target + real_size, [this, &start](auto i)
            {
                auto point = this->transform(this->build_coords(i));

                return static_cast<zint>(this->source(point + start).floor());
            });
        }
    private:
        template<typename T>
        void execute2(std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value, T> *target,
                     const vec3<float> &origin) {

            size_t word         = dim<zfloat>();
            auto d              = this->config().dimensions;
            auto num_threads    = this->config().num_threads;

            if(d.z >= d.y)
            {
                #pragma omp parallel for num_threads(num_threads)
                for(int z = 0; z < d.z; z++)
                {
                    _MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
                    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
                    auto depth = z * d.y;

                    for(int y = 0; y < d.y; y++)
                    {
                        this->process_chunk<T>(target, origin, d, depth, y, z);
                    }
                }
            }
            else
            {
                for(int z = 0; z < d.z; z++)
                {
                    auto depth = z * d.y;

                    #pragma omp parallel for num_threads(num_threads)
                    for(int y = 0; y < d.y; y++)
                    {
                        _MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
                        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

                        this->process_chunk<T>(target, origin, d, depth, y, z);
                    }
                }
            }
        }

        template<typename T>
        void execute(std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value, T> *target,
                     const vec3<float> &origin) {

            auto d          = this->config().dimensions;
            auto real_size  = d.x * d.y * d.z;
            auto remainder  = real_size % zfloat::dim;
            auto size       = real_size + remainder;


            size_t i = 0;

            for(; i < size; i += zfloat::dim)
            {
                auto x = i % d.x;
                auto y = (i / d.x) % d.y;
                auto z = i / (d.x * d.y);

                auto point = this->transform(this->build_coords(x, y, z))
                           + this->transform(static_cast<vec3<zfloat>>(origin));

                auto word = i < real_size ? zfloat::dim : remainder;

                if(std::is_same<T, int>::value)
                {
                    zint r = vround(this->source(point));

                    std::copy(r.begin(), r.begin() + word, target + i);
                }
                else if(std::is_same<T, float>::value)
                {
                    zfloat r = this->source(point);

                    std::copy(r.begin(), r.begin() + word, target + i);
                }
            }
        }

        template<typename T>
        void process_chunk(std::enable_if_t<std::is_same<T, int>::value || std::is_same<T, float>::value, T> *target,
                           const vec3<float> &origin, const vec3<int> &dimensions, int depth, int y, int z) {

            auto word       = dim<zfloat>();
            T* stride       = target + ((depth + y) * dimensions.x);
            auto remainder  = dimensions.x % word;
            auto adjust     = remainder > 0 ? word - remainder : 0;

            for (size_t x = 0; x < dimensions.x; x += word)
            {
                auto coords = this->transform(
                        this->build_coords(
                                static_cast<zfloat>(static_cast<int>(x)),
                                static_cast<zfloat>(y),
                                static_cast<zfloat>(z)));

                ZTRACE("scheduler coords: " << coords);

                if(std::is_same<T, int>::value)
                {
                    zint r = vround(this->source(static_cast<vec3<zfloat>>(origin) + coords));
                    this->write_result(stride, dimensions, remainder, x, r);
                }
                else if(std::is_same<T, float>::value)
                {
                    zfloat r = this->source(static_cast<vec3<zfloat>>(origin) + coords);
                    this->write_result(stride, dimensions, remainder, x, r);
                }
            }
        }

        void write_result(int *stride, const vec3<int> &dimensions, size_t remainder, int x, zint result)
        {
            base_t::write_result(dimensions, stride, remainder, x, result);
        }

        void write_result(float *stride, const vec3<int> &dimensions, size_t remainder, int x, zfloat result)
        {
            base_t::write_result(dimensions, stride, remainder, x, result);
        }
    };
}}