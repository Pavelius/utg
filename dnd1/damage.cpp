#include "main.h"

BSDATA(damagei) = {
	{"Blundgeon"},
	{"Slashing"},
	{"Pierce"},
	{"Fire"},
	{"Cold"},
	{"Electric"},
	{"Poison", PoisonImmunity},
	{"Magic"},
	{"Cure"},
};
assert_enum(damagei, Cure)