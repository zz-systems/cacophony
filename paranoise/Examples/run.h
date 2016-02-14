#pragma once

#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"

namespace paranoise { namespace examples {

	SIMD_ENABLE(TReal, TInt)
		void run(const int w, const int h,
			const std::function<std::shared_ptr<std::vector<float>>()>& vector_algorithm, 
			const std::function<std::shared_ptr<std::vector<float>>()>& scalar_algorithm)
	{



		scheduler_settings settings({ w, h, 1 }, 1337, true);

		auto simd_t1 = high_resolution_clock::now();

		auto result1 = *vector_algorithm();

		auto simd_t2 = high_resolution_clock::now();
		auto simd_duration = duration_cast<milliseconds>(simd_t2 - simd_t1).count();

		cout << "PARACHUTE (optimized code + compiler optimization): " << simd_duration << "ms" << endl;

#ifndef PROFILE
		auto sisd_t1 = high_resolution_clock::now();

		auto result2 = *scalar_algorithm();

		auto sisd_t2 = high_resolution_clock::now();
		auto sisd_duration = duration_cast<milliseconds>(sisd_t2 - sisd_t1).count();

		cout << "Compiler auto-vectorized: " << sisd_duration << "ms" << endl;
		cout << "Speedup : " << (double)sisd_duration / simd_duration << endl << endl;

		cout << "Minval SIMD: " << *min_element(begin(v1), end(v1)) << endl << endl;
		cout << "Maxval SIMD: " << *max_element(begin(v1), end(v1)) << endl << endl;

		cout << "Minval SISD: " << *min_element(begin(v2), end(v2)) << endl << endl;
		cout << "Maxval SISD: " << *max_element(begin(v2), end(v2)) << endl << endl;

		cimg_library::CImg<uint8> img1(w, h, 1, 3), img2(w, h, 1, 3);

		parallel_for(0, w * h, [&](const auto i)
			//for (int i = 0; i < v.size(); i++)	
		{
			int x = i % w;
			int y = i / w;


			auto color1 = planet_grad.getColor(result1[i]);

			img1(x, y, 0) = color1.r;
			img1(x, y, 1) = color1.g;
			img1(x, y, 2) = color1.b;

			auto color2 = planet_grad.getColor(result2[i]);

			img2(x, y, 0) = color2.r;
			img2(x, y, 1) = color2.g;
			img2(x, y, 2) = color2.b;
		}
		);

		img1.display("simd", true);
		img2.display("sisd", true);
#endif

	}

}}
