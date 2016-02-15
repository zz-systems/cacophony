#pragma once
#ifndef PARANOISE_CAPABILITY
#define PARANOISE_CAPABILITY
#include <algorithm>
#include <iostream>

using namespace std;

namespace paranoise { namespace parallel
{
	struct capability
	{
		bool has_sse2;
		bool has_sse3;
		bool has_ssse3;

		bool has_sse41;
		bool has_sse42;

		bool has_fma3;
		bool has_fma4;

		bool has_avx1;
		bool has_avx2;
		bool has_avx512;

		bool has_bmi1;
		bool has_bmi2;

		bool use_xrstore;

		bool has_gpgpu;

		capability()
		{
			int cpuInfo[4];
			__cpuid(cpuInfo, 1);

			has_sse2	= cpuInfo[3] & (1 << 26)	|| false;
			has_sse3	= cpuInfo[2] & (1 << 0)		|| false;
			has_ssse3	= cpuInfo[2] & (1 << 9)		|| false;
			
			has_sse41	= cpuInfo[2] & (1 << 19)	|| false;
			has_sse42	= cpuInfo[2] & (1 << 20)	|| false;

			has_fma3	= cpuInfo[2] & (1 << 12)	|| false;


			use_xrstore = cpuInfo[2] & (1 << 27)	|| false;

			has_avx1	= cpuInfo[2] & (1 << 28)	|| false;
			
			// Extended CPU info
			__cpuid(cpuInfo, 7);

			has_avx2	= cpuInfo[1] & (1 << 5)		|| false;
			has_avx512	= cpuInfo[1] & (1 << 16)	|| false;

			if (use_xrstore && has_avx1)
			{
				// Check if the OS will save the YMM registers
				unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
				has_avx1 = (xcrFeatureMask & 0x6) || false;
			}
		}

		friend ostream& operator<<(ostream& os, const capability& dt);
	};

	ostream& operator<<(ostream& os, const capability& cap)
	{
		os << "Has SSE2: "		<< cap.has_sse2 << endl;
		os << "Has SSE3: "		<< cap.has_sse3 << endl;
		os << "Has SSSE3: "		<< cap.has_ssse3 << endl;

		os << "Has SSE4.1: "	<< cap.has_sse41 << endl;
		os << "Has SSE4.2: "	<< cap.has_sse42 << endl;

		os << "Has FMA3: "		<< cap.has_fma3 << endl;

		os << "Uses XRSTORE: "	<< cap.use_xrstore << endl;

		os << "Has AVX1: "		<< cap.has_avx1 << endl;
		os << "Has AVX2: "		<< cap.has_avx2 << endl;
		os << "Has AVX512: "	<< cap.has_avx512 << endl;

		return os;
	}
}}
#endif

