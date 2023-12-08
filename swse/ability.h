#pragma once

#include "nameable.h"

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
	Fortitude, Reflex, Will, Armor,
	MaxDexterityBonus, DodgeBonus, EquipmentBonus, SneakAttackBonus, DemolitionistBonus, IndomitableBonus,
	ConditionTrack,
	GamblingCheck,
	HyperdrivenPerDay, RerollAttackPerDay, SecondWindPerDay,
	ForcePoints, ForcePointsPerEncounter,
	Reaction, SwiftAction, MoveAction, StandartAction,
	InitiativeResult, Relation,
	LastAbility = Relation
};
struct abilityi : nameable {
};
struct abilityable {
	char		abilities[LastAbility + 1];
	void		clear();
	bool		isallow(ability_s v, int count = 1) const;
	void		setability(ability_s v, int i) { abilities[v] = i; }
};
extern ability_s last_ability;