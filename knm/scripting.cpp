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

struct pushtitle {
	const char* header;
	const char* resid;
	pushtitle(const char* id) : header(answers::header), resid(answers::resid) {
		answers::header = getnm(id);
		answers::resid = player->id;
	}
	~pushtitle() {
		answers::header = header;
		answers::resid = resid;
	}
};

template<> void fnscript<abilityi>(int value, int counter) {
	player->abilities[value] += counter;
}

template<> void fnscript<uniti>(int value, int counter) {
}

static void addan(entity& e) {
	an.add(&e, e.getname());
}

static bool no_player(const void* object) {
	auto p = (entity*)object;
	return p->player == 0;
}

static bool filter_player(const void* object) {
	auto p = (entity*)object;
	return p->player == player;
}

static bool filter_province(const void* object) {
	auto p = (entity*)object;
	return p->getprovince() == province;
}

static void pay_hero(int bonus) {
}

static void add_leaders(int bonus) {
	player->add(Tactic, 2);
	player->add(Army, 1);
}

static void add_actions(int bonus) {
}

static void add_research(int bonus) {
}

static void add_secret_goal(int bonus) {
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

static void choose_strategy(fnvisible proc, bool keep) {
	pushtitle push("ChooseStrategy");
	an.clear();
	for(auto& e : bsdata<strategyi>()) {
		if(proc(&e) != keep)
			continue;
		addan(e);
	}
	last_strategy = (strategyi*)an.choose();
}

static void choose_strategy(int bonus) {
	choose_strategy(no_player, bonus >= 0);
}

static void select_players(int bonus) {
	for(auto& e : bsdata<playeri>())
		players.add(&e);
}

static void select_your_provincies(int bonus) {
	querry.collectiona::select(bsdata<provincei>::source, filter_player, bonus >= 0);
}

static void select_your_provincies(fnvisible proc, int bonus) {
	for(auto& e : bsdata<provincei>())
		querry.add(&e);
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
	{"AddSecretGoal", add_secret_goal},
	{"ChooseStrategy", choose_strategy},
	{"ForEachPlayer", for_each_player},
	{"ForEachProvince", for_each_province},
	{"PayForLeaders", pay_for_leaders},
	{"PayHero", pay_hero},
	{"PayResearch", pay_research},
	{"PickStrategy", pick_strategy},
	{"SelectPlayersBySpeaker", select_players},
	{"SelectYourProvincies", select_your_provincies},
};
BSDATAF(script)