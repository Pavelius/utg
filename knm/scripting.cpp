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

static char				log_text[1024];
static stringbuilder	actions_log(log_text);
static char				console_text[512];
stringbuilder			console(console_text);
static entitya			choosing;
static bool				need_break;
static int				last_value, last_pay;
static card				**card_begin, **card_end;

void update_ui();

namespace {
struct pointline {
	point				position;
	char				count;
};
struct realmi {
	slice<pointline>	tiles;
	point				start[6];
};
}

static pointline players6[] = {
	{{2, 0}, 2},
	{{1, 1}, 5},
	{{0, 2}, 6},
	{{1, 3}, 2},
	{{4, 3}, 2},
	{{0, 4}, 6},
	{{1, 5}, 5},
	{{2, 6}, 2},
};
static realmi galaxy6 = {players6, {{1, 0}, {4, 0}, {0, 3}, {6, 3}, {1, 6}, {4, 6}}};

static void create_realm(const realmi& source) {
	auto tile_index = 0;
	for(auto& e : source.tiles) {
		auto index = e.position;
		for(auto i = 0; i < e.count; i++) {
			((provincei*)querry[tile_index++])->position = index;
			index.x++;
		}
	}
	bsdata<provincei>::elements[0].position = {3, 3};
}

static void assign_starting_positions() {
	auto index = 0;
	auto& positions = galaxy6.start;
	for(auto& e : bsdata<playeri>()) {
		auto ps = e.homeland;
		if(!ps)
			continue;
		ps->position = positions[index++];
	}
}

static int getone(int v) {
	return v ? v : 1;
}

