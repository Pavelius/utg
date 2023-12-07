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
	{"AllDefenceBonuses"},
	{"AllSkillChecks"},
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
	{"InitiativeResult"},
	{"Relation"},
};
assert_enum(abilityi, LastAbility)

void abilityable::clear() {
	memset(this, 0, sizeof(*this));
}