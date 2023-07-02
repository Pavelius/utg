#include "querry.h"
#include "script.h"

template<> void fnscript<querryi>(int value, int bonus) {
	bsdata<querryi>::elements[value].proc(bonus);
}