#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Assault"},
	{"Comunication"},
	{"Exploration"},
	{"Medicine"},
	{"Hunting"},
	{"Research"},
};
assert_enum(abilityi, Research)

BSDATA(statei) = {
	{"Crew"},
	{"Discontent", FG(NegativeState)},
	{"Supply"},
	{"Machinery"},
	{"Reroll"},
	{"Problem", FG(NegativeState)},
	{"Success"},
	{"Advantage"},
};
assert_enum(statei, Advantage)