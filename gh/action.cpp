#include "main.h"

BSDATA(actioni) = {
	{"Shield"},
	{"Retaliate"},
	{"Move"},
	{"Attack"},
	{"Push"},
	{"Pull"},
	{"Heal"},
	{"DisarmTrap"},
	{"Loot"},
	{"Bless"},
	{"Curse"},
	{"RecoverDiscarded"},
	{"TrapDamage"}, {"TrapPoisonDamage"},
	{"Discard"},	
};
assert_enum(actioni, Discard)