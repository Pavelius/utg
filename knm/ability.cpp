#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Cost"},
	{"Strenght"},
	{"Melee"},
	{"Shoot"},
	{"Messy"},
	{"MeleeDefence"},
	{"ShootDefence"},
	{"Recruit"},
	{"Reinforcement"},
	{"Resources"},
	{"Influence"},
	{"Trade"},
	{"Champion"},
	{"General"},
	{"Hero"},
	{"Lore"},
	{"Fame"},
	{"Goods"},
};
assert_enum(abilityi, Goods);