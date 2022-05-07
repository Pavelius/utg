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

void abilitya::addabilities(const abilitya& source) {
	for(unsigned i = 0; i < sizeof(abilities) / sizeof(abilities[0]); i++) {
		if(!bsdata<abilityi>::elements[i].is(abilityi::Indicator))
			abilities[i] += source.abilities[i];
	}
	rerollall.add(source.rerollall);
	doubleclue.add(source.doubleclue);
	tought.add(source.tought);
	restore.add(source.restore);
	pickextra.add(source.pickextra);
	scavenge.add(source.scavenge);
}

void abilitya::loadability(const abilitya& source) {
	for(auto i = 0; i <= Focus; i++) {
		if(!bsdata<abilityi>::elements[i].is(abilityi::Indicator))
			abilities[i] = source.abilities[i];
	}
	rerollall = source.rerollall;
	doubleclue = source.doubleclue;
	tought = source.tought;
	restore = source.restore;
	pickextra = source.pickextra;
	scavenge = source.scavenge;
}