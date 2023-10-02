#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Cost"},
	{"Recruit"},
	{"Limit"},
	{"Strenght"},
	{"Combat"},
	{"LightDamage"},
	{"HeavyDamage"},
	{"CriticalDamage"},
	{"Shoots"},
	{"Resources"},
	{"Influence"},
	{"Trade"},
	{"Tactic"},
	{"Army"},
	{"Strategy"},
	{"Lore"},
	{"Fame"},
	{"Goods"},
};
assert_enum(abilityi, Goods);