#include "ability.h"
#include "bsdata.h"

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
	{"Movement"},
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
	{"Position"},
	{"Relation"},
};
assert_enum(abilityi, LastAbility)

abilityn last_ability;

void abilityable::clear() {
	memset(this, 0, sizeof(*this));
}

bool abilityable::isallow(abilityn v, int count) const {
	switch(v) {
	case SwiftAction: return (abilities[SwiftAction] + abilities[MoveAction] + abilities[StandartAction]) >= count;
	case MoveAction: return (abilities[MoveAction] + abilities[StandartAction]) >= count;
	default: return abilities[v] >= count;
	}
}