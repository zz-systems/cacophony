#pragma once
#ifndef PARANOISE_CAPABILITY
#define PARANOISE_CAPABILITY
#include <algorithm>
#include <iostream>

using namespace std;



namespace paranoise { namespace parallel
{
	enum capabilities
	{
		CAPABILITY_NONE		= 0,

		CAPABILITY_SSE2		= 1 << 0,
		CAPABILITY_SSE3		= 1 << 1,
		CAPABILITY_SSSE3	= 1 << 2,
		
		CAPABILITY_SSE41	= 1 << 3,
		CAPABILITY_SSE42	= 1 << 4,
		
		CAPABILITY_FMA3		= 1 << 5,
		CAPABILITY_FMA4		= 1 << 6,

		CAPABILITY_AVX1		= 1 << 7,
		CAPABILITY_AVX2		= 1 << 8,
		CAPABILITY_AVX512	= 1 << 9,

		CAPABILITY_OPENCL	= 1 << 10,
		CAPABILITY_FPGA		= 1 << 11
	};

	struct capability
	{
		int c;

		capability()
		{
			c = CAPABILITY_NONE;

			int cpuInfo[4];
			__cpuid(cpuInfo, 1);

			// SSE2:
			c |= cpuInfo[3] & (1 << 26) ? CAPABILITY_SSE2 : CAPABILITY_NONE;
			
			// SSE3:
			c |= cpuInfo[2] & (1 << 0) ? CAPABILITY_SSE3 : CAPABILITY_NONE;
			// SSSE3
			c |= cpuInfo[2] & (1 << 9) ? CAPABILITY_SSSE3 : CAPABILITY_NONE;
			
			// SSE 4.1
			c |= cpuInfo[2] & (1 << 19) ? CAPABILITY_SSE41 : CAPABILITY_NONE;
			// SSE 4.2
			c |= cpuInfo[2] & (1 << 20) ? CAPABILITY_SSE42 : CAPABILITY_NONE;

			// FMA3
			c |= cpuInfo[2] & (1 << 12) ? CAPABILITY_FMA3 : CAPABILITY_NONE;


			bool use_xrstore = cpuInfo[2] & (1 << 27)	|| false;

			c |= cpuInfo[2] & (1 << 28) ? CAPABILITY_AVX1 : CAPABILITY_NONE;
			
			// Extended CPU info
			__cpuid(cpuInfo, 7);

			c |= cpuInfo[1] & (1 << 5) ? CAPABILITY_AVX2 : CAPABILITY_NONE;
			c |= cpuInfo[1] & (1 << 16) ? CAPABILITY_AVX512 : CAPABILITY_NONE;

			//if (use_xrstore && has_avx1)
			//{
			//	// Check if the OS will save the YMM registers
			//	unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
			//	has_avx1 = (xcrFeatureMask & 0x6) || false;
			//}
		}
		bool hasFlag(const capabilities c) const
		{
			return this->c & c;
		}
		bool setFlag(const capabilities c)
		{
			this->c |= c;
		}
		friend ostream& operator<<(ostream& os, const capability& dt);
	};

	ostream& operator<<(ostream& os, const capability& cap)
	{
		os << "Has SSE2: "		<< cap.hasFlag(CAPABILITY_SSE2) << endl;
		os << "Has SSE3: "		<< cap.hasFlag(CAPABILITY_SSE3)  << endl;
		os << "Has SSSE3: "		<< cap.hasFlag(CAPABILITY_SSSE3) << endl;

		os << "Has SSE4.1: "	<< cap.hasFlag(CAPABILITY_SSE41) << endl;
		os << "Has SSE4.2: "	<< cap.hasFlag(CAPABILITY_SSE42) << endl;

		os << "Has FMA3: "		<< cap.hasFlag(CAPABILITY_FMA3) << endl;

		//os << "Uses XRSTORE: "	<< cap.use_xrstore << endl;

		os << "Has AVX1: "		<< cap.hasFlag(CAPABILITY_AVX1) << endl;
		os << "Has AVX2: "		<< cap.hasFlag(CAPABILITY_AVX2) << endl;
		os << "Has AVX512: "	<< cap.hasFlag(CAPABILITY_AVX512) << endl;

		return os;
	}
}}
#endif

