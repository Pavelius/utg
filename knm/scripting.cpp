#include "answers.h"
#include "army.h"
#include "crt.h"
#include "entitya.h"
#include "player.h"
#include "playera.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"
#include "strategy.h"
#include "unit.h"

template<> void fnscript<abilityi>(int value, int counter) {
	player->abilities[value] += counter;
}

template<> void fnscript<uniti>(int value, int counter) {
}

static void pay_hero(int bonus) {
}

static void add_leaders(int bonus) {
	player->add(Tactic, 1);
	player->add(Army, 1);
	player->add(Strategy, 1);
}

static void add_actions(int bonus) {
}

static void add_research(int bonus) {
}

static void add_goods(int bonus) {
}

static void combat_round(int bonus) {
	attacker.prepare(Shield);
	defender.prepare(Shield);
	attacker.engage(Damage);
	defender.engage(Damage);
	defender.suffer(attacker.get(Damage));
	attacker.suffer(defender.get(Damage));
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

static void for_each_province(int bonus) {
	pushvalue push(province);
	pushvalue push_querry(querry);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_querry.value) {
		province = static_cast<provincei*>(p);
		script_run(commands);
	}
	script_stop();
}

BSDATA(script) = {
	{"AddActions", add_actions},
	{"AddGoods", add_goods},
	{"AddLeaders", add_leaders},
	{"AddResearch", add_research},
	{"ForEachPlayer", for_each_player},
	{"ForEachProvince", for_each_province},
	{"PayForLeaders", pay_for_leaders},
	{"PayHero", pay_hero},
	{"PayResearch", pay_research},
	{"PickStrategy", pick_strategy},
};
BSDATAF(script)