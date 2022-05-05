#include "crt.h"
#include "main.h"

BSDATA(abilityi) = {
	{"Speed", FG(abilityi::Stat)},
	{"Sneak", FG(abilityi::Stat)},
	{"Fight", FG(abilityi::Stat)},
	{"Will", FG(abilityi::Stat)},
	{"Lore", FG(abilityi::Stat)},
	{"Luck", FG(abilityi::Stat)},
	{"Horror"},
	{"Combat"},
	{"Escape"},
	{"HorrorDamage"},
	{"CombatDamage"},
	{"Toughness"},
	{"Health", FG(abilityi::Indicator)},
	{"Sanity", FG(abilityi::Indicator)},
	{"Clue", FG(abilityi::Indicator)},
	{"Money", FG(abilityi::Indicator)},
	{"Focus"},
};
assert_enum(abilityi, Focus)

void abilitya::add(const abilitya& e) {
	for(unsigned i = 0; i < sizeof(abilities) / sizeof(abilities[0]); i++)
		abilities[i] += e.abilities[i];
}