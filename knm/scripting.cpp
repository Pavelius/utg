#include "answers.h"
#include "army.h"
#include "crt.h"
#include "player.h"
#include "script.h"
#include "strategy.h"
#include "unit.h"

static void add_leaders(int bonus) {
	player->add(Hero, 1);
	player->add(Champion, 1);
	player->add(General, 1);
}

static void attacker_move(int bonus) {
	attacker.engage(defender);
	defender.engage(attacker);
}

static void defender_move(int bonus) {
	defender.engage(attacker);
}

static void pick_strategy(int bonus) {
	last_strategy->player = player;
}

BSDATA(script) = {
	{"AddLeaders", add_leaders},
	{"AttackerMove", attacker_move},
	{"DefenderMove", defender_move},
	{"PickStrategy", pick_strategy},
};