#include "answers.h"
#include "army.h"
#include "card.h"
#include "crt.h"
#include "entitya.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "pushtitle.h"
#include "pushvalue.h"
#include "script.h"
#include "strategy.h"
#include "unit.h"

static char log_text[260];
static stringbuilder log(log_text);
static listi* last_list;

template<> void fnscript<abilityi>(int value, int counter) {
	player->current.abilities[value] += counter;
}

template<> void fnscript<uniti>(int value, int counter) {
}

static bool allow_input(variant v);

static bool allow_input(const variants& source) {
	for(auto v : source) {
		if(!allow_input(v))
			return false;
	}
	return true;
}

static bool allow_input(variant v) {
	if(v.iskind<listi>())
		return allow_input(bsdata<listi>::elements[v.value].elements);
	else if(v.iskind<cardi>())
		return allow_input(bsdata<cardi>::elements[v.value].effect);
	else if(v.iskind<abilityi>()) {
		if(v.counter < 0) {
			if(player->get((ability_s)v.value) < -v.counter)
				return false;
		}
	} else if(v.iskind<script>())
		return fntest<script>(v.value, v.counter);
	return true;
}

static void add_input(variant v) {
	if(!allow_input(v))
		return;
	an.add(v.getpointer(), v.getname());
}

static void add_input(const char* trigger) {
	if(!trigger)
		return;
	for(auto& e : bsdata<cardi>()) {
		if(e.trigger != trigger || e.usedeck())
			continue;
		if(e.player && e.player != player)
			continue;
		add_input(&e);
	}
}

static void add_input_cards(const char* trigger) {
	for(auto& e : bsdata<cardi>()) {
		if(e.player && e.player != player)
			continue;
		if(e.usedeck())
			continue;
		if(strcmp(e.id, trigger) != 0)
			continue;
		add_input(&e);
	}
}

static void apply_input() {
	auto result = an.choose();
	if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else if(bsdata<playeri>::have(result))
		player = (playeri*)result;
	else if(bsdata<strategyi>::have(result))
		last_strategy = (strategyi*)result;
	else if(bsdata<cardi>::have(result)) {
		last_card = (cardi*)result;
		script_run(last_card->effect);
	} else if(bsdata<listi>::have(result)) {
		last_list = (listi*)result;
		script_run(((listi*)result)->elements);
	}
}

static bool no_player(const void* object) {
	auto p = (entity*)object;
	return p->player == 0;
}

static bool filter_activated(const void* object) {
	auto p = (entity*)object;
	return p->is(player);
}

static void filter_activated_querry(int bonus) {
	querry.match(filter_activated, bonus >= 0);
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
	pushtitle push("AddLeaders");
	if(!bonus)
		bonus = 1;
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
		add_input(&e);
	}
	apply_input();
}

static void clear_input(int bonus) {
	an.clear();
}

static void input_querry(int bonus) {
	for(auto p : querry)
		add_input(p);
}

static void choose_input(int bonus) {
	apply_input();
}

static void choose_querry(int bonus) {
	an.clear();
	input_querry(bonus);
	choose_input(bonus);
}

static void make_action(int bonus) {
	pushtitle push("MakeAction");
	an.clear();
	apply_input();
}

static void choose_strategy(int bonus) {
	choose_strategy(no_player, bonus >= 0);
}

static void select_players(int bonus) {
	for(auto& e : bsdata<playeri>())
		players.add(&e);
}

static void select_strategy(int bonus) {
	querry.collectiona::select(bsdata<strategyi>::source, no_player, bonus >= 0);
}

static void select_provincies(int bonus) {
	querry.collectiona::select(bsdata<provincei>::source, filter_player, bonus >= 0);
}

static void select_your_provincies(int bonus) {
	querry.collectiona::select(bsdata<provincei>::source, filter_player, bonus >= 0);
}

static void establish_control(int bonus) {
	if(bonus >= 0)
		province->player = player;
	else
		province->player = 0;
}

static strategyi* find_strategy(const playeri* player) {
	for(auto& e : bsdata<strategyi>()) {
		if(e.player == player)
			return &e;
	}
	return 0;
}

static strategyi* find_active_strategy(const playeri* player) {
	for(auto& e : bsdata<strategyi>()) {
		if(e.is(Used))
			continue;
		if(e.player == player)
			return &e;
	}
	return 0;
}

static bool allow_end_round(int bonus) {
	return !player->is(Used) && !find_active_strategy(player);
}
static void end_round(int bonus) {
	player->set(Used);
}

static void for_each_player(int bonus) {
	pushvalue push_players(players);
	pushvalue push_player(player);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_players.value) {
		player = (playeri*)p;
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
	{"ClearInput", clear_input},
	{"ChooseStrategy", choose_strategy},
	{"ChooseInput", choose_input},
	{"ChooseQuerry", choose_querry},
	{"EndRound", end_round},
	{"EsteblishControl", establish_control},
	{"InputQuerry", input_querry},
	{"FilterActivated", filter_activated_querry},
	{"ForEachPlayer", for_each_player},
	{"ForEachProvince", for_each_province},
	{"PayForLeaders", pay_for_leaders},
	{"PayHero", pay_hero},
	{"PayResearch", pay_research},
	{"PickStrategy", pick_strategy},
	{"SelectPlayersBySpeaker", select_players},
	{"SelectProvinces", select_provincies},
	{"SelectProvincesYouControl", select_your_provincies},
	{"SelectStrategy", select_strategy},
};
BSDATAF(script)