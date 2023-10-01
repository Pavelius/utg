#include "answers.h"
#include "army.h"
#include "crt.h"
#include "player.h"
#include "playera.h"
#include "pushvalue.h"
#include "script.h"
#include "strategy.h"
#include "unit.h"

template<> void fnscript<abilityi>(int value, int counter) {
}

template<> void fnscript<uniti>(int value, int counter) {
}

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

static void for_each_player(int bonus) {
	pushvalue push_players(players);
	pushvalue push_player(player);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_players.value) {
		player = p;
		script_run(commands);
	}
	script_stop();
}

BSDATA(script) = {
	{"AddLeaders", add_leaders},
	{"AttackerMove", attacker_move},
	{"DefenderMove", defender_move},
	{"ForEachPlayer", for_each_player},
	{"PickStrategy", pick_strategy},
};
BSDATAF(script)