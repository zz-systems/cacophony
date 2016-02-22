#pragma once
//#define PROFILE
#ifndef PROFILE

#include "lib/CImg-1.6.9/CImg.h"

#endif

#include "run.h"
#include <chrono>
#include <ppl.h>


#include <functional>
#include <memory>
#include <vector>
#include "../paranoise/color.h"

namespace paranoise { namespace examples {
	using namespace std;

	inline void run(const int w, const int h,
		const function<shared_ptr<vector<float>>(void)>& vector_algorithm,
		const function<shared_ptr<vector<float>>(void)>& scalar_algorithm,
		const util::gradient1D& gradient)
	{
		using namespace std;
		using namespace chrono;

		//PreFetchCacheLine()
#ifndef PROFILE
		cout << endl << "Normal: " << endl;
		auto sisd_t1 = high_resolution_clock::now();


		auto result2 = *scalar_algorithm();

		auto sisd_t2 = high_resolution_clock::now();
		auto sisd_duration = duration_cast<milliseconds>(sisd_t2 - sisd_t1).count();

		cout << "Compiler optimized: " << sisd_duration << "ms" << endl;
#endif
		cout << endl << "Parachute/Paranoise:" << endl;
		auto simd_t1 = high_resolution_clock::now();

		auto result1 = *vector_algorithm();

		auto simd_t2 = high_resolution_clock::now();
		auto simd_duration = duration_cast<milliseconds>(simd_t2 - simd_t1).count();

		cout << "PARACHUTE (optimized code + compiler optimization): " << simd_duration << "ms" << endl;
#ifndef PROFILE
		cout << "Speedup : " << (double)sisd_duration / simd_duration << endl << endl;
#endif
		cout << "Minval SIMD: " << *min_element(begin(result1), end(result1)) << endl << endl;
		cout << "Maxval SIMD: " << *max_element(begin(result1), end(result1)) << endl << endl;		

#ifndef PROFILE
		cout << "Minval SISD: " << *min_element(begin(result2), end(result2)) << endl << endl;
		cout << "Maxval SISD: " << *max_element(begin(result2), end(result2)) << endl << endl;

		cimg_library::CImg<uint8> img1(w, h, 1, 3), img2(w, h, 1, 3);

		Concurrency::parallel_for(0, w * h, [&](const auto i)
			//for (int i = 0; i < v.size(); i++)	
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
		);

		img1.display("simd", true);
		img2.display("sisd", true);
#endif

	}

}}
