#include "ability.h"
#include "creature.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"

template<> void fnscript<modifieri>(int value, int bonus) {
	modifier = (modifier_s)value;
}

template<> void fnscript<abilityi>(int value, int bonus) {
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += bonus; break;
	default: player->abilities[value] += bonus; break;
	}
}

static void choose_creature(int bonus) {
	player = creatures.choose(0, getnm("Cancel"));
}

BSDATA(script) = {
	{"ChooseCreature", choose_creature},
};
BSDATAF(script)