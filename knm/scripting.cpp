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

static void pay_hero(int bonus) {
}

static void add_leaders(int bonus) {
	player->add(Hero, 1);
	player->add(Champion, 1);
	player->add(General, 1);
}

static void add_actions(int bonus) {
}

static void add_research(int bonus) {
}

static void add_goods(int bonus) {
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

static void pay_for_leaders(int bonus) {
}

static void pay_research(int bonus) {
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
	{"AddActions", add_actions},
	{"AddGoods", add_goods},
	{"AddLeaders", add_leaders},
	{"AddResearch", add_research},
	{"AttackerMove", attacker_move},
	{"DefenderMove", defender_move},
	{"ForEachPlayer", for_each_player},
	{"PayForLeaders", pay_for_leaders},
	{"PayHero", pay_hero},
	{"PayResearch", pay_research},
	{"PickStrategy", pick_strategy},
};
BSDATAF(script)