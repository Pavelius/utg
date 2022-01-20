#include "main.h"

BSDATA(tagi) = {
	{"Explosion", {Gear}},
	{"HeavyWeapon", {Gear}},
	{"Tought", {Gear}},
	{"Regular", {Injury, Gear}},
	{"Veteran", {Injury, Morale}},
	{"Elite", {Injury, Gear}},
	{"Vehicle", {Armor}},
	{"BadSpot"}, // Бонус -1 на один следующий бросок до конца сцены
	{"Opportunity"}, // Бонус +1 на один следующий бросок до конца сцены
	{"Hiding"}, // Скрыт из виду.
	{"Covered"},
	{"Illuminated"},
	{"Intimate"},
	{"Close"},
	{"Far"},
};
assert_enum(tagi, Far)