#pragma once

#ifndef PARANOISE_GENERIC_OPERATORS
#define PARANOISE_GENERIC_OPERATORS

#include "base.h"

namespace zzsystems { 	
	
	ANY(T) ASSIGN_OP(+=, T) { return a = (a + b); }
	ANY(T) ASSIGN_OP(-=, T) { return a = (a - b); }
	ANY(T) ASSIGN_OP(*=, T) { return a = (a * b); }
	ANY(T) ASSIGN_OP(/=, T) { return a = (a / b); }	

	ANY(T) ASSIGN_OP(&=, T) { return a = (a & b); }
	ANY(T) ASSIGN_OP(|=, T) { return a = (a | b); }
	ANY(T) ASSIGN_OP(^=, T) { return a = (a ^ b); }

	ANY(T) ASSIGN_OP2(>>=, T, int) { return a = a >> b; }
	ANY(T) ASSIGN_OP2(<<=, T, int) { return a = a << b; }

	ANY(T) BIN_OP(>=, T) { return !(a < b); }
	ANY(T) BIN_OP(<=, T) { return !(a > b); }
}

#endif
