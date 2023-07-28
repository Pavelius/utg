#include "nameable.h"
#include "stringbuilder.h"

#pragma once

enum ability_s : unsigned char {
	Cost, CostCount,
	Combat, CombatCount, Bombardment, BombardmentCount,
	AntiFighterBarrage, AntiFighterBarrageCount, SpaceCannon, SpaceCannonCount,
	Move, Production, Reinforcement, MaximumInOneLocation, CapacityShips, Capacity,
};
struct abilityi : nameable {
	void			getinfo(stringbuilder& sb) const;
};