int count_cards(const playeri* player, const entity* location) {
	auto result = 0;
	for(auto& e : bsdata<card>()) {
		if(!e || e.player != player)
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

static void apply_trigger(int bonus) {
	if(!last_id)
		return;
	// Standart component trigger (autouse)
	for(auto& e : bsdata<cardi>()) {
		if(e.usedeck() || !e.trigger)
			continue;
		if(e.player != player)
			continue;
		if(strcmp(e.trigger, last_id) == 0)
			script_run(e.effect);
	}
	// Upgrade trigger (autouse)
	for(auto& e : bsdata<upgradei>()) {
		if(!e.trigger || !player->isupgrade(&e))
			continue;
		if(strcmp(e.trigger, last_id) == 0)
			script_run(e.effect);
	}
}

static bool allow_pay(ability_s v, int bonus) {
	auto total = player->current.abilities[Gold];
	if(v != Gold)
		total += player->current.abilities[v];
	return total >= bonus;
}

static armyi* get_player_army() {
	if(player == attacker.player)
		return &attacker;
	else
		return &defender;
}

static armyi* get_enemy_army() {
	if(player == attacker.player)
		return &defender;
	else
		return &attacker;
}

static armyi* get_looser_army() {
	if(winner_army == &attacker)
		return &defender;
	else if(winner_army == &defender)
		return &attacker;
	return 0;
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
	auto push = last_id;
	last_id = bsdata<listi>::elements[value].id;
	counter = getone(counter);
	for(auto i = 0; i < counter; i++)
		fnscript<listi>(value, 0);
	last_id = push;
}

static void use_ability_or_gold(ability_s v, int value) {
	if(player->current.abilities[v] >= value)
		player->current.abilities[v] -= value;
	else {
		value -= player->current.abilities[v];
		player->current.abilities[v] = 0;
		player->current.abilities[Gold] -= value;
	}
}

static void use_ability(ability_s v, int& value) {
	while(value > 0 && v) {
		if(player->current.abilities[v] >= value) {
			player->current.abilities[v] -= value;
			logging(player, get_log("DecreaseAbility"), bsdata<abilityi>::elements[v].getname(), value);
			value = 0;
		} else {
			value -= player->current.abilities[v];
			logging(player, get_log("DecreaseAbility"), bsdata<abilityi>::elements[v].getname(), player->current.abilities[v]);
			player->current.abilities[v] = 0;
		}
		v = bsdata<abilityi>::elements[v].payoff;
	}
}

template<> void fnscript<abilityi>(int value, int counter) {
	last_ability = (ability_s)value;
	if(counter > 0) {
		player->current.abilities[value] += counter;
		logging(player, get_log("RaiseAbility"), bsdata<abilityi>::elements[value].getname(), counter);
	} else if(counter < 0) {
		counter = -counter;
		use_ability(last_ability, counter);
	}
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

template<> void fnscript<upgradei>(int value, int counter) {
	if(counter >= 0)
		player->setupgrade(bsdata<upgradei>::elements + value);
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

static void add_component_cards(const char* trigger) {
	for(auto& e : bsdata<cardi>()) {
		if((e.player && e.player != player) || e.usedeck())
			continue;
		if(!e.trigger || strcmp(e.trigger, trigger) != 0)
			continue;
		if(!script_allow(e.effect))
			continue;
		add_input(e);
	}
}

static void add_tactic_cards(const char* trigger) {
	for(auto& e : bsdata<card>()) {
		if(e.player != player)
			continue;
		auto p = e.getcomponent();
		if(!p)
			continue;
		if(!p->usedeck())
			continue;
		if(!p->trigger || strcmp(p->trigger, trigger) != 0)
			continue;
		if(!script_allow(p->effect))
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
	else if(bsdata<upgradei>::have(result))
		last_upgrade = (upgradei*)result;
	else if(bsdata<abilityi>::have(result)) {
		auto v = (ability_s)((abilityi*)result - bsdata<abilityi>::elements);
		player->current.abilities[v] += 1;
	} else
		((fnevent)result)();
	update_ui();
}

static const char* get_title(const char* id, int count = 0) {
	static char temp[260];
	stringbuilder sb(temp); sb.clear(); sb.add("%1Ask", id);
	auto title = getdescription(temp);
	if(!title)
		return 0;
	sb.clear(); sb.add(title, count);
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

static bool filter_allowed_upgrade(const void* object) {
	auto p = (upgradei*)object;
	return player->isupgradeallow(p);
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

static bool filter_tactic_cards(const void* object) {
	auto p = (entity*)object;
	if(p->player != player)
		return false;
	auto pc = p->getcomponent();
	if(!pc)
		return false;
	if(pc->location != bsdata<decki>::elements + TacticsDeck)
		return false;
	return true;
}

static bool filter_trigger(const void* object) {
	auto p = (entity*)object;
	auto pc = p->getcomponent();
	if(!pc || !pc->trigger)
		return false;
	return strcmp(pc->trigger, last_id) == 0;
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
		if(count >= limit)
			return false;
	}
	return true;
}

static bool filter_scored_objectives(const void* object) {
	auto p = (card*)object;
	return !p->is(player) && script_allow(p->getcomponent()->effect);
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

static void group_province(int bonus) {
	querry.group(get_province);
}

static void group_unit(int bonus) {
	querry.group(get_unit);
}

static void set_value(int bonus) {
	last_value = bonus;
}

static void upgrade_player(int bonus) {
	player->setupgrade(last_upgrade);
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

static void pay_plus_good(ability_s v, int& bonus) {
	if(bonus < 0)
		bonus = 0;
	if(v != Gold) {
		if(player->current.abilities[v] >= bonus) {
			player->current.abilities[v] -= bonus;
			bonus = 0;
		} else {
			bonus -= player->current.abilities[v];
			player->current.abilities[v] = 0;
		}
	}
	if(player->current.abilities[Gold] >= bonus) {
		player->current.abilities[Gold] -= bonus;
		bonus = 0;
	} else {
		bonus -= player->current.abilities[Gold];
		player->current.abilities[Gold] = 0;
	}
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
	if(yesno(getdescription("StrategySecondaryAsk"), last_strategy->getname()))
		pay_hero(bonus);
	else
		script_stop();
}

static void pay_ability(const char* id, ability_s v, ability_s currency, int gain, int cost, int maximum_cap, abilitya& gainer, abilitya& payer) {
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
	gainer.abilities[v] += gain * i;
	payer.abilities[currency] -= cost * i;
}

static void pay_hirelings(int bonus) {
	pay_ability(last_script->id, Hirelings, Gold, 1, 1, bonus, *last_army, player->current);
}

static void choosing_reset() {
	choosing.clear();
}

static void apply_pay(ability_s v, int bonus) {
	last_pay -= bonus;
	pay_plus_good(v, last_pay);
}

static void apply_pay(int bonus) {
	apply_pay(last_ability, bonus);
}

static void recruit_troops(int army_used, int army_maximum, int build_troops_maximum, int maximum_cost) {
	pushtitle push_title("RecruitTroops");
	last_pay = 0;
	while(true) {
		clear_input(0);
		auto troops_count = choosing.getcount();
		last_pay = choosing.gettotal(Cost);
		last_pay -= player->getarmy(Tools);
		if(last_pay < 0)
			last_pay = 0;
		auto troops_army = army_used + choosing.gettotal(Army);
		auto troops_army_maximum = army_maximum * 10;
		console.addn(getnm("AllowBuildTroops"), troops_count, build_troops_maximum);
		console.addn(getnm("AllowBuildArmy"), (troops_army + 9) / 10, army_maximum);
		console.addn(getnm("AllowCredit"), last_pay, maximum_cost);
		if(choosing) {
			console.addn("---");
			for(auto p : choosing)
				console.addn(getnm("RecruitUnitCost"), p->getname(), p->get(Cost));
		}
		auto total = maximum_cost - last_pay;
		if(troops_count < build_troops_maximum) {
			for(auto p : player->troops) {
				if(troops_army + p->get(Army) > troops_army_maximum)
					continue;
				if(total < p->get(Cost))
					continue;
				an.add(p, getnm("RecruitUnitCost"), p->getname(), p->get(Cost));
			}
		}
		if(choosing)
			an.add(choosing_reset, getnm("RecruitReset"));
		auto p = (uniti*)an.choose(getdescription("RecruitWhatTroop"), getnm("CheckOut"), 1);
		if(!p)
			break; // Check out;
		else if(bsdata<uniti>::have(p))
			choosing.add(p);
		else
			apply_input(p);
	}
	console.clear();
}

static void add_troops(int bonus) {
	for(auto p : choosing)
		add_troop((uniti*)p);
}

static int get_troops(ability_s v, provincei* province, playeri* player) {
	auto result = 0;
	for(auto& e : bsdata<troopi>()) {
		if(e.getprovince() == province && e.player == player)
			result += e.get(v);
	}
	return result;
}

static void recruit_troops_in_province(int bonus) {
	choosing_reset();
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

static void add_secret_goal(int bonus) {
}

static void add_gold(int bonus) {
	player->current.abilities[Gold] += bonus;
}

static void add_army(int bonus) {
	if(bonus == 100)
		bonus = last_value;
	else
		bonus = -last_value;
	last_army->abilities[last_ability] += bonus;
}

static void add_enemy_casualty(int bonus) {
	auto enemy = get_enemy_army();
	if(!enemy)
		return;
	enemy->suffer(bonus, false);
	enemy->applycasualty();
	update_ui();
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
	pay_ability(last_script->id, Hero, Influence, 1, bonus, 3, player->current, player->current);
}

static bool allow_pay_goods(int bonus) {
	return player->current.abilities[Goods] >= bonus;
}
static void pay_goods(int bonus) {
	player->current.abilities[Goods] -= bonus;
}

static bool allow_pay_resource_cost(int bonus) {
	return allow_pay(Resources, bonus);
}
static void pay_resource_cost(int bonus) {
	if(!allow_pay_resource_cost(bonus) || !yesno(getnm("DoYouWishPayFor"), bonus)) {
		script_stop();
		return;
	}
	pay_plus_good(Resources, bonus);
}

static void need_resources(int bonus) {
	if(allow_pay_resource_cost(bonus))
		pay_plus_good(Resources, bonus);
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
	choosing.clear();
}

static void choose_movement() {
	choosing.clear();
	while(true) {
		make_wave();
		querry.collectiona::select(bsdata<troopi>::source, filter_move_range, true);
		querry.match(filter_province_activated, false);
		clear_input(0);
		for(auto p : querry) {
			if(choosing.find(p) != -1)
				continue;
			an.add(p, "%1 (%2)", p->getname(), p->getprovince()->getname());
		}
		for(auto p : choosing) {
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
			choosing.add(result);
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
	apply_movement(choosing);
}

static void make_action(int bonus) {
	pushtitle push(last_script->id);
	pushvalue push_strategy(last_strategy, (strategyi*)0);
	focus_player(0);
	an.clear();
	add_component_cards("MakeAction");
	add_tactic_cards("MakeAction");
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

static void select_player_tactic_cards(int bonus) {
	querry.collectiona::select(bsdata<card>::source, filter_tactic_cards, bonus >= 0);
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

static void select_upgrade(int bonus) {
	querry.collectiona::select(bsdata<upgradei>::source, filter_player_upgrade, bonus >= 0);
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

static void select_public_goals(int bonus) {
	querry.select(bsdata<decki>::elements + ObjectivesDeck, 0, 0);
}

static void build_structure(int bonus) {
	add_structure(last_structure);
	update_ui();
}

static void establish_control(int bonus) {
	if(bonus >= 0) {
		if(count_units(province, player) > 0)
			province->player = player;
	} else
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
	defender.engage("Milita", bonus, count_structures(province));
}

static void attack_hirelings(int bonus) {
	attacker.engage("Hirelings", 2, bonus);
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
	auto d = defender.getstrenght() + province->get(Strenght) + province->getbonus(Strenght);
	if(a >= d)
		winner_army = &attacker;
	else
		winner_army = &defender;
}

static void prepare_ability(ability_s v) {
	if(attacker.player)
		attacker.abilities[v] += attacker.player->getarmy(v);
	if(defender.player)
		defender.abilities[v] += defender.player->getarmy(v);
}

static void prepare_ability_if_valid(armyi& source, ability_s v) {
	if(!source.player)
		return;
	if(source.abilities[v] > 0 && v < sizeof(source.abilities) / sizeof(source.abilities[0]))
		source.abilities[v] += source.player->getarmy(v);
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
	defender.abilities[Milita] += province->get(Milita) + province->getbonus(Milita);
	if(!attacker.troops || !defender.troops) {
		script_stop();
		return;
	}
	attacker.prepare(Shield);
	defender.prepare(Shield);
	prepare_ability(Combat);
	prepare_ability(Shoots);
	prepare_ability(Shield);
	prepare_ability(Strenght);
}

static void apply_casualty(int bonus) {
	if(console) {
		pause(getnm("ApplyCasualty"));
		console.clear();
	}
	attacker.applycasualty();
	attacker.casualty.clear();
	defender.applycasualty();
	defender.casualty.clear();
	determine_winner();
	update_ui();
}

static void attack_army(armyi& source, ability_s type, bool milita_attack = false, bool hirelings_attack = false) {
	pushvalue push_player(player, source.player);
	pushvalue push_army(last_army, &source);
	source.setheader("###%1");
	apply_trigger(0);
	if(milita_attack)
		source.engage(getnm("Milita"), 2 + source.get(Combat), source.get(Milita));
	if(hirelings_attack)
		source.engage(getnm("Hirelings"), 3 + source.get(Combat), source.get(Hirelings));
	source.engage(type);
}

static void hail_arrows(int bonus) {
	pushtitle header(last_script->id, last_script->id);
	console.clear();
	attack_army(attacker, Shoots);
	attack_army(defender, Shoots);
	attacker.suffer(defender.abilities[Damage]);
	defender.abilities[Damage] = 0;
	defender.suffer(attacker.abilities[Damage]);
	attacker.abilities[Damage] = 0;
	apply_casualty(0);
}

static void melee_clash(int bonus) {
	pushtitle header(last_script->id, last_script->id);
	console.clear();
	attack_army(attacker, Damage, false, true);
	attack_army(defender, Damage, true, true);
	attacker.abilities[Hirelings] = 0;
	defender.abilities[Hirelings] = 0;
	attacker.suffer(defender.abilities[Damage]);
	defender.abilities[Damage] = 0;
	defender.suffer(attacker.abilities[Damage]);
	attacker.abilities[Damage] = 0;
	apply_casualty(0);
}

static void choose_querry_list(int bonus) {
	pushtitle header(last_list->id);
	entitya choosing;
	while(true) {
		console.clear();
		clear_input(0);
		if(choosing) {
			auto pd = getdescription(stw(last_id, "Choosed"));
			if(pd) {
				console.addn("###");
				console.add(pd, player->getname(), province->getname());
				for(auto p : choosing)
					console.addn(p->getname());
			}
		} else {
			auto p = getdescription(stw(last_id, "Empthy"));
			if(p)
				console.add(p, player->getname(), province->getname());
		}
		for(auto p : querry) {
			if(choosing.find(p) == -1)
				add_input(*p);
		}
		if(choosing)
			an.add((void*)1, getnm("ResetList"));
		auto result = an.choose(get_title(last_id), getnm(stw("Apply", last_id)), 0);
		if(!result)
			break;
		else if(result == (void*)1)
			choosing.clear();
		else if(bsdata<card>::have(result))
			choosing.add(result);
	}
	querry = choosing;
}

static void choose_tactics(int bonus) {
	auto push_id = last_list->id;
	pushvalue push_player(player, last_army->player);
	pushtitle header(last_list->id);
	while(true) {
		console.clear();
		clear_input(0);
		if(last_army->tactics) {
			console.addn(getnm("ChooseTacticsPrompt"), last_army->player->getname());
			for(auto p : last_army->tactics)
				console.addn(p->getname());
		}
		for(auto& e : bsdata<card>()) {
			if(e.player != player)
				continue;
			auto p = e.getcomponent();
			if(!p)
				continue;
			if(!p->usedeck())
				continue;
			if(!p->trigger || strcmp(p->trigger, push_id) != 0)
				continue;
			if(last_army->tactics.find(&e) != -1)
				continue;
			if(!script_allow(p->effect))
				continue;
			add_input(e);
		}
		auto result = an.choose(get_title(last_id), getnm("ApplyTactics"), 0);
		if(!result)
			break;
		else if(bsdata<card>::have(result))
			last_army->tactics.add(result);
	}
}

static void set_army_tactics(int bonus) {
	last_army->tactics = querry;
}

static void attacker_army(int bonus) {
	last_army = &attacker;
	player = last_army->player;
}

static void defender_army(int bonus) {
	last_army = &defender;
	player = last_army->player;
}

static void winner_army_script(int bonus) {
	if(!winner_army)
		return;
	last_army = winner_army;
	player = last_army->player;
}

static bool is_allow_actions() {
	for(auto& e : bsdata<playeri>()) {
		if(!e.is(Used))
			return true;
	}
	return false;
}

static void if_attacker(int bonus) {
	auto result_true = (attacker.player == player);
	if(result_true != (bonus >= 0))
		script_stop();
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

static void if_structures(int bonus) {
	auto result_true = count_structures(province) > 0;
	if(result_true != (bonus >= 0))
		script_stop();
}

static void if_win_battle(int bonus) {
	last_army = winner_army;
	auto result_true = (!last_army || last_army == &attacker);
	if(result_true != (bonus >= 0))
		script_stop();
}

static void winner_player(int bonus) {
	if(winner_army)
		player = winner_army->player;
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

static int compare_cards(const void* v1, const void* v2) {
	auto e1 = *((entity**)v1);
	auto e2 = *((entity**)v2);
	auto p1 = e1->getcomponent();
	auto p2 = e2->getcomponent();
	if(p1->priority != p2->priority)
		return p1->priority - p2->priority;
	if(e1->player != e2->player)
		return (e1->player == attacker.player) ? 1 : -1;
	return p1 - p2;
}

static void play_tactic() {
	pushvalue push_player(player, last_card->player);
	pushvalue push_army(last_army, get_player_army());
	pushtitle push_title(last_card->getcomponent()->id);
	console.clear();
	auto pd = getdescription(stw(last_card->getcomponent()->id, "Use"));
	if(!pd)
		pd = getdescription("PlayerTacticUse");
	console.add(pd);
	pause();
	last_card->play();
	last_card->discard();
	update_ui();
}

static void play_tactics(int bonus) {
	pushvalue push_card(last_card);
	querry = attacker.tactics;
	querry.add(defender.tactics);
	querry.sort(compare_cards);
	card_begin = (card**)querry.begin();
	card_end = (card**)querry.end();
	while(card_begin < card_end) {
		last_card = *card_begin++;
		play_tactic();
	}
	if(!attacker.troops || !defender.troops)
		script_stop();
	prepare_ability_if_valid(defender, Milita);
	determine_winner();
}

static void remove_enemy_tactics(int bonus) {
	auto p = card_begin;
	auto ps = card_begin;
	while(p < card_end) {
		if((*p)->player == player)
			*ps++ = *p;
		p++;
	}
	card_end = ps;
}

static void retreat_troops_tactic(int bonus) {
	pushvalue push(province);
	auto start_province = province;
	make_wave();
	select_provincies(0);
	querry.match(filter_neightboard, true);
	querry.collectiona::match(filter_player, true);
	current_province(-1);
	if(querry) {
		choose_province(0);
		if(province) {
			move_troops(player, push.value, province);
			activity_token(0);
		}
	} else
		destory_troops(player, province);
	get_player_army()->troops.clear();
}

static void retreat_troops(int bonus) {
	auto looser = get_looser_army();
	if(!looser || !looser->player || !looser->troops)
		return;
	pushvalue push(province);
	pushvalue push_player(player, looser->player);
	retreat_troops_tactic(bonus);
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
	{"FilterAllowedUpgrade", filter_allowed_upgrade},
	{"FilterHomeland", filter_homeland},
	{"FilterNeightboard", filter_neightboard},
	{"FilterPlayer", filter_player},
	{"FilterProvinceLimit", filter_province_limit},
	{"FilterScoredObjective", filter_scored_objectives},
	{"FilterStructureLimit", filter_structure_limit},
	{"FilterSpeaker", filter_speaker},
	{"FilterTacticCards", filter_player},
	{"FilterTrigger", filter_trigger},
};
BSDATAF(filteri);
BSDATA(script) = {
	{"ActivityToken", activity_token},
	{"AddArmy", add_army},
	{"AddEnemyCasualty", add_enemy_casualty},
	{"AddGold", add_gold},
	{"AddLeaders", add_leaders},
	{"AddPlayerTag", add_player_tag},
	{"AddProvinceInfluence", add_province_influence},
	{"AddProvinceResources", add_province_resource},
	{"AddSecretGoal", add_secret_goal},
	{"AddStart", add_start},
	{"AddTroops", add_troops},
	{"AddValue", add_value},
	{"AttackerArmy", attacker_army},
	{"AttackHirelings", attack_hirelings},
	{"AttackMilita", attack_milita},
	{"ApplyCasualty", apply_casualty},
	{"ApplyPay", apply_pay},
	{"ApplyTrigger", apply_trigger},
	{"BuildStructure", build_structure},
	{"PutCardOnTable", put_card_on_table},
	{"ChooseCardsDiscard", choose_cards_discard},
	{"ChooseMovement", choose_movement},
	{"ChooseProvince", choose_province, allow_choose},
	{"ChooseTactics", choose_tactics, allow_choose},
	{"ChooseQuerry", choose_querry, allow_choose},
	{"ChooseQuerryList", choose_querry_list, allow_choose},
	{"CurrentProvince", current_province},
	{"DefenderArmy", defender_army},
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
	{"GroupUnit", group_unit, allow_select},
	{"HailOfArrows", hail_arrows},
	{"IfAttacker", if_attacker},
	{"IfControlCapital", if_control_capital},
	{"IfNoQuerryBreak", if_no_querry_break},
	{"IfStructures", if_structures},
	{"IfWinBattle", if_win_battle},
	{"InputQuerry", input_querry},
	{"MakeAction", make_action},
	{"MakeWave", make_wave},
	{"MeleeClash", melee_clash},
	{"NeedResources", need_resources, allow_pay_resource_cost},
	{"NoConditions", script_none, allow_and_stop},
	{"OpenTactic", open_tactic},
	{"PayForLeaders", pay_for_leaders, allow_pay_for_leaders},
	{"PayGold", pay_gold, allow_pay_gold},
	{"PayGoods", pay_goods, allow_pay_goods},
	{"PayHero", pay_hero, allow_pay_hero},
	{"PayHeroYesNo", pay_hero_yesno, allow_pay_hero},
	{"PayHirelings", pay_hirelings, allow_pay_hero},
	{"PayResources", pay_resources, allow_pay_resources},
	{"PayResourcesCost", pay_resource_cost, allow_pay_resource_cost},
	{"PickCards", pick_cards},
	{"PickSpeaker", pick_speaker},
	{"PickStrategy", pick_strategy},
	{"PlayerUsed", player_used},
	{"PlayTactics", play_tactics},
	{"PrepareArmy", prepare_army},
	{"PushPlayer", push_player},
	{"PushProvince", push_province},
	{"RecruitTroopsInProvince", recruit_troops_in_province},
	{"RefreshInfluence", refresh_influence},
	{"RefreshResources", refresh_resources},
	{"RefreshTrade", refresh_trade},
	{"RemoveStrategy", remove_strategy},
	{"RemoveEnemyTactics", remove_enemy_tactics},
	{"Repeat", repeat_statement},
	{"RetreatTroops", retreat_troops},
	{"RetreatTroopsTactic", retreat_troops_tactic},
	{"SelectPlayers", select_players, allow_select},
	{"SelectPlayersBySpeaker", select_players_speaker, allow_select},
	{"SelectPlayerTacticCards", select_player_tactic_cards, allow_select},
	{"SelectPlayerTroops", select_player_troops, allow_select},
	{"SelectProvinceStructures", select_province_structures, allow_select},
	{"SelectProvinces", select_provincies, allow_select},
	{"SelectPublicGoals", select_public_goals, allow_select},
	{"SelectStrategy", select_strategy, allow_select},
	{"SelectStructures", select_structures, allow_select},
	{"SelectUpgrade", select_upgrade, allow_select},
	{"SetArmyTactics", set_army_tactics},
	{"SetProvinceCapital", set_province_capital, allow_script},
	{"SetValue", set_value, allow_script},
	{"SetValueQuerryCount", set_value_querry_count, allow_script},
	{"UpgradePlayer", upgrade_player},
	{"WinnerArmy", winner_army_script},
};
BSDATAF(script)