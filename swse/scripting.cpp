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

static bool allow_combat() {
	for(auto p : creatures) {
		if(p->is(Hostile))
			return true;
	}
	return false;
}

static void choose_creature(int bonus) {
	player = creatures.choose(0, getnm("Cancel"));
}

static void choose_opponent(int bonus) {
	opponent = creatures.choose(0);
}

static bool allow_make_melee_attack(int bonus) {
	return true;
}
static void make_melee_attack(int bonus) {

}

BSDATA(script) = {
	{"ChooseCreature", choose_creature},
	{"ChooseOpponent", choose_opponent},
	{"MakeMeleeAttack", make_melee_attack},
};
BSDATAF(script)