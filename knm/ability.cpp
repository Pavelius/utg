#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Cost"},
	{"Limit"},
	{"LimitInProvince"},
	{"Recruit"},
	{"Strenght"},
	{"Combat"},
	{"Damage"},
	{"Shoots"},
	{"Shield"},
	{"Move"},
	{"Resources"},
	{"Influence"},
	{"Goods"},
	{"Hero"},
	{"Army"},
	{"Lore"},
	{"Fame"},
	{"Gold"},
};
assert_enum(abilityi, Gold);

ability_s last_ability;