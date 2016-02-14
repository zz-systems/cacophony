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

using namespace paranoise;
using namespace paranoise::parallel;
using namespace paranoise::module;
using namespace paranoise::scheduler;
using namespace paranoise::util;

using namespace std;
using namespace std::placeholders;

using namespace std::chrono;


using namespace paranoise::examples;

int main()
{
	bool avxSupported = false;

	// If Visual Studio 2010 SP1 or later
#if (_MSC_FULL_VER >= 160040219)
	// Checking for AVX requires 3 things:
	// 1) CPUID indicates that the OS uses XSAVE and XRSTORE
	//     instructions (allowing saving YMM registers on context
	//     switch)
	// 2) CPUID indicates support for AVX
	// 3) XGETBV indicates the AVX registers will be saved and
	//     restored on context switch
	//
	// Note that XGETBV is only available on 686 or later CPUs, so
	// the instruction needs to be conditionally run.
	int cpuInfo[4];
	__cpuid(cpuInfo, 1);

	bool osUsesXSAVE_XRSTORE = cpuInfo[2] & (1 << 27) || false;
	bool cpuAVXSuport = cpuInfo[2] & (1 << 28) || false;

	if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
	{
		// Check if the OS will save the YMM registers
		unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
		avxSupported = (xcrFeatureMask & 0x6) || false;
	}
#endif

	if (avxSupported)
	{
		cout << ("AVX is supported") << endl;
	}
	else
	{
		cout << ("AVX is NOT supported\n") << endl;;
	}

	

	_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

#ifdef PROFILE
	const int w = 2048;//4096; //8192 * 2;
	const int h = 2048;//4096; //8192 * 2;
#else 
	const int w = 4096; //8192 * 2;
	const int h = 4096; //8192 * 2;
#endif

	run()
	auto result1 = schedule2D<float4>(
		generate_complex_planet<float4, int4>(),
		//[w, h](const fvec &c) { return (c)* fvec(180, 180, 180) / fvec(w, h, h) - fvec(90, 90, 90);},
		[w, h](const fvec &c) { return (c) * fvec(10, 10, 10) / fvec(w, h, h) - fvec(5, 5, 5);},
		settings);

	auto result2 = schedule2D<float>(
		generate_complex_planet<float, int>(),
		//[w, h](const vec &c) { return (c)* vec(180, 180, 180) / vec(w, h, h) - vec(90, 90, 90);},
		[w, h](const vec &c) { return (c)* vec(10, 10, 10) / vec(w, h, h) - vec(5, 5, 5);},
		settings);


    return 0;
}


