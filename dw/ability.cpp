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
	{"PaladinZeal"},
	{"DietyDemand"},
	{"DruidInstinct"},
	{"RangerReadiness"},
};
assert_enum(abilityi, RangerReadiness)

BSDATA(abilityseti) = {
	{"FightingStyles"},
	{"SpellDomains"},
};
assert_enum(abilityseti, SpellDomains)

abilityn last_ability;