#pragma once
#ifndef PARANOISE_CONSTANTS
#define PARANOISE_CONSTANTS
#include "basetypes.h"
#include "parallel/all.h"


namespace paranoise { namespace constants
{
	using namespace parallel;
	

	ANY(TType)
	struct consts
	{
		static CONSTDEF(TType, pi,		3.14159265358979323846);
		static CONSTDEF(TType, deg2rad, 0.01745329251994329577);
		static CONSTDEF(TType, sqrt3,	1.73205080756887729353);
	};

	ANY(TType)
	struct fastload
	{
		static TType _0()		{ return 0; }

		static TType ones()		{ return 0xFFFF'FFFF; }

		static TType _1()		{ return 1; }
		static TType _2()		{ return 2; }
		static TType _3()		{ return 3; }
		static TType _4()		{ return 4; }
		static TType _5()		{ return 5; }
		static TType _6()		{ return 6; }
		static TType _7()		{ return 7; }
		static TType _8()		{ return 8; }
		static TType _9()		{ return 9; }
		static TType _10()		{ return 10; }
		static TType _11()		{ return 11; }
		static TType _12()		{ return 12; }
		static TType _13()		{ return 13; }
		static TType _14()		{ return 14; }
		static TType _15()		{ return 15; }

		static TType sign1all0() { return 0x8000'0000; }
		static TType sign0all1() { return 0x7FFF'FFFF; }
	};
	

	template<typename featuremask>
	struct fastload<int4<featuremask>>
	{
		static int4<featuremask> _0() { return _mm_setzero_si128(); }
		static int4<featuremask> ones() { return _0() == _0(); }

		static int4<featuremask> _1() { return ones() >> 31; }
		static int4<featuremask> _2() { return ones() >> 31 << 1; }
		static int4<featuremask> _3() { return ones() >> 30; }
		static int4<featuremask> _4() { return ones() >> 31 << 2; }
		static int4<featuremask> _5() { return _4() + _1(); }
		static int4<featuremask> _6() { return ones() >> 30 << 2; }
		static int4<featuremask> _7() { return ones() >> 29; }
		static int4<featuremask> _8() { return ones() >> 31 << 3; }

		static int4<featuremask> _9()  { return _8() + _1(); }
		static int4<featuremask> _10() { return _8() + _2(); }
		static int4<featuremask> _11() { return _8() + _3(); }
		static int4<featuremask> _12() { return _8() + _4(); }
		static int4<featuremask> _13() { return _8() + _5(); }
		static int4<featuremask> _14() { return _8() + _6(); }
		static int4<featuremask> _15() { return ones() >> 28; }

		static int4<featuremask> sign1all0() { return ones() << 31; }
		static int4<featuremask> sign0all1() { return ones() >> 1; }
	};

	template<typename featuremask>
	struct fastload<float4<featuremask>>
	{
		static float4<featuremask> _0()  { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_0()); }
		static float4<featuremask> ones() { return _mm_castsi128_ps(fastload<int4<featuremask>>::ones().val); }

		static float4<featuremask> _1() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_1()); }
		static float4<featuremask> _2() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_2()); }
		static float4<featuremask> _3() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_3()); }
		static float4<featuremask> _4() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_4()); }
		static float4<featuremask> _5() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_5()); }
		static float4<featuremask> _6() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_6()); }
		static float4<featuremask> _7() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_7()); }
		static float4<featuremask> _8() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_8()); }

		static float4<featuremask> _9()	{ return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_9()); }
		static float4<featuremask> _10() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_10()); }
		static float4<featuremask> _11() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_11()); }
		static float4<featuremask> _12() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_12()); }
		static float4<featuremask> _13() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_13()); }
		static float4<featuremask> _14() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_14()); }
		static float4<featuremask> _15() { return static_cast<float4<featuremask>>(fastload<int4<featuremask>>::_15()); }

		static float4<featuremask> sign1all0() { return _mm_castsi128_ps(fastload<int4<featuremask>>::sign1all0().val); }
		static float4<featuremask> sign0all1() { return _mm_castsi128_ps(fastload<int4<featuremask>>::sign0all1().val); }
	};
}}

#endif
