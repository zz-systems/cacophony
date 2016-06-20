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

#include "scheduler_base.h"
#include "cpu_scheduler_mt.h"
#include "cpu_scheduler_st.h"

namespace zzsystems { namespace solowej { namespace platform {
    using namespace async;
    using namespace gorynych;
    using namespace modules;
    using json = nlohmann::json;

    DISPATCHED class ALIGNED scheduler :
        public scheduler_base<capability>
    {
        bool multithreaded;
        using vreal = typename static_dispatcher<capability>::vreal;
        using vint  = typename static_dispatcher<capability>::vint;
    public:
        scheduler()
        {
            scheduler_st    = gorynych::make_shared<cpu_scheduler_st<capability>>();
            scheduler_mt    = gorynych::make_shared<cpu_scheduler_mt<capability>>();
            //scheduler_gpu   = gorynych::make_shared<gpu_scheduler<capability>>();
        }

        virtual void deserialize(const json &source) override
        {
            multithreaded   = source.value<bool>("use_threads", true);

            this->config    << source;
            (*scheduler_st) << source;
            (*scheduler_mt) << source;
        }

        virtual void schedule(float *target, const vec3<float> &origin) /*const*/ override
        {
            if(multithreaded)
                scheduler_mt->schedule(target, origin);
            else
                scheduler_st->schedule(target, origin);
        }

        virtual void set_source(Module<vreal> source) override
        {
            scheduler_st->set_source(source);
            scheduler_mt->set_source(source);
        }

        virtual void set_transform(Transformer<vreal> transformer) override
        {
            scheduler_st->set_transform(transformer);
            scheduler_mt->set_transform(transformer);
        }
    private:
        shared_ptr<cpu_scheduler_st<capability>> scheduler_st;
        shared_ptr<cpu_scheduler_mt<capability>> scheduler_mt;
        //shared_ptr<gpu_scheduler<capability>>    scheduler_gpu;
    };
}}}