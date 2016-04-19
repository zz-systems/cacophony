#include "stdafx.h"
#include "CppUnitTest.h"

#include "../paranoise/parallel/all.h"
#include "../paranoise/interpolation.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace zzsystems { namespace paranoise { namespace tests {
	using namespace interpolation;
	using namespace simdal;
	using namespace unittest;

	TEST_CLASS(Tesvinterpolation)
	{
	public:
		
		TEST_METHOD(Test_InterpolateCubic)
		{
			//AreEqual(X, cerp<float>(1, 2, 3, 4, 5));

			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10		
			for (float n3 = -2;		n3 < 2;		n3 += 0.25)			// 16		
			for (float n2 = -50;	n2 < 50;	n2 += 15)			// 10			
			for (float n1 = -25;	n1 < 25;	n1 += 25)			// 5				
			for (float n0 = -1;		n0 < 1;		n0 += 0.25)			// 8
			{		
				auto ref = cerp<float>(n0, n1, n2, n3, a);

				AreEqual(ref, cerp<sse_real>(n0, n1, n2, n3, a));
				AreEqual(ref, cerp<avx_real>(n0, n1, n2, n3, a));
			}	
		}

	TEST_METHOD(Test_Lerp)
		{
			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10		
			for (float n1 = -25;	n1 < 25;	n1 += 10)			// 5				
			for (float n0 = -1;	n0 < 1;		n0 += 0.25)			// 8
			{				
				auto ref	= lerp<float>(n0, n1, a);				

				AreEqual(ref, lerp<sse_real>(n0, n1, a));
				AreEqual(ref, lerp<avx_real>(n0, n1, a));
			}	
		}

		TEST_METHOD(Test_SCurve3)
		{
			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10		
			{
				auto ref = scurve3<float>(a);

				AreEqual(ref, scurve3<sse_real>(a));
				AreEqual(ref, scurve3<avx_real>(a));
			}	
		}

		TEST_METHOD(Test_SCurve5)
		{
			for (float a = -2.5;	a < 2.5;	a += 0.5)			// 10	
			{
				auto ref = scurve5<float>(a);

				AreEqual(ref, scurve5<sse_real>(a));
				AreEqual(ref, scurve5<avx_real>(a));				
			}	
		}
	};
}}}