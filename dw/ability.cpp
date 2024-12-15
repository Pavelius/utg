#include "ability.h"
#include "bsdata.h"

BSDATA(abilityi) = {
	{"HP"},
	{"Damage"},
	{"Armor"},
	{"Pierce"},
	{"Load"},
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellegence"},
	{"Wisdow"},
	{"Charisma"},
	{"FightingFocus"},
	{"TravelRole"},
};
assert_enum(abilityi, TravelRole)

BSDATA(abilityseti) = {
	{"FightingStyles"},
	{"SpellDomains"},
};
assert_enum(abilityseti, SpellDomains)

abilityn last_ability;