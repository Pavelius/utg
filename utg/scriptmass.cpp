#include "script.h"
#include "scriptmass.h"

template<> void fnscript<scriptmass>(int value, int bonus) {
	script::apply = bsdata<scriptmass>::elements[value].proc;
}