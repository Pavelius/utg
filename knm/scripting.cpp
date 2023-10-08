#include "answers.h"
#include "army.h"
#include "card.h"
#include "crt.h"
#include "deck.h"
#include "draw.h"
#include "entitya.h"
#include "filter.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "pushtitle.h"
#include "pushvalue.h"
#include "script.h"
#include "strategy.h"
#include "structure.h"
#include "tag.h"
#include "troop.h"
#include "unit.h"

extern const char* message_string;

static char				log_text[1024];
static stringbuilder	actions_log(log_text);
static char				console_text[512];
static stringbuilder	console(console_text);
static entitya			recruit;
static int				need_pay;
static bool				need_break;

static int getone(int v) {
	return v ? v : 1;
}

static void loggingv(const playeri* player, const char* format, const char* format_param, char separator = '\n') {
	if(!format || format[0] == 0)
		return;
	if(separator)
		actions_log.addsep(separator);
	if(player)
		actions_log.add("[%1]: ", player->getname());
	actions_log.addv(format, format_param);
}

static void logging(const char* format, ...) {
	loggingv(0, format, xva_start(format));
}

static void logging(const playeri* player, const char* format, ...) {
	loggingv(player, format, xva_start(format));
}

static const char* get_log(const char* id) {
	return getdescription(stw(id, "Log"));
}

static const char* get_script_log() {
	return get_log(last_script->id);
}

template<> void fnscript<abilityi>(int value, int counter) {
	player->current.abilities[value] += counter;
	if(counter > 0)
		logging(player, get_log("RaiseAbility"), bsdata<abilityi>::elements[value].getname(), counter);
	else
		logging(player, get_log("DecreaseAbility"), bsdata<abilityi>::elements[value].getname(), counter);
}

template<> void fnscript<uniti>(int value, int counter) {
}

static void add_structure(structurei* ps) {
	auto p = bsdata<structure>::add();
	p->id = (const char*)ps;
	p->location = province;
	p->player = player;
}

template<> void fnscript<structurei>(int value, int counter) {
	for(auto i = 0; i < counter; i++)
		add_structure(bsdata<structurei>::elements + value);
}

static bool have_tag(const void* object, int value) {
	auto p = (entity*)object;
	return p->is((tag_s)value);
}

