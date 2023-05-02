#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellect"},
	{"Wisdow"},
	{"Charisma"},
	{"MeleeAttack"},
	{"MeleeDamage"},
	{"RangeAttack"},
	{"RangeDamage"},
	{"SpellRoll"},
	{"AC"},
	{"Level"},
	{"HP"},
};
assert_enum(abilityi, HP)