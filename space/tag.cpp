#include "main.h"

BSDATA(tagi) = {
	{"BadSpot"}, // Бонус -1 на один следующий бросок до конца сцены
	{"Opportunity"}, // Бонус +1 на один следующий бросок до конца сцены
	{"Armored"},
	{"Explosion"},
	{"HeavyWeapon"},
	{"Smashing"},// Inflict gear as additional harm
	{"Tricky"}, // Inflict exhause as additional harm
	{"Agile"}, // +1 exhause
	{"Tought"},
	{"Leader"},
	{"Regular"},
	{"Veteran"},
	{"Elite"},
	{"Rifle"},
	{"Vehicle"},
	{"Broken"},
	{"Covered"},
	{"Illuminated"},
	{"Hiding"}, // Скрыт из виду.
	{"Intimate"},
	{"Close"},
	{"Far"},
};
assert_enum(tagi, Far)