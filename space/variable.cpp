#include "crt.h"
#include "script.h"
#include "variable.h"

variable* last_variable;

BSDATAC(variable, 32)

template<> void fnscript<variable>(int value, int bonus) {
	last_variable = bsdata<variable>::elements + value;
}