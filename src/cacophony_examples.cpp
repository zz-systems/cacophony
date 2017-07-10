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

#include <iostream>

#include "libcacophony.hpp"
#include "util/color.hpp"

#ifndef CACOPHONY_EXAMPLES_HEADLESS
#include "CImg.h"
#endif

namespace cacophony { namespace examples {
        using namespace zacc;
        using namespace util;

        inline void run(const int w, const int h,
                        const std::function<std::shared_ptr<std::vector<float>>(void)>& vector_algorithm,
        const std::function<std::shared_ptr<std::vector<float>>(void)>& scalar_algorithm,
        const util::gradient1D& gradient)
    {
        using namespace std;
        using namespace chrono;

        //PreFetchCacheLine()
#ifndef PROFILE
        auto sisd_t1 = high_resolution_clock::now();
        auto result2 = *scalar_algorithm();
        //vector<float> result2(512 * 512 * 1);
        auto sisd_t2 = high_resolution_clock::now();
        auto sisd_duration = duration_cast<milliseconds>(sisd_t2 - sisd_t1).count();

        cout << "Time: " << sisd_duration << "ms" << endl;
#endif
        auto simd_t1 = high_resolution_clock::now();

        auto result1 = *vector_algorithm();

        auto simd_t2 = high_resolution_clock::now();
        auto simd_duration = duration_cast<milliseconds>(simd_t2 - simd_t1).count();

        cout << "Time: " << simd_duration << "ms" << endl;
#ifndef PROFILE
        cout << "Speedup : " << (double)sisd_duration / simd_duration << endl << endl;
#endif
        cout << "Minval SIMD: " << *min_element(begin(result1), end(result1)) << endl << endl;
        cout << "Maxval SIMD: " << *max_element(begin(result1), end(result1)) << endl << endl;

#ifndef PROFILE
        cout << "Minval SISD: " << *min_element(begin(result2), end(result2)) << endl << endl;
        cout << "Maxval SISD: " << *max_element(begin(result2), end(result2)) << endl << endl;

#ifndef CACOPHONY_EXAMPLES_HEADLESS
        cimg_library::CImg<uint8_t> img1(w, h, 1, 3), img2(w, h, 1, 3);

        //async::parallel_for(async::irange(0, w * h), [&](const auto i)
#pragma omp parallel for
        for (int i = 0; i < w * h; i++)
    {
        int x = i % w;
        int y = i / w;


        auto color1 = gradient.getColor(result1[i]);

        img1(x, y, 0) = color1.r;
        img1(x, y, 1) = color1.g;
        img1(x, y, 2) = color1.b;

        auto color2 = gradient.getColor(result2[i]);

        img2(x, y, 0) = color2.r;
        img2(x, y, 1) = color2.g;
        img2(x, y, 2) = color2.b;
    }
    //);

    img1.display("simd", true);
    img2.display("sisd", true);
#endif

#endif

}

void run()
{
    auto sysinfo = &zacc::platform::instance();
    std::cout << *sysinfo;

    //platform e;

    //e.info.feature_flags = CAPABILITY_NONE;
    //sysinfo.setFlag(CAPABILITY_AVX2, false);
    //sysinfo.setFlag(CAPABILITY_AVX1, false);
    //e.info.setFlag(CAPABILITY_SSE42, false);
    //e.info.setFlag(CAPABILITY_SSE41, false);
    //e.info.setFlag(CAPABILITY_FMA3, false);
    //e.info.setFlag(CAPABILITY_FMA4, false);

    // Restrict with extracted bitmask
    auto flags = sysinfo->raw();
    //sysinfo->set(capabilities::AVX2, false);
    //sysinfo->set(capabilities::AVX1, false);
    //sysinfo->set(capabilities::SCALAR, false);


    std::cout << *sysinfo;

    //cacophony_get_engine("examples")->.feature_flags &= sysinfo->raw();
    cacophony_get_engine("examples")->compile_file("config/planet.json");

    //auto flags = cacophony_get_engine("examples")->info.feature_flags;

    auto d = cacophony_get_engine("examples")->get_config().dimensions;
    std::cerr << d.x << " " << d.y << " " << d.z << std::endl;
    auto f1 = [&]()
    {
        //std::vector<float, aligned_allocator<float, 32>> t(w * h * 1);
        auto t = zacc::make_shared<std::vector<float>>(d.x * d.y * d.z);

        sysinfo->reload();
        //sysinfo->set(capabilities::AVX2, false);
        //sysinfo->set(capabilities::AVX1, false);
        //sysinfo->set(capabilities::FMA3, false);

        //::cacophony_get_engine("examples")->info.feature_flags = flags;
        ::cacophony_run("examples", &t->at(0), 0, 0, 0);

        return t;
    };

    auto f2 = [&]()
    {
        auto t = zacc::make_shared<std::vector<float>>(d.x * d.y * d.z);

        sysinfo->set(0);
        //::cacophony_get_engine("examples")->info->s.feature_flags = (CAPABILITY_NONE);
        ::cacophony_run("examples", &t->at(0), 0, 0, 0);

        return t;
    };

    gradient1D grad
            {
                    { -1.01f, color_rgb(0, 0, 0, 255) },
                    { -0.9375f, color_rgb(0,   0,   0, 255) },
                    { -0.8750f, color_rgb(44, 216, 242, 255) },
                    { 0.0000f, color_rgb(191, 191, 191, 255) },
                    { 0.5000f, color_rgb(33, 116, 125, 255) },
                    { 0.7500f, color_rgb(45, 113,  98, 255) },
                    { 1.01f, color_rgb(255, 176, 192, 255) }
            };

    cacophony::examples::run(d.x, d.z, f1, f2, grad);
}
}}

int main(int argc, char** argv)
{
    cacophony::examples::run();

    return 0;
}
