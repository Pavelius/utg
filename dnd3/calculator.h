#pragma once

enum condition_s : unsigned char {
	NoCondition,
	AbilityRoll, AbilityGreater, AbilityLesser,
	ItemEquiped, ItemEquipedSpecial,
	OpponentRace,
};
enum conditionf_s : unsigned char {
	ConditionInvert, ConditionOr,
};
struct conditioni {
	condition_s		type;
	unsigned char	flags;
	short			ability, item, number;
	bool			is(conditionf_s v) const;
};
