#include "calculator.h"
#include "crt.h"

bool conditioni::is(conditionf_s v) const {
	return (flags & FG(v)) != 0;
}