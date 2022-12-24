#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Assault", FG(Ability)},
	{"Comunication", FG(Ability)},
	{"Exploration", FG(Ability)},
	{"Medicine", FG(Ability)},
	{"Hunting", FG(Ability)},
	{"Pilot", FG(Ability)},
	{"Research", FG(Ability)},
	{"Crew"},
	{"Discontent", FG(NegativeState)},
	{"Supply"},
	{"Machinery"},
	{"Reroll"},
	{"Problem", FG(NegativeState)},
	{"Success"},
	{"Advantage"},
};
assert_enum(abilityi, Advantage)