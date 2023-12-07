#pragma once

#include "nameable.h"

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
	Fortitude, Reflex, Will, Armor,
	AllDefenceBonuses, AllSkillChecks,
	MaxDexterityBonus, DodgeBonus, EquipmentBonus, SneakAttackBonus, DemolitionistBonus, IndomitableBonus,
	ConditionTrack,
	GamblingCheck,
	HyperdrivenPerDay, RerollAttackPerDay, SecondWindPerDay,
	ForcePoints, ForcePointsPerEncounter,
	InitiativeResult, Relation,
	LastAbility = Relation
};
struct abilityi : nameable {
};
struct abilityable {
	char abilities[LastAbility + 1];
	void clear();
};
