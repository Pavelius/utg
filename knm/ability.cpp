#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Cost"},
	{"CostCount"},
	{"Melee"},
	{"MeleeShield"},
	{"Shoot"},
	{"ShootShield"},
	{"Production"},
	{"Reinforcement"},
};
assert_enum(abilityi, Reinforcement);