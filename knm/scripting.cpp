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
#include "troop.h"
#include "unit.h"

static char				log_text[1024];
static stringbuilder	log(log_text);
static char				console_text[512];
static stringbuilder	console(console_text);
static entitya			recruit;

template<> void fnscript<abilityi>(int value, int counter) {
	player->current.abilities[value] += counter;
}

template<> void fnscript<uniti>(int value, int counter) {
}

static void add_input(nameable& e) {
	an.add(&e, e.getname());
}

static void add_input_cards(const char* trigger) {
	for(auto& e : bsdata<cardi>()) {
		if(e.player && e.player != player)
			continue;
		if(e.usedeck())
			continue;
		if(strcmp(e.trigger, trigger) != 0)
			continue;
		if(!script_allow(e.effect))
			continue;
		add_input(e);
	}
}

static void add_strategy_cards() {
	for(auto& e : bsdata<strategyi>()) {
		if(e.player != player)
			continue;
		if(e.is(Used))
			continue;
		add_input(e);
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
		auto push = last_card; last_card = (cardi*)result;
		script_run(last_card->effect);
		last_card = push;
	} else if(bsdata<listi>::have(result)) {
		auto push_list = last_list; last_list = (listi*)result;
		script_run(((listi*)result)->elements);
		last_list = push_list;
	} else if(bsdata<abilityi>::have(result)) {
		auto v = (ability_s)((abilityi*)result - bsdata<abilityi>::elements);
		player->current.add(v, 1);
	}
}

static void clear_input(int bonus) {
	an.clear();
	if(an.console)
		an.console->clear();
}

static void input_querry(int bonus) {
	for(auto p : querry)
		add_input(*p);
}

static void choose_input(int bonus) {
	pushtitle push(last_list->id);
	apply_input();
}

static void choose_querry(int bonus) {
	an.clear();
	input_querry(bonus);
	choose_input(bonus);
}

static bool no_player(const void* object) {
	auto p = (entity*)object;
	return p->player == 0;
}

static bool filter_activated(const void* object) {
	auto p = (entity*)object;
	return p->is(player);
}

