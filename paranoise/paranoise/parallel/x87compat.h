#pragma once
#ifndef PARANOISE_X87_COMPAT
#define PARANOISE_X87_COMPAT


namespace paranoise { 
	// NEVER. EVER. include this in your own project if you don't want to override the default compariosn behavior
	namespace x87compat {
		template<typename T>
		inline int operator < (const T a, const T b) { return a < b ? 0xffffffff : 0x0; }

		template<typename T>
		inline int operator > (const T a, const T b) { return a > b ? 0xffffffff : 0x0; }

		template<typename T>
		inline int operator == (const T a, const T b) { return a == b ? 0xffffffff : 0x0; }

		template<typename T>
		inline T operator & (const T a, const T mask) { return a * (1 & mask); }

		template<typename T>
		inline T cselect(const bool condition, const T &a, const T &b) { return condition ? a : b; }
	}
}
#endif
