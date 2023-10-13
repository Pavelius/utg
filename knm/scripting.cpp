#include "answers.h"
#include "army.h"
#include "area.h"
#include "card.h"
#include "crt.h"
#include "deck.h"
#include "draw.h"
#include "draw_object.h"
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

static char			log_text[1024];
static stringbuilder actions_log(log_text);
static char			console_text[512];
stringbuilder		console(console_text);
static entitya		recruit, movement;
static bool			need_break;
static int			last_value;

void update_ui();

static int getone(int v) {
	return v ? v : 1;
}

int count_cards(const playeri* player, const entity* location) {
	auto result = 0;
	for(auto& e : bsdata<card>()) {
		if(!e)
			continue;
		if(e.player != player)
			continue;
		auto p = e.getcomponent();
		if(!p || !p->usedeck() || p->location != location)
			continue;
		result++;
	}
	return result;
}

static int count_structures(const provincei* province) {
	auto result = 0;
	for(auto& e : bsdata<structure>()) {
		if(e.location == province)
			result++;
	}
	return result;
}

static int count_structures(const provincei* province, const structurei* type) {
	auto result = 0;
	for(auto& e : bsdata<structure>()) {
		if((structurei*)e.id == type && e.location == province)
			result++;
	}
	return result;
}

static int count_units(const provincei* province, const playeri* player) {
	auto result = 0;
	for(auto& e : bsdata<troopi>()) {
		if(e.location == province && e.player == player)
			result++;
	}
	return result;
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

static void wave_hostile() {
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player || !e.player)
			continue;
		if(!area.isvalid(e.position))
			continue;
		area.setblock(e.position, 0xFF);
	}
}

static void wave_clear() {
	area.clearpath();
}

static void wave_proceed() {
	area.makewave(province->position);
}

static void make_wave() {
	wave_clear();
	wave_hostile();
	wave_proceed();
}

void reapeated_list(int value, int counter) {
	counter = getone(counter);
	for(auto i = 0; i < counter; i++)
		fnscript<listi>(value, 0);
}

template<> void fnscript<abilityi>(int value, int counter) {
	last_ability = (ability_s)value;
	player->current.abilities[value] += counter;
	if(counter > 0)
		logging(player, get_log("RaiseAbility"), bsdata<abilityi>::elements[value].getname(), counter);
	else if(counter < 0)
		logging(player, get_log("DecreaseAbility"), bsdata<abilityi>::elements[value].getname(), counter);
}

static void add_troop(uniti* type) {
	auto p = bsdata<troopi>::add();
	p->id = (const char*)type;
	p->location = province;
	p->player = player;
}

static void add_structure(structurei* type) {
	auto p = bsdata<structure>::add();
	p->id = (const char*)type;
	p->location = province;
	p->player = player;
}