template<> void fnscript<tagi>(int value, int counter) {
	querry.match(have_tag, value, counter >= 0);
}
template<> bool fntest<tagi>(int value, int counter) {
	fnscript<tagi>(value, counter);
	return querry.getcount() != 0;
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

static void apply_input(void* result) {
	if(!result) {
		// Nothing to do
	} else if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else if(bsdata<playeri>::have(result))
		player = (playeri*)result;
	else if(bsdata<strategyi>::have(result))
		last_strategy = (strategyi*)result;
	else if(bsdata<card>::have(result)) {
		auto push = last_component; last_component = (card*)result;
		script_run(last_component->getcomponent()->effect);
		last_component = push;
	} else if(bsdata<listi>::have(result))
		((listi*)result)->run();
	else if(bsdata<abilityi>::have(result)) {
		auto v = (ability_s)((abilityi*)result - bsdata<abilityi>::elements);
		player->current.abilities[v] += 1;
	} else
		((fnevent)result)();
}

static const char* get_title(const char* id, int count = 0) {
	static char temp[260];
	auto title = getdescription(stw(id, "Ask"));
	if(!title)
		return 0;
	stringbuilder sb(temp); sb.clear();
	sb.add(title, count);
	return temp;
}

static void apply_input(int count = 0) {
	auto result = an.choose(get_title(last_id, count));
	apply_input(result);
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

static void choose_province(int bonus) {
	pushtitle push(last_list->id);
	pushvalue push_message(message_string, get_title(last_list->id, bonus));
	province = querry.chooseprovince();
	if(!province)
		script_stop();
}

static bool no_player(const void* object) {
	auto p = (entity*)object;
	return p->player == 0;
}

static bool filter_activated(const void* object) {
	auto p = (entity*)object;
	return p->is(player);
}

static bool filter_used(const void* object) {
	auto p = (entity*)object;
	return p->is(Used);
}

static bool filter_player(const void* object) {
	auto p = (entity*)object;
	return p->getplayer() == player;
}

static bool filter_speaker(const void* object) {
	auto p = (entity*)object;
	return p->getplayer() == speaker;
}

static bool filter_homeland(const void* object) {
	auto p = (entity*)object;
	return p->ishomeland();
}

static bool filter_province(const void* object) {
	auto p = (entity*)object;
	return p->getprovince() == province;
}

static int compare_player_priority(const void* v1, const void* v2) {
	auto p1 = *((playeri**)v1);
	auto p2 = *((playeri**)v2);
	return p1->initiative - p2->initiative;
}

static void refresh_ability(ability_s v) {
	player->set(v, player->getmaximum(v));
}

static void refresh_trade(int bonus) {
	refresh_ability(Goods);
}

static void refresh_resources(int bonus) {
	refresh_ability(Resources);
}

static void refresh_influence(int bonus) {
	refresh_ability(Influence);
}

static void add_consumbale(ability_s v, int bonus) {
	bonus = getone(bonus);
	auto total = province->get(v) + province->getbonus(v);
	player->add(v, total * bonus);
}

static void add_province_influence(int bonus) {
	add_consumbale(Influence, bonus);
}

static void add_province_resource(int bonus) {
	add_consumbale(Resources, bonus);
}

static void activity_token(int bonus) {
	if(!player)
		return;
	if(bonus >= 0)
		province->set(player);
	else
		province->remove(player);
}

static void add_player_tag(int bonus) {
	if(bonus >= 0)
		player->set(last_tag);
	else
		player->remove(last_tag);
}

static void remove_strategy(int bonus) {
	logging(last_strategy->player, get_script_log(), last_strategy->getname());
	last_strategy->player = 0;
}

static void pay(ability_s v, int bonus) {
	if(player->current.abilities[v] >= bonus)
		player->current.abilities[v] -= bonus;
}

static bool allow_pay(ability_s v, int bonus) {
	return player->current.abilities[v] >= bonus;
}

static bool allow_pay_for_leaders(int bonus) {
	return allow_pay(Influence, bonus);
}

static bool allow_pay_hero(int bonus) {
	return allow_pay(Tactic, getone(bonus));
}
static void pay_hero(int bonus) {
	bonus = getone(bonus);
	pay(Tactic, bonus);
}

static void pay_hero_yesno(int bonus) {
	if(draw::yesno(getdescription("StrategySecondaryAsk"), last_strategy->getname()))
		pay_hero(bonus);
	else
		script_stop();
}

static void pay_ability(const char* id, ability_s v, ability_s currency, int cost, int maximum_cap) {
	pushtitle push_title(id);
	an.clear();
	auto choose_prompt = getdescription(stw(id, "Answer"));
	if(!choose_prompt)
		choose_prompt = getdescription("PayResource");
	auto choose_cancel = getdescription(stw(id, "Cancel"));
	if(!choose_cancel)
		choose_cancel = getdescription("PayResourceCancel");
	for(auto i = 1; i <= maximum_cap; i++) {
		auto total = cost * i;
		auto value = player->get(currency);
		if(value < total)
			break;
		an.add((void*)i, choose_prompt,
			bsdata<abilityi>::elements[v].getname(),
			bsdata<abilityi>::elements[currency].getname(),
			i, total);
	}
	auto i = (int)an.choose(get_title(id), choose_cancel, 0);
	player->current.abilities[v] += i;
	player->current.abilities[currency] -= cost * i;
}

static void recruit_reset() {
	recruit.clear();
}

static void recruit_troops(int army_used, int army_maximum, int build_troops_maximum, int maximum_cost) {
	pushtitle push_title(last_script->id);
	recruit_reset();
	while(true) {
		clear_input(0);
		auto troops_count = recruit.getcount();
		auto troops_cost = recruit.gettotal(Cost);
		auto troops_army = army_used + recruit.gettotal(Army);
		auto troops_army_maximum = army_maximum * 10;
		console.addn(getnm("AllowBuildTroops"), troops_count, build_troops_maximum);
		console.addn(getnm("AllowBuildArmy"), (troops_army + 9) / 10, army_maximum);
		console.addn(getnm("AllowCredit"), troops_cost, maximum_cost);
		if(recruit) {
			console.addn("---");
			for(auto p : recruit)
				console.addn(getnm("RecruitUnitCost"), p->getname(), p->get(Cost));
		}
		auto total = maximum_cost - troops_cost;
		if(troops_count < build_troops_maximum) {
			for(auto p : player->troops) {
				if(troops_army + p->get(Army) > troops_army_maximum)
					continue;
				if(total < p->get(Cost))
					continue;
				an.add(p, getnm("RecruitUnitCost"), p->getname(), p->get(Cost));
			}
		}
		if(recruit)
			an.add(recruit_reset, getnm("RecruitReset"));
		auto p = (uniti*)an.choose(getdescription("RecruitWhatTroop"), getnm("CheckOut"), 1);
		if(!p)
			break; // Check out;
		else if(bsdata<uniti>::have(p))
			recruit.add(p);
		else
			apply_input(p);
	}
	console.clear();
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
	auto army_cap = player->get(Army);
	auto recruit_value = province->get(Resources) + province->getbonus(Recruit) + bonus;
	auto payment_value = player->get(Resources) + player->get(Gold);
	recruit_troops(army_used, army_cap, recruit_value, payment_value);
}

static void add_leaders(int bonus) {
	if(!bonus)
		bonus = 1;
	pushtitle push("AddLeaders");
	while(bonus > 0) {
		an.clear();
		add_input(bsdata<abilityi>::elements[Tactic]);
		add_input(bsdata<abilityi>::elements[Army]);
		apply_input(bonus);
		bonus--;
	}
}

static void add_start(int bonus) {
	auto push_province = province;
	province = player->homeland;
	script_run(player->start);
	province = push_province;
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

static card* pickcard(deck_s type) {
	return (card*)bsdata<decki>::elements[type].cards.pick();
}

static void putcard(deck_s type, entity* p, bool to_the_top = false) {
	bsdata<decki>::elements[type].cards.add(p);
}

static void pick_speaker(int bonus) {
	speaker = player;
}

static void pick_strategy(int bonus) {
	last_strategy->player = player;
	player->initiative = last_strategy->initiative;
}

static void pay_for_leaders(int bonus) {
	pay_ability(last_script->id, Tactic, Influence, 2, 3);
}

static bool allow_pay_goods(int bonus) {
	return player->current.abilities[Goods] >= bonus;
}
static void pay_goods(int bonus) {
	player->current.abilities[Goods] -= bonus;
}

static void pay_research(int bonus) {
}

static void apply_primary_strategy() {
	last_strategy->set(Used);
	script_run(last_strategy->primary);
}

static void execute_action(const char* id, const variants& source) {
	pushtitle push(id);
	script_run(source);
}

static void execute_action(const playeri* p, const char* id, const variants& source) {
	pushvalue push(player, const_cast<playeri*>(p));
	execute_action(id, source);
}

static void apply_secondary_strategy() {
	for(auto& e : bsdata<playeri>()) {
		if(&e == player)
			continue;
		execute_action(&e, last_strategy->id, last_strategy->secondary);
	}
}

static void make_action(int bonus) {
	pushtitle push("MakeAction");
	pushvalue push_strategy(last_strategy, (strategyi*)0);
	an.clear();
	add_input_cards("MakeAction");
	add_strategy_cards();
	apply_input();
	if(last_strategy) {
		apply_primary_strategy();
		apply_secondary_strategy();
	}
}

static void look_laws(int bonus) {
	pushtitle push(last_script->id);
	adat<entity*, 16> source;
	for(auto i = 0; i < bonus; i++)
		source.add(pickcard(TacticsDeck));
	while(source.getcount() > 0) {
		an.clear();
		for(auto p : source)
			an.add(p, p->getname());
		auto result = (entity*)an.choose(get_title(last_id), 0, 0);
		source.remove(0);
		an.clear();
		an.add((void*)1, getnm("ToTheTop"));
		an.add((void*)2, getnm("ToTheBottom"));
		auto index = (long)an.choose(0, 0, 0);
		putcard(TacticsDeck, result);
	}
}

static void select_players_speaker(int bonus) {
	querry.clear();
	for(auto& e : bsdata<playeri>()) {
		if(e.is(Used))
			continue;
		querry.add(&e);
	}
}

static void sort_players() {
	querry.sort(compare_player_priority);
}

static void select_players(int bonus) {
	querry.collectiona::select(bsdata<playeri>::source);
	sort_players();
}

static void select_strategy(int bonus) {
	querry.collectiona::select(bsdata<strategyi>::source, no_player, bonus >= 0);
}

static void select_provincies(int bonus) {
	querry.collectiona::select(bsdata<provincei>::source);
}

static void select_your_provincies(int bonus) {
	querry.collectiona::select(bsdata<provincei>::source, filter_player, bonus >= 0);
}

static void select_province_structures(int bonus) {
	querry.collectiona::select(bsdata<structure>::source, filter_province, bonus >= 0);
}

static void* group_id(const void* object) {
	return (void*)((entity*)object)->id;
}

static void group_structure_type(int bonus) {
	querry.group(group_id);
}

static void establish_control(int bonus) {
	if(bonus >= 0)
		province->player = player;
	else
		province->player = 0;
}

static void player_used(int bonus) {
	if(bonus >= 0)
		player->set(Used);
	else
		player->remove(Used);
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

static bool is_allow_actions() {
	for(auto& e : bsdata<playeri>()) {
		if(!e.is(Used))
			return true;
	}
	return false;
}

static void if_no_querry_break(int bonus) {
	auto result_true = (querry.getcount() == 0);
	if(result_true == (bonus >= 0)) {
		need_break = true;
		script_stop();
	}
}

static void for_each_player(int bonus) {
	entityv push_players(querry);
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

static void for_each_strategy(int bonus) {
	pushvalue push(last_strategy);
	pushvalue push_querry(querry);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_querry.value) {
		last_strategy = static_cast<strategyi*>(p);
		script_run(commands);
	}
	script_stop();
}

static void repeat_statement(int bonus) {
	variants commands; commands.set(script_begin, script_end - script_begin);
	pushvalue push_break(need_break, false);
	while(!need_break)
		script_run(commands);
	script_stop();
}

static bool allow_script(int bonus) {
	last_script->proc(bonus);
	return true;
}

static bool allow_select(int bonus) {
	allow_script(bonus);
	return querry.getcount() != 0;
}

static bool allow_for_each(int bonus) {
	script_stop();
	return querry.getcount() != 0;
}

static bool allow_choose(int bonus) {
	script_stop();
	return querry.getcount() != 0;
}

void initialize_script() {
	answers::console = &console;
	answers::prompt = console.begin();
}

BSDATA(filteri) = {
	{"FilterActivated", filter_activated},
	{"FilterHomeland", filter_homeland},
	{"FilterPlayer", filter_player},
	{"FilterSpeaker", filter_speaker},
};
BSDATAF(filteri);
BSDATA(script) = {
	{"ActivityToken", activity_token},
	{"AddActions", add_actions},
	{"AddGoods", add_goods},
	{"AddLeaders", add_leaders},
	{"AddProvinceInfluence", add_province_influence},
	{"AddProvinceResources", add_province_resource},
	{"AddResearch", add_research},
	{"AddSecretGoal", add_secret_goal},
	{"AddPlayerTag", add_player_tag},
	{"AddStart", add_start},
	{"ChooseProvince", choose_province, allow_choose},
	{"ChooseQuerry", choose_querry, allow_choose},
	{"EndRound", end_round, allow_end_round},
	{"EsteblishControl", establish_control},
	{"ForEachPlayer", for_each_player, allow_for_each},
	{"ForEachProvince", for_each_province, allow_for_each},
	{"ForEachStrategy", for_each_strategy, allow_for_each},
	{"IfNoQuerryBreak", if_no_querry_break},
	{"InputQuerry", input_querry},
	{"LookLaws", look_laws},
	{"MakeAction", make_action},
	{"PayForLeaders", pay_for_leaders, allow_pay_for_leaders},
	{"PayGoods", pay_goods, allow_pay_goods},
	{"PayHero", pay_hero, allow_pay_hero},
	{"PayHeroYesNo", pay_hero_yesno, allow_pay_hero},
	{"PayResearch", pay_research},
	{"PickSpeaker", pick_speaker},
	{"PickStrategy", pick_strategy},
	{"PlayerUsed", player_used},
	{"RecruitTroops", recruit_troops},
	{"RefreshInfluence", refresh_influence},
	{"RefreshResources", refresh_resources},
	{"RefreshTrade", refresh_trade},
	{"RemoveStrategy", remove_strategy},
	{"Repeat", repeat_statement},
	{"SelectPlayers", select_players, allow_select},
	{"SelectPlayersBySpeaker", select_players_speaker, allow_select},
	{"SelectProvinceStructures", select_province_structures, allow_select},
	{"SelectProvinces", select_provincies, allow_select},
	{"SelectStrategy", select_strategy, allow_select},
};
BSDATAF(script)