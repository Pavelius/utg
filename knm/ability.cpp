#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Cost"},
	{"Recruit"},
	{"Limit"},
	{"Strenght"},
	{"Combat"},
	{"Damage"},
	{"Shoots"},
	{"Shield"},
	{"Resources"},
	{"Influence"},
	{"Trade"},
	{"Tactic"},
	{"Army"},
	{"Lore"},
	{"Fame"},
	{"Commodities"},
	{"Goods"},
};
assert_enum(abilityi, Goods);