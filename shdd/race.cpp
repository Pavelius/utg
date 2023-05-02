#include "crt.h"
#include "race.h"

BSDATA(racei) = {
	{"NoRace"},
	{"Human"},
	{"Dwarf"},
	{"Elf"},
	{"HalfOrc"},
};
assert_enum(racei, HalfOrc)