template<> void fnscript<uniti>(int value, int counter) {
	for(auto i = 0; i < counter; i++)
		add_troop(bsdata<uniti>::elements + value);
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

template<> void fnscript<decki>(int value, int counter) {
	last_deck = bsdata<decki>::elements + value;
	querry.clear();
	for(auto i = 0; i < counter; i++)
		querry.add(last_deck->cards.pick());
}

static void focus_player(int bonus) {
	if(player->homeland)
		player->homeland->focusing();
}

static void focus_province(int bonus) {
	province->focusing();
}

static void add_input(entity& e) {
	an.add(&e, e.getname());
}

static void add_nameable(nameable& e) {
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

static void pick_cards(int bonus) {
	for(auto p : querry)
		p->pick(player);
	querry.clear();
}

static void apply_input(void* result, bool play_card = true) {
	if(!result) {
		// Nothing to do
	} else if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else if(bsdata<playeri>::have(result))
		player = (playeri*)result;
	else if(bsdata<strategyi>::have(result))
		last_strategy = (strategyi*)result;
	else if(bsdata<structurei>::have(result))
		last_structure = (structurei*)result;
	else if(bsdata<troopi>::have(result))
		last_troop = (troopi*)result;
	else if(bsdata<card>::have(result)) {
		if(play_card)
			((card*)result)->play();
		else
			last_card = (card*)result;
	} else if(bsdata<cardi>::have(result)) {
		if(play_card)
			((cardi*)result)->play();
		else
			last_component = (cardi*)result;
	} else if(bsdata<listi>::have(result))
		((listi*)result)->run();
	else if(bsdata<abilityi>::have(result)) {
		auto v = (ability_s)((abilityi*)result - bsdata<abilityi>::elements);
		player->current.abilities[v] += 1;
	} else
		((fnevent)result)();
	update_ui();
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

static void destroy_enemy_structures(int bonus) {
	for(auto& e : bsdata<structure>()) {
		if(e.location == province && e.player != player)
			e.clear();
	}
	update_ui();
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

static void choose_card(int bonus) {
	pushtitle push(last_list->id);
	an.clear();
	input_querry(bonus);
	auto result = an.choose(get_title(last_id));
	apply_input(result, false);
	if(!last_card && !last_component)
		script_stop();
}

static bool no_player(const void* object) {
	auto p = (entity*)object;
	return p->player == 0;
}

static bool filter_valid(const void* object) {
	auto p = (entity*)object;
	return p->id != 0;
}

static bool filter_activated(const void* object) {
	auto p = (entity*)object;
	return p->is(player);
}

static bool filter_province_activated(const void* object) {
	auto p = (entity*)object;
	return p->getprovince()->is(player);
}

static bool filter_used(const void* object) {
	auto p = (entity*)object;
	return p->is(Used);
}

bool filter_player_upgrade(const void* object) {
	auto p = (upgradei*)object;
	return player->isupgrade(p);
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
	return p->id && p->getprovince() == province;
}

static bool filter_structure_limit(const void* object) {
	if(bsdata<structurei>::have(object)) {
		auto p = (structurei*)object;
		auto limit_in_province = p->abilities[LimitInProvince];
		if(limit_in_province) {
			auto count = count_structures(province, p);
			if(count >= limit_in_province)
				return false;
		}
	}
	return true;
}

static bool filter_province_limit(const void* object) {
	auto p = (provincei*)object;
	auto limit = p->abilities[Limit];
	if(limit) {
		auto count = count_structures(p);
		if(limit >= count)
			return false;
	}
	return true;
}

static bool filter_move_range(const void* object) {
	auto p = (entity*)object;
	if(!p->id)
		return false;
	auto move = p->get(Move);
	if(!move)
		return false;
	auto range = area.getblock(p->getprovince()->position);
	if(range == 0xFF)
		return false;
	return move >= range;
}

static bool filter_neightboard(const void* object) {
	auto p = (entity*)object;
	return area.getblock(p->getprovince()->position) <= 1;
}

static void* get_province(const void* object) {
	return ((entity*)object)->getprovince();
}

static void group_province(int bonus) {
	querry.group(get_province);
}

static void set_value(int bonus) {
	last_value = bonus;
}

static void add_value(int bonus) {
	last_value += bonus;
}

static void set_value_querry_count(int bonus) {
	last_value = querry.getcount() + bonus;
}

static void set_province_capital(int bonus) {
	province = bsdata<provincei>::elements;
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
	return allow_pay(Hero, getone(bonus));
}
static void pay_hero(int bonus) {
	pay(Hero, getone(bonus));
}

static void pay_hero_yesno(int bonus) {
	if(draw::yesno(getdescription("StrategySecondaryAsk"), last_strategy->getname()))
		pay_hero(bonus);
	else
		script_stop();
}

static void pay_ability(const char* id, ability_s v, ability_s currency, int gain, int cost, int maximum_cap) {
	pushtitle push_title(id);
	an.clear();
	auto choose_prompt = getdescription(stw(id, "Answer"));
	if(!choose_prompt)
		choose_prompt = getdescription("PayResource");
	auto choose_cancel = getdescription(stw(id, "Cancel"));
	if(!choose_cancel)
		choose_cancel = getdescription("PayResourceCancel");
	for(auto i = 1; i <= maximum_cap; i++) {
		auto cost_total = cost * i;
		auto value = player->get(currency);
		if(value < cost_total)
			break;
		auto gain_total = i * gain;
		an.add((void*)i, choose_prompt,
			bsdata<abilityi>::elements[v].getname(),
			bsdata<abilityi>::elements[currency].getname(),
			gain_total, cost_total);
	}
	auto i = (int)an.choose(get_title(id), choose_cancel, 0);
	player->current.abilities[v] += gain * i;
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
	if(player != province->player)
		return;
	auto recruit_value = province->get(Recruit) + province->getbonus(Recruit);
	if(!recruit_value)
		return;
	recruit_value += province->get(Resources) + province->getbonus(Resources) + bonus;
	auto army_used = get_troops(Army, province, player);
	auto army_cap = player->get(Army);
	auto payment_value = player->get(Resources) + player->get(Gold);
	recruit_troops(army_used, army_cap, recruit_value, payment_value);
}

static void add_leaders(int bonus) {
	if(!bonus)
		bonus = 1;
	pushtitle push("AddLeaders");
	while(bonus > 0) {
		an.clear();
		add_nameable(bsdata<abilityi>::elements[Hero]);
		add_nameable(bsdata<abilityi>::elements[Army]);
		apply_input(bonus);
		bonus--;
	}
}

static void add_start(int bonus) {
	auto push_province = province;
	province = player->homeland;
	script_run(player->start);
	province = push_province;
	update_ui();
}

static void add_research(int bonus) {
	player->current.abilities[Lore] += bonus;
}

static void add_secret_goal(int bonus) {
}

static void add_gold(int bonus) {
	player->current.abilities[Gold] += bonus;
}

static void combat_round(int bonus) {
	attacker.prepare(Shield);
	defender.prepare(Shield);
	attacker.engage(Damage, 0);
	defender.engage(Damage, 0);
	defender.suffer(attacker.get(Damage));
	attacker.suffer(defender.get(Damage));
}

static card* pickcard(deck_s type) {
	return (card*)bsdata<decki>::elements[type].cards.pick();
}

static void pick_speaker(int bonus) {
	speaker = player;
}

static void pick_strategy(int bonus) {
	last_strategy->player = player;
	player->initiative = last_strategy->initiative;
}

static void pay_for_leaders(int bonus) {
	pay_ability(last_script->id, Hero, Influence, 1, bonus, 3);
}

static bool allow_pay_goods(int bonus) {
	return player->current.abilities[Goods] >= bonus;
}
static void pay_goods(int bonus) {
	player->current.abilities[Goods] -= bonus;
}

static void pay_research(int bonus) {
	pay_ability(last_script->id, Lore, Resources, bonus, 1, 5);
}

static bool allow_pay_resources(int bonus) {
	return allow_pay(Resources, getone(bonus));
}
static void pay_resources(int bonus) {
	pay(Resources, getone(bonus));
}

static bool allow_pay_gold(int bonus) {
	return allow_pay(Gold, getone(bonus));
}
static void pay_gold(int bonus) {
	pay(Gold, getone(bonus));
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

static void cancel_movement() {
	movement.clear();
}

static void choose_movement() {
	movement.clear();
	while(true) {
		make_wave();
		querry.collectiona::select(bsdata<troopi>::source, filter_move_range, true);
		querry.match(filter_province_activated, false);
		clear_input(0);
		for(auto p : querry) {
			if(movement.find(p) != -1)
				continue;
			an.add(p, "%1 (%2)", p->getname(), p->getprovince()->getname());
		}
		for(auto p : movement) {
			if(!console)
				console.add("###%1", province->getname());
			console.addn("%1 (%2)", p->getname(), p->getprovince()->getname());
		}
		if(console)
			an.add(cancel_movement, getnm("ResetMovement"));
		auto result = an.choose(get_title(last_id), getnm("ApplyMove"), 0);
		if(!result)
			break;
		else if(bsdata<troopi>::have(result))
			movement.add(result);
		else
			((fnevent)result)();
	}
	console.clear();
}

static void apply_movement(const entitya& source) {
	for(auto p : source)
		p->location = province;
	update_ui();
}

static void choose_movement(int bonus) {
	pushtitle push(last_list->id);
	choose_movement();
	apply_movement(movement);
}

static void make_action(int bonus) {
	pushtitle push("MakeAction");
	pushvalue push_strategy(last_strategy, (strategyi*)0);
	focus_player(0);
	an.clear();
	add_input_cards("MakeAction");
	add_strategy_cards();
	apply_input();
	if(last_strategy) {
		apply_primary_strategy();
		apply_secondary_strategy();
	}
}

static void make_wave(int bonus) {
	make_wave();
}

static int choose_case(const char* v1, const char* v2, const char* v3 = 0) {
	an.clear();
	if(v1)
		an.add((void*)1, getnm(v1));
	if(v2)
		an.add((void*)2, getnm(v2));
	if(v3)
		an.add((void*)3, getnm(v3));
	return (long)an.choose(0, 0, 0);
}

static void put_card_on_table(int bonus) {
	last_card->location = 0;
}

static void choose_cards_discard(int bonus) {
	while(bonus > 0) {
		choose_card(0);
		auto index = choose_case("ToTheTop", "ToTheBottom");
		querry.remove(last_card);
		last_card->discard(index == 1);
		bonus--;
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

static void select_player_troops(int bonus) {
	querry.collectiona::select(bsdata<troopi>::source, filter_player, true);
}

static void select_strategy(int bonus) {
	querry.collectiona::select(bsdata<strategyi>::source, no_player, bonus >= 0);
}

static void select_structures(int bonus) {
	querry.collectiona::select(bsdata<structurei>::source);
}

static void select_provincies(int bonus) {
	querry.collectiona::select(bsdata<provincei>::source);
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

static void build_structure(int bonus) {
	add_structure(last_structure);
	update_ui();
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

static void attack_milita(int bonus) {
	auto count = count_structures(province);
	attacker.damage(1 + bonus, count);
}

static bool filter_player_province(const void* object) {
	auto p = (entity*)object;
	return p->player == player && p->location == province;
}

static void add_units(entitya& source, playeri* v) {
	source.select(province, v);
	source.sortunits();
}

static void determine_winner() {
	auto a = attacker.getstrenght();
	auto d = defender.getstrenght();
	if(a >= d)
		winner_army = &attacker;
	else
		winner_army = &defender;
}

static void prepare_army(int bonus) {
	winner_army = 0;
	attacker.clear();
	defender.clear();
	if(player == province->player) {
		script_stop();
		return;
	}
	attacker.select(province, player);
	defender.select(province, province->player);
	if(!attacker.troops || !defender.troops)
		script_stop();
	attacker.prepare(Shield);
	defender.prepare(Shield);
}

static void show_battle_result() {
	if(!console)
		return;
	pushtitle header(last_list->id);
	draw::pause();
	attacker.applycasualty();
	defender.applycasualty();
	determine_winner();
	update_ui();
}

static void attack_army(armyi& source) {
	if(source.troops) {
		console.addn("---");
		source.engage(Damage, 0);
	}
}

static void melee_clash(int bonus) {
	console.clear();
	attack_army(attacker);
	attack_army(defender);
	attacker.suffer(defender.abilities[Damage]);
	defender.suffer(attacker.abilities[Damage]);
	show_battle_result();
}

static bool is_allow_actions() {
	for(auto& e : bsdata<playeri>()) {
		if(!e.is(Used))
			return true;
	}
	return false;
}

static void if_control_capital(int bonus) {
	auto result_true = (bsdata<provincei>::elements[0].player == player);
	if(result_true != (bonus >= 0))
		script_stop();
}

static void if_no_querry_break(int bonus) {
	auto result_true = (querry.getcount() == 0);
	if(result_true == (bonus >= 0)) {
		need_break = true;
		script_stop();
	}
}

static void if_win_battle(int bonus) {
	last_army = winner_army;
	auto result_true = (!last_army || last_army == &attacker);
	if(result_true != (bonus >= 0))
		script_stop();
}

static void for_each_card(int bonus) {
	entityv push_querry(querry);
	pushvalue push(player);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_querry) {
		last_card = static_cast<card*>(p);
		script_run(commands);
	}
	script_stop();
}

static void for_each_player(int bonus) {
	entityv push_querry(querry);
	pushvalue push(player);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_querry) {
		player = static_cast<playeri*>(p);
		script_run(commands);
	}
	script_stop();
}

static void for_each_province(int bonus) {
	entityv push_querry(querry);
	pushvalue push(province);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_querry) {
		province = static_cast<provincei*>(p);
		script_run(commands);
	}
	script_stop();
}

static void for_each_strategy(int bonus) {
	entityv push_querry(querry);
	pushvalue push(last_strategy);
	variants commands; commands.set(script_begin, script_end - script_begin);
	for(auto p : push_querry) {
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

static void push_player(int bonus) {
	pushvalue push(player);
	variants commands; commands.set(script_begin, script_end - script_begin);
	script_run(commands);
	script_stop();
}

static void push_province(int bonus) {
	pushvalue push(province);
	variants commands; commands.set(script_begin, script_end - script_begin);
	script_run(commands);
	script_stop();
}

static void current_province(int bonus) {
	if(bonus >= 0) {
		if(querry.find(province) == -1)
			querry.add(province);
	} else {
		auto pi = querry.find(province);
		if(pi != -1)
			querry.remove(pi, 1);
	}
}

static void move_troops(const playeri* player, const provincei* start, provincei* destination) {
	for(auto& e : bsdata<troopi>()) {
		if(e.player == player && e.getprovince() == start)
			e.location = destination;
	}
	update_ui();
}

static void destory_troops(const playeri* player, const provincei* start) {
	for(auto& e : bsdata<troopi>()) {
		if(e.player == player && e.getprovince() == start)
			e.clear();
	}
	update_ui();
}

static armyi* get_looser_army() {
	if(winner_army == &attacker)
		return &defender;
	else if(winner_army == &defender)
		return &attacker;
	return 0;
}

static void retreat_troops(int bonus) {
	auto looser = get_looser_army();
	if(!looser || !looser->player || !looser->troops)
		return;
	pushvalue push(province);
	pushvalue push_player(player, looser->player);
	auto start_province = province;
	make_wave();
	select_provincies(0);
	querry.match(filter_neightboard, true);
	current_province(-1);
	if(querry) {
		choose_province(0);
		if(province) {
			move_troops(player, push.value, province);
			activity_token(0);
		}
	} else
		destory_troops(player, province);
}

static void apply_trigger(int bonus) {
	if(!last_list)
		return;
	auto id = last_list->id;
	// Standart component trigger (autouse)
	for(auto& e : bsdata<cardi>()) {
		if(e.usedeck() || !e.trigger)
			continue;
		if(e.player != player)
			continue;
		if(strcmp(e.trigger, id) != 0)
			continue;
		script_run(e.effect);
	}
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

static bool allow_and_stop(int bonus) {
	script_stop();
	return true;
}

static void open_tactic(int bonus) {
}

void initialize_script() {
	answers::console = &console;
	answers::prompt = console.begin();
}

BSDATA(filteri) = {
	{"FilterActivated", filter_activated},
	{"FilterHomeland", filter_homeland},
	{"FilterPlayer", filter_player},
	{"FilterProvinceLimit", filter_province_limit},
	{"FilterNeightboard", filter_neightboard},
	{"FilterSpeaker", filter_speaker},
	{"FilterStructureLimit", filter_structure_limit},
};
BSDATAF(filteri);
BSDATA(script) = {
	{"ActivityToken", activity_token},
	{"AddGold", add_gold},
	{"AddLeaders", add_leaders},
	{"AddProvinceInfluence", add_province_influence},
	{"AddProvinceResources", add_province_resource},
	{"AddResearch", add_research},
	{"AddSecretGoal", add_secret_goal},
	{"AddPlayerTag", add_player_tag},
	{"AddStart", add_start},
	{"AddValue", add_value},
	{"AttackMilita", attack_milita},
	{"ApplyTrigger", apply_trigger},
	{"BuildStructure", build_structure},
	{"PutCardOnTable", put_card_on_table},
	{"ChooseCardsDiscard", choose_cards_discard},
	{"ChooseMovement", choose_movement},
	{"ChooseProvince", choose_province, allow_choose},
	{"ChooseQuerry", choose_querry, allow_choose},
	{"CurrentProvince", current_province},
	{"DestroyEnemyStructures", destroy_enemy_structures},
	{"EndRound", end_round, allow_end_round},
	{"EsteblishControl", establish_control},
	{"FocusPlayer", focus_player},
	{"FocusProvince", focus_province},
	{"ForEachCard", for_each_card, allow_for_each},
	{"ForEachPlayer", for_each_player, allow_for_each},
	{"ForEachProvince", for_each_province, allow_for_each},
	{"ForEachStrategy", for_each_strategy, allow_for_each},
	{"GroupProvince", group_province, allow_select},
	{"IfControlCapital", if_control_capital},
	{"IfNoQuerryBreak", if_no_querry_break},
	{"IfWinBattle", if_win_battle},
	{"InputQuerry", input_querry},
	{"MakeAction", make_action},
	{"MakeWave", make_wave},
	{"MeleeClash", melee_clash},
	{"NoConditions", script_none, allow_and_stop},
	{"OpenTactic", open_tactic},
	{"PayForLeaders", pay_for_leaders, allow_pay_for_leaders},
	{"PayGold", pay_gold, allow_pay_gold},
	{"PayGoods", pay_goods, allow_pay_goods},
	{"PayHero", pay_hero, allow_pay_hero},
	{"PayHeroYesNo", pay_hero_yesno, allow_pay_hero},
	{"PayResearch", pay_research},
	{"PayResources", pay_resources, allow_pay_resources},
	{"PickCards", pick_cards},
	{"PickSpeaker", pick_speaker},
	{"PickStrategy", pick_strategy},
	{"PlayerUsed", player_used},
	{"PrepareArmy", prepare_army},
	{"PushPlayer", push_player},
	{"PushProvince", push_province},
	{"RecruitTroops", recruit_troops},
	{"RefreshInfluence", refresh_influence},
	{"RefreshResources", refresh_resources},
	{"RefreshTrade", refresh_trade},
	{"RemoveStrategy", remove_strategy},
	{"Repeat", repeat_statement},
	{"RetreatTroops", retreat_troops},
	{"SelectPlayers", select_players, allow_select},
	{"SelectPlayersBySpeaker", select_players_speaker, allow_select},
	{"SelectPlayerTroops", select_player_troops, allow_select},
	{"SelectProvinceStructures", select_province_structures, allow_select},
	{"SelectProvinces", select_provincies, allow_select},
	{"SelectStrategy", select_strategy, allow_select},
	{"SelectStructures", select_structures, allow_select},
	{"SetProvinceCapital", set_province_capital, allow_script},
	{"SetValue", set_value, allow_script},
	{"SetValueQuerryCount", set_value_querry_count, allow_script},
};
BSDATAF(script)