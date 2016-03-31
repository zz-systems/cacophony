#pragma once
#ifndef PARANOISE_CONSTANTS
#define PARANOISE_CONSTANTS

#include "parallel/all.h"


namespace zzsystems { namespace paranoise { namespace constants
{
	using namespace simdal;
	
#define EVALUATED_CONST(type, name, value) \
	static type name() { return value; }

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
		EVALUATED_CONST(TType, _0, 0);		

		static TType ones()		{ return 0xFFFF'FFFF; }
		static TType intmin()	{ return 0x8000'0001; }
		static TType intmax()	{ return 0x7FFF'FFFF; }

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

	template <>
	inline float fastload<float>::ones()
	{
		return numeric_limits<float>::quiet_NaN();
	}

	template<typename featuremask>
	struct fastload<_int4>
	{
		static _int4 _0()	  { return _mm_setzero_si128(); }
		static _int4 ones()	  { return _0() == _0(); }
		static _int4 intmin() { return -(ones >> 1); }
		static _int4 intmax() { return ones() >> 1; }

		static _int4 _1() { return ones() >> 31; }
		static _int4 _2() { return ones() >> 31 << 1; }
		static _int4 _3() { return ones() >> 30; }
		static _int4 _4() { return ones() >> 31 << 2; }
		static _int4 _5() { return _4() + _1(); }
		static _int4 _6() { return ones() >> 30 << 1; }
		static _int4 _7() { return ones() >> 29; }
		static _int4 _8() { return ones() >> 31 << 3; }

		static _int4 _9()  { return _8() + _1(); }
		static _int4 _10() { return _8() + _2(); }
		static _int4 _11() { return _8() + _3(); }
		static _int4 _12() { return _8() + _4(); }
		static _int4 _13() { return _8() + _5(); }
		static _int4 _14() { return _8() + _6(); }
		static _int4 _15() { return ones() >> 28; }

		static _int4 sign1all0() { return ones() << 31; }
		static _int4 sign0all1() { return ones() >> 1; }
	};

	template<typename featuremask>
	struct fastload<_float4>
	{
		static _float4 _0()  { return fastload<_int4>::_0(); }
		static _float4 ones() { return _mm_castsi128_ps(fastload<_int4>::ones().val); }

		static _float4  intmin() { return -(ones >> 1); }
		static _float4  intmax() { return ones() >> 1; }

		static _float4 _1() { return fastload<_int4>::_1(); }
		static _float4 _2() { return fastload<_int4>::_2(); }
		static _float4 _3() { return fastload<_int4>::_3(); }
		static _float4 _4() { return fastload<_int4>::_4(); }
		static _float4 _5() { return fastload<_int4>::_5(); }
		static _float4 _6() { return fastload<_int4>::_6(); }
		static _float4 _7() { return fastload<_int4>::_7(); }
		static _float4 _8() { return fastload<_int4>::_8(); }

		static _float4 _9()	{ return fastload<_int4>::_9(); }
		static _float4 _10() { return fastload<_int4>::_10(); }
		static _float4 _11() { return fastload<_int4>::_11(); }
		static _float4 _12() { return fastload<_int4>::_12(); }
		static _float4 _13() { return fastload<_int4>::_13(); }
		static _float4 _14() { return fastload<_int4>::_14(); }
		static _float4 _15() { return fastload<_int4>::_15(); }

		static _float4 sign1all0() { return _mm_castsi128_ps(fastload<_int4>::sign1all0().val); }
		static _float4 sign0all1() { return _mm_castsi128_ps(fastload<_int4>::sign0all1().val); }
	};

	template<typename featuremask>
	struct fastload<_int8>
	{
		static _int8 _0() { return _mm256_setzero_si256(); }
		static _int8 ones() { return _0() == _0(); }
		static _int8 intmin() { return -(ones >> 1); }
		static _int8 intmax() { return ones() >> 1; }

		static _int8 _1() { return ones() >> 31; }
		static _int8 _2() { return ones() >> 31 << 1; }
		static _int8 _3() { return ones() >> 30; }
		static _int8 _4() { return ones() >> 31 << 2; }
		static _int8 _5() { return _4() + _1(); }
		static _int8 _6() { return ones() >> 30 << 1; }
		static _int8 _7() { return ones() >> 29; }
		static _int8 _8() { return ones() >> 31 << 3; }

		static _int8 _9() { return _8() + _1(); }
		static _int8 _10() { return _8() + _2(); }
		static _int8 _11() { return _8() + _3(); }
		static _int8 _12() { return _8() + _4(); }
		static _int8 _13() { return _8() + _5(); }
		static _int8 _14() { return _8() + _6(); }
		static _int8 _15() { return ones() >> 28; }

		static _int8 sign1all0() { return ones() << 31; }
		static _int8 sign0all1() { return ones() >> 1; }
	};

	template<typename featuremask>
	struct fastload<_float8>
	{
		static _float8 _0() { return fastload<_int8>::_0(); }
		static _float8 ones() { return _mm_castsi128_ps(fastload<_int8>::ones().val); }

		static _float8  intmin() { return -(ones >> 1); }
		static _float8  intmax() { return ones() >> 1; }

		static _float8 _1() { return fastload<_int8>::_1(); }
		static _float8 _2() { return fastload<_int8>::_2(); }
		static _float8 _3() { return fastload<_int8>::_3(); }
		static _float8 _4() { return fastload<_int8>::_4(); }
		static _float8 _5() { return fastload<_int8>::_5(); }
		static _float8 _6() { return fastload<_int8>::_6(); }
		static _float8 _7() { return fastload<_int8>::_7(); }
		static _float8 _8() { return fastload<_int8>::_8(); }

		static _float8 _9() { return fastload<_int8>::_9(); }
		static _float8 _10() { return fastload<_int8>::_10(); }
		static _float8 _11() { return fastload<_int8>::_11(); }
		static _float8 _12() { return fastload<_int8>::_12(); }
		static _float8 _13() { return fastload<_int8>::_13(); }
		static _float8 _14() { return fastload<_int8>::_14(); }
		static _float8 _15() { return fastload<_int8>::_15(); }

		static _float8 sign1all0() { return _mm_castsi128_ps(fastload<_int8>::sign1all0().val); }
		static _float8 sign0all1() { return _mm_castsi128_ps(fastload<_int8>::sign0all1().val); }
	};
}}}

#endif