static int compare_player_priority(const void* v1, const void* v2) {
	auto p1 = *((playeri**)v1);
	auto p2 = *((playeri**)v2);
	return p1->initiative - p2->initiative;
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

static int getone(int v) {
	return v ? v : 1;
}

static void pay(ability_s v, int bonus) {
	if(player->current.abilities[v] >= bonus)
		player->current.abilities[v] -= bonus;
}

static bool allow_pay(ability_s v, int bonus) {
	return player->current.abilities[v] >= bonus;
}

static bool pay_hero_allow(int bonus) {
	return allow_pay(Tactic, getone(bonus));
}
static void pay_hero(int bonus) {
	pay(Tactic, getone(bonus));
}

static void pay_ability(const char* id, ability_s v, ability_s currency, int cost, int maximum_cap) {
	pushtitle push_title(id);
	an.clear();
	for(auto i = 1; i <= maximum_cap; i++) {
		auto total = cost * i;
		auto value = player->get(v);
		if(value < total)
			break;
		an.add((void*)i, getdescription("PayResource"),
			bsdata<abilityi>::elements[v].getname(),
			bsdata<abilityi>::elements[currency].getname(),
			i, total);
	}
	auto n = (int)an.choose(0, getdescription("PayCancel"), 1);
	player->current.add(v, n);
	player->current.add(currency, -cost * n);
}

static void recruit_reset() {
	recruit.clear();
}

static void recruit_troops(int army_used, int army_maximum, int build_troops_maximum, int maximum_cost) {
	recruit.clear();
	while(true) {
		clear_input(0);
		auto troops_cost = recruit.gettotal(Cost);
		auto troops_army = (army_used + recruit.gettotal(Army) + 9) / 10;
		console.addn(getnm("AllowBuildTroops"), recruit.getcount(), build_troops_maximum);
		console.addn(getnm("AllowBuildArmy"), troops_army, army_maximum);
		console.addn(getnm("AllowCredit"), troops_cost, maximum_cost);
		if(recruit) {
			console.add("---");
			for(auto p : recruit)
				console.add(getdescription("RecruitTroopPrompt"), p->getname(), p->get(Cost));
		}
		auto total = maximum_cost - troops_cost;
		for(auto p : player->troops) {
			if(total >= p->get(Cost))
				an.add(p, getdescription("RecruitUnitCost"), p->get(Cost));
		}
		if(recruit)
			an.add(recruit_reset, getnm("RecruitReset"));
		auto p = (uniti*)an.choose(getdescription("RecruitWhatTroop"), getnm("CheckOut"), 1);
		if(!p)
			break; // Check out;
		else if(bsdata<uniti>::have(p))
			recruit.add(p);
		else
			((fnevent)p)();
	}
}

static int get_troops(ability_s v, provincei* province, playeri* player) {
	auto result = 0;
	for(auto& e : bsdata<troopi>()) {
		if(e.getprovince() == province && e.player == player)
			result += e.get(v);
	}
	return result;
}

static void recruit_troops(int bonus) {
	auto army_used = get_troops(Army, province, player);
	recruit_troops(army_used, player->get(Army), province->get(Resources) + bonus, player->get(Resources) + player->get(Gold));
}

static void add_leaders(int bonus) {
	if(!bonus)
		bonus = 1;
	pushtitle push("AddLeaders");
	while(bonus > 0) {
		an.clear();
		add_input(bsdata<abilityi>::elements[Tactic]);
		add_input(bsdata<abilityi>::elements[Army]);
		apply_input();
		bonus--;
	}
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
	player->initiative = last_strategy->initiative;
}

static void pay_for_leaders(int bonus) {
	pay_ability("PayLeaders", Tactic, Influence, 2, 3);
}

static void pay_research(int bonus) {
}

static void apply_primary_strategy(int bonus) {
	last_strategy->set(Used);
	script_run(last_strategy->primary);
}

static void apply_secondary_strategy(int bonus) {
	script_run(last_strategy->secondary);
}

static void make_action(int bonus) {
	pushtitle push("MakeAction");
	pushvalue push_strategy(last_strategy, (strategyi*)0);
	an.clear();
	add_input_cards("MakeAction");
	add_strategy_cards();
	apply_input();
	if(last_strategy)
		apply_primary_strategy(0);
}

static void select_players_speaker(int bonus) {
	players.clear();
	for(auto& e : bsdata<playeri>()) {
		if(e.is(Used))
			continue;
		players.add(&e);
	}
}

static void sort_players() {
	players.sort(compare_player_priority);
}

static void select_players(int bonus) {
	select_players_speaker(bonus);
	sort_players();
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
	entityv push_players(players);
	pushvalue push_player(player);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_players) {
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

void initialize_script() {
	answers::console = &console;
	answers::prompt = console.begin();
}

BSDATA(script) = {
	{"AddActions", add_actions},
	{"AddGoods", add_goods},
	{"AddLeaders", add_leaders},
	{"AddResearch", add_research},
	{"AddSecretGoal", add_secret_goal},
	{"ApplyPrimaryStrategy", apply_primary_strategy},
	{"ApplySecondaryStrategy", apply_secondary_strategy},
	{"ClearInput", clear_input},
	{"ChooseInput", choose_input},
	{"ChooseQuerry", choose_querry},
	{"EndRound", end_round},
	{"EsteblishControl", establish_control},
	{"FilterActivated", filter_activated_querry},
	{"ForEachPlayer", for_each_player},
	{"ForEachProvince", for_each_province},
	{"InputQuerry", input_querry},
	{"MakeAction", make_action},
	{"PayForLeaders", pay_for_leaders},
	{"PayHero", pay_hero, pay_hero_allow},
	{"PayResearch", pay_research},
	{"PickStrategy", pick_strategy},
	{"RecruitTroops", recruit_troops},
	{"SelectPlayers", select_players},
	{"SelectPlayersBySpeaker", select_players_speaker},
	{"SelectProvinces", select_provincies},
	{"SelectProvincesYouControl", select_your_provincies},
	{"SelectStrategy", select_strategy},
};
BSDATAF(script)