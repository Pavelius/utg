#include "ability.h"
#include "bsdata.h"

BSDATA(abilityi) = {
	{"Cost"},
	{"Limit"},
	{"LimitInProvince"},
	{"Recruit"},
	{"Tools"},
	{"Strenght"},
	{"Combat"},
	{"Damage"},
	{"Shoots"},
	{"Shield"},
	{"Hirelings"},
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

abilityn last_ability;