#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellegence"},
	{"Wisdow"},
	{"Charisma"},
	{"Fortitude"},
	{"Reflex"},
	{"Will"},
	{"Armor"},
	{"MaxDexterityBonus"},
	{"DodgeBonus"},
	{"EquipmentBonus"},
	{"SneakAttackBonus"},
	{"DemolitionistBonus"},
	{"IndomitableBonus"},
	{"ConditionTrack"},
	{"GamblingCheck"},
	{"HyperdrivenPerDay"},
	{"RerollAttackPerDay"},
	{"SecondWindPerDay"},
	{"ForcePoints"},
	{"ForcePointsPerEncounter"},
	{"Reaction"},
	{"SwiftAction"},
	{"MoveAction"},
	{"StandartAction"},
	{"InitiativeResult"},
	{"Relation"},
};
assert_enum(abilityi, LastAbility)

void abilityable::clear() {
	memset(this, 0, sizeof(*this));
}

ability_s abilityable::getuse(ability_s v) const {
	switch(v) {
	case SwiftAction:
		if(abilities[SwiftAction] > 0)
			return SwiftAction;
		if(abilities[MoveAction] > 0)
			return MoveAction;
		if(abilities[StandartAction] > 0)
			return StandartAction;
		return {};
	case MoveAction:
		if(abilities[MoveAction] > 0)
			return MoveAction;
		if(abilities[StandartAction] > 0)
			return StandartAction;
		return {};
	default:
		if(abilities[v] > 0)
			return v;
		return {};
	}
}

void useaction(ability_s v);