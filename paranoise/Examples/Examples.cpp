// Examples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>

#ifndef PROFILE
#include "lib/CImg-1.6.9/CImg.h"
#endif

#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"

#include "run.h"
#include "wood.h"
#include "granite.h"
#include "planet.h"
#include "run.h"
#include "../paranoise/capabiliy.h"

using namespace paranoise;
using namespace parallel;
using namespace module;
using namespace scheduler;
using namespace util;
using namespace examples;

using namespace std;

int main(int argc, char** argv)
{
	capability c;

	cout << c;
	

	//_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

#ifdef PROFILE
	const int w = 2048;//4096; //8192 * 2;
	const int h = 2048;//4096; //8192 * 2;
#else 
	const int w = 4096; //8192 * 2;
	const int h = 4096; //8192 * 2;
#endif

	using vfloat = float4;
	using vint = int4;

	using fvec = Vector3<vfloat>;
	using vec = Vector3<float>;
	

	scheduler_settings settings({ w, h, 1 }, 1337, true);	

	auto f1 = [=]()
	{
		return schedule2D<vfloat>(
			generate_granite<vfloat, vint>(),
			[w, h](const fvec &c) { return (c)* fvec(10, 10, 10) / fvec(w, h, h) - fvec(5, 5, 5);},
			settings);
	};

	auto f2 = [=]() 
	{
		return schedule2D<float>(
			generate_granite<float, int>(),
			[w, h](const vec &c) { return (c)* vec(10, 10, 10) / vec(w, h, h) - vec(5, 5, 5);},
			settings);
	};

	run(w, h, f1, f2, granite_grad);

    return 0;
}


