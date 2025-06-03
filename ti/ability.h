#include "nameable.h"
#include "stringbuilder.h"

#pragma once

enum abilityn : unsigned char {
	Cost, CostCount,
	Combat, CombatCount, Bombardment, BombardmentCount,
	AntiFighterBarrage, AntiFighterBarrageCount, SpaceCannon, SpaceCannonCount,
	Move, Production, Reinforcement, MaximumInOneLocation, CapacityShips, Capacity,
};
struct abilityi : nameable {
};
