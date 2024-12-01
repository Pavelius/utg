#pragma once

#include "feat.h"
#include "nameable.h"

enum abilityn : unsigned char {
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
	short		hpm; // Maximum hit points
	void		clear();
	bool		isallow(abilityn v, int count = 1) const;
	void		setability(abilityn v, int i) { abilities[v] = i; }
};
extern abilityn last_ability;