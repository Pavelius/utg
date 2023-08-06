#include "crt.h"
#include "ability.h"

BSDATA(abilityi) = {
	{"Cost"},
	{"CostCount"},
	{"Combat"},
	{"CombatCount"},
	{"Bombardment"},
	{"BombardmentCount"},
	{"AntiFighterBarrage"},
	{"AntiFighterBarrageCount"},
	{"SpaceCannon"},
	{"SpaceCannonCount"},
	{"Move"},
	{"Production"},
	{"Reinforcement"},
	{"MaximumInOneLocation"},
	{"CapacityShips"},
	{"Capacity"},
};
assert_enum(abilityi, Capacity)