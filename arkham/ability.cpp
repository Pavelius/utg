#include "crt.h"
#include "main.h"

BSDATA(abilityi) = {
	{"Speed"},
	{"Sneak"},
	{"Fight"},
	{"Will"},
	{"Lore"},
	{"Luck"},
	{"Horror"},
	{"Combat"},
	{"Escape"},
	{"HorrorDamage"},
	{"CombatDamage"},
	{"Toughness"},
	{"Health"},
	{"Sanity"},
	{"Clue"},
	{"Focus"},
};
assert_enum(abilityi, Focus)

void abilitya::add(const abilitya& e) {
	for(unsigned i = 0; i < sizeof(abilities) / sizeof(abilities[0]); i++)
		abilities[i] += e.abilities[i];
}