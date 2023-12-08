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
	{"FullRoundAction"},
	{"InitiativeResult"},
	{"Relation"},
};
assert_enum(abilityi, LastAbility)

ability_s last_ability;

void abilityable::clear() {
	memset(this, 0, sizeof(*this));
}

bool abilityable::isallow(ability_s v, int count) const {
	switch(v) {
	case SwiftAction:
		return (abilities[SwiftAction] + abilities[MoveAction] + abilities[StandartAction]) >= count;
	case MoveAction:
		return (abilities[MoveAction] + abilities[StandartAction]) >= count;
	default:
		return abilities[v] >= count;
	}
}