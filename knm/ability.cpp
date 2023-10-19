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
	{"Milita"},
	{"Move"},
	{"Resources"},
	{"Influence"},
	{"Goods"},
	{"Hero"},
	{"Army"},
	{"Tactic"},
	{"Fame"},
	{"Gold"},
};
assert_enum(abilityi, Gold);

ability_s last_ability;