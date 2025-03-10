#include "action.h"
#include "answers.h"
#include "army.h"
#include "collection.h"
#include "costitem.h"
#include "draw.h"
#include "draw_object.h"
#include "game.h"
#include "gender.h"
#include "list.h"
#include "moveorder.h"
#include "report.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "rand.h"
#include "randomizer.h"
#include "relation.h"
#include "script.h"
#include "site.h"
#include "statable.h"

gamei game;

void stract(stringbuilder& sb, gendern gender, const char* name, const char* format, const char* format_param);

void add_line(stringbuilder& sb, const costa& source);
void next_turn();
void player_turn();
void update_ui();

static int units_gold_upkeep = 3;
static char sb_buffer[4096];
static stringbuilder sb(sb_buffer);
static int modal_result;
static costa rewards;
static void* answer_result;
static costi* last_cost;

int script_count(int counter) {
	if(counter > 0)
		return counter;
	else if(counter < 0)
		return (d100() < -counter) ? 1 : 0;
	return 1;
}

static void clear_rewards() {
	memset(rewards, 0, sizeof(rewards));
}

static bool pay(costa& resources, const costa& cost, bool run) {
	for(auto i = Resources; i <= Limit; i = (costn)(i + 1)) {
		auto v = cost[i];
		if(!v)
			continue;
		if(resources[i] >= cost[i]) {
			if(run)
				resources[i] -= cost[i];
		} else {
			auto v1 = pay_alternate(i);
			if(v1 == i)
				return false;
			if(resources[i] + resources[v1] >= cost[i]) {
				if(run) {
					v -= resources[i]; resources[i] = 0;
					resources[v1] -= v;
				}
			} else
				return false;
		}
	}
	return true;
}

static void update_header(const char* format, ...) {
	static char temp[260]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	answers::header = temp;
}

static void focus(const provincei* p) {
	if(p)
		slide_camera(p->position);
}

static void update_player_income() {
	memset(player->income, 0, sizeof(player->income));
	for(auto v = (costn)0; v < Limit; v = (costn)(v + 1))
		player->income[v] = get_income(v);
	for(auto v = (costn)0; v < Limit; v = (costn)(v + 1))
		player->income[v] = get_income_modified(v, player->income[v]);
}

static int get_buildings_upkeep(costn v) {
	auto result = 0;
	for(auto& e : bsdata<site>()) {
		if(e.province && e.province->player == player)
			result += e.type->upkeep[v];
	}
	return get_value("BuildingsUpkeep", -result);
}

static int get_buildings_income(costn v) {
	auto result = 0;
	for(auto& e : bsdata<site>()) {
		if(e.province && e.province->player == player)
			result += get_value(e.type->id, e.type->income[v]);
	}
	return result;
}

static int get_additional_income(costn v, const provincei& e) {
	switch(v) {
	case Resources: return e.income[Trade];
	case Influence: return e.income[Noble];
	case Warfire: return e.income[War];
	case Happiness: return e.income[Culture];
	default: return 0;
	}
}

static int get_provinces_income(costn v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += e.landscape->income[v];
	}
	return get_value("ProvincesIncome", result);
}

static int get_provinces_upgrade_income(costn v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += get_additional_income(v, e);
	}
	return get_value("ProvincesUpgradeIncome", result);
}

int get_income(costn v) {
	auto result = get_provinces_income(v);
	result += get_provinces_upgrade_income(v);
	result += get_buildings_income(v);
	result += get_value(player->id, player->faith[v]);
	result += get_buildings_upkeep(v);
	return result;
}

static int get_happiness(costn v, int base) {
	if(v != Gold && v != Lore)
		return 0;
	auto param = player->income[Happiness];
	if(param <= -3)
		return get_value("LowHappiness", base * (param + 2) * 5 / 100);
	else if(param >= 3)
		return get_value("HighHappiness", base * (param - 2) * 5 / 100);
	return 0;
}

int get_income_modified(costn v, int result) {
	auto base = result;
	result += get_happiness(v, base);
	return result;
}

static void update_player_units() {
	player->units = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			player->units += e.units;
	}
	for(auto& e : bsdata<moveorder>()) {
		if(e.player == player)
			player->units += e.count;
	}
}

static void update_provinces() {
	for(auto& e : bsdata<provincei>()) {
		memcpy(e.income, e.landscape->income, sizeof(e.income));
		e.buildings = 0;
		e.sites = 0;
	}
	for(auto& e : bsdata<site>()) {
		if(e.is(Building))
			e.province->buildings++;
		else
			e.province->sites++;
		addvalue(e.province->income, e.type->income);
	}
}

static void update_province_visibility() {
	for(auto& e : bsdata<neighbor>()) {
		if(bsdata<provincei>::elements[e.n1].player == player
			|| bsdata<provincei>::elements[e.n2].player == player) {
			visibility.set(e.n1);
			visibility.set(e.n2);
		}
	}
	update_ui();
}

static void update_player_provinces() {
	for(auto& e : bsdata<playeri>())
		e.provinces = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player)
			e.player->provinces++;
	}
}

static void update_player(int bonus) {
	update_player_provinces();
	update_provinces();
	update_player_income();
	update_player_units();
	visibility.clear();
	update_province_visibility();
}

static void* choose_province_header(const char* title) {
	auto push_header = answers::header;
	answers::header = province->getname();
	auto pu = an.choose(title, getnm("Cancel"));
	answers::header = push_header;
	return pu;
}

static bool have_exist(const sitei* pv) {
	for(auto& e : bsdata<site>()) {
		if(e.province == province && e.type == pv)
			return true;
	}
	return false;
}

static bool is_neighboar(featn v) {
	neightbors source;
	source.selectn(province);
	for(auto p : source) {
		if(p->landscape->is(v))
			return true;
	}
	return false;
}

static bool is_coastal(int bonus) {
	return is_neighboar(Water);
}

static bool is_province_player(const void* pv) {
	return ((provincei*)pv)->player == player;
}

static bool is_province_ocean(const void* pv) {
	return ((provincei*)pv)->iswater();
}

static bool canbuild(int bonus) {
	if(!last_site->is(Building))
		return false;
	if(have_exist(last_site))
		return false;
	return true;
}

static void payunit(int bonus) {
	costa cost = {};
	subvalue(player->resources, cost);
}

static void add_site(int bonus) {
	auto p = bsdata<site>::add();
	p->type = last_site;
	p->province = province;
	p->explore = 0;
}

static void recruit(int bonus) {
	bonus += province->units;
	if(bonus < 0)
		bonus = 0;
	province->units = bonus;
}

void add_neutral(const char* id) {
	auto push_player = player;
	player = 0;
	recruit(1);
	player = push_player;
}

static void block_ocean() {
	for(auto& e : bsdata<provincei>()) {
		if(e.iswater())
			e.setblocked();
	}
}

template<> void fnscript<costi>(int value, int bonus) {
	rewards[value] += bonus;
}

extern fnevent input_province;

static void end_turn() {
	next_turn();
}

static void standart_result() {
	if(bsdata<provincei>::have(answer_result))
		province = (provincei*)answer_result;
	else if(bsdata<actioni>::have(answer_result)) {
		auto p = (actioni*)answer_result;
		pay(player->resources, p->cost, true);
		p->proc();
		update_player(0);
	} else
		((fnevent)answer_result)();
}

static void choose_province() {
	draw::buttonparam();
}

static bool player_building(const void* pv) {
	return ((site*)pv)->province && ((site*)pv)->province->player == player;
}

static bool player_province_building(const void* pv) {
	return ((site*)pv)->province == province;
}

static void add_answers(fnevent proc, const char* id) {
	an.add(proc, getnm(id));
}

static void add_answers(actioni& e) {
	if(e.test && !e.test(province))
		return;
	if(!pay(player->resources, e.cost, false))
		return;
	an.add(&e, e.getname());
}

static void add_answers(fnevent proc, const char* id, int count, const char* piece_id = 0) {
	if(piece_id)
		an.add(proc, "%1 (%2i %-*3)", getnm(id), count, getnm(piece_id));
	else
		an.add(proc, "%1 (%2i %Piece)", getnm(id), count);
}

static void add_sites(fnevent proc, int count, int explore) {
	an.add(proc, "%Explore %3i%% (%1i %-*2)", count, getnm("Site"), explore);
}

static void mark_player_provinces() {
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			e.setzerocost();
	}
}

static void gain_income(int bonus) {
	addvalue(player->resources, player->income);
}

static void random_site(int bonus) {
	collection<sitei> source;
	source.select();
	last_site = source.random();
}

static void build(int bonus) {
	if(have_exist(last_site))
		return;
	auto p = bsdata<site>::add();
	p->type = last_site;
	p->province = province;
	p->explore = 100;
	update_player(0);
}

static void build_district(int bonus) {
	province->income[getbsi(last_cost)]++;
	update_player(0);
}

template<> void fnscript<sitei>(int value, int bonus) {
	last_site = bsdata<sitei>::elements + value;
	if(bonus > 0)
		build(bonus);
}

static void choose_build_district(int bonus) {
	static costn districts[] = {War, Trade, Culture, Noble};
	costa price = {};
	price[Resources] = 10;
	an.clear();
	for(auto a : districts) {
		if(!pay(player->resources, price, false))
			continue;
		auto p = bsdata<costi>::elements + a;
		an.add(p, p->getname());
	}
	last_cost = (costi*)an.choose(getnm("WhatDoYouWantToBuild"), getnm("Cancel"));
}

static void choose_build(int bonus) {
	auto push_last = last_site;
	an.clear();
	for(auto& e : bsdata<sitei>()) {
		last_site = &e;
		if(!canbuild(0))
			continue;
		if(!pay(player->resources, e.cost, false))
			continue;
		an.add(last_site, getnm(last_site->id));
	}
	last_site = push_last;
	last_site = (sitei*)an.choose(getnm("WhatDoYouWantToBuild"), getnm("Cancel"));
}

static void pay_resources(int bonus) {
	costa price = {};
	price[Resources] = bonus * 5;
	pay(player->resources, price, true);
}

static void paycost(int bonus) {
	pay(player->resources, last_site->cost, true);
}

static void add_building() {
	auto push_building = last_site;
	choose_build(0);
	if(last_site) {
		paycost(0);
		build(0);
		province->builded++;
	}
	last_site = push_building;
}

static void add_district() {
	auto push_building = last_cost;
	choose_build_district(0);
	if(last_cost) {
		pay_resources(2);
		build_district(0);
		province->builded++;
	}
	last_cost = push_building;
}

static void remove_building(site* pb) {
	if(!yesno(getnm("DoYouWantRemoveBuilding"), pb->type->getname()))
		return;
	pb->clear();
}

static bool choose_answer() {
	answer_result = an.choose(0, getnm("Cancel"), 1);
	return answer_result != 0;
}

static void choose_buildings() {
	pushvalue push_image(answers::resid, "Buildings");
	while(!draw::isnext()) {
		update_header("%1 - %Settlements", province->getname());
		an.clear();
		collection<site> buildings; buildings.select(player_province_building);
		for(auto p : buildings)
			an.add(p, p->type->getname());
		if(province->buildings < province->income[Size])
			an.add(add_building, getnm("AddBuilding"), province->income[Size] - province->buildings);
		an.add(add_district, getnm("AddDistrict"), province->income[Size] - province->buildings);
		if(!choose_answer())
			break;
		if(bsdata<site>::have(answer_result))
			remove_building((site*)answer_result);
		else
			standart_result();
	}
}

static const char* get_site_header(const site* pv) {
	static char temp[260];
	stringbuilder sb(temp);
	sb.add("%1 - %2", pv->province->getname(), pv->type->getname());
	return temp;
}

static void attack_site() {
}

static void choose_site_option(site* pv) {
	if(!pv)
		return;
	pushvalue push_input(input_province, (fnevent)0);
	pushvalue push_header(answers::header, get_site_header(pv));
	pushvalue push_prompt(answers::prompt);
	pushvalue push_resid(answers::resid);
	if(pv->type->resid)
		answers::resid = pv->type->resid;
	answers::prompt = getnme(pv->type->getid());
	an.clear();
	an.add(attack_site, getnm("VisitSite"));
	auto result = an.choose(0, getnm("Leave"), 1);
	if(!result)
		return;
	else
		((fnevent)result)();
}

static void choose_sites() {
	while(!draw::isnext()) {
		update_header("%1 - %Sites", province->getname());
		an.clear();
		for(auto& e : bsdata<site>()) {
			if(e.province != province)
				continue;
			an.add(&e, e.type->getname());
		}
		if(!choose_answer())
			break;
		else if(bsdata<site>::have(answer_result)) {
			location = (site*)answer_result;
			choose_site_option(location);
		} else
			standart_result();
	}
}

static const char* header(const char* prefix, const char* id) {
	auto pn = getnme(str("%1%2", prefix, id));
	if(pn)
		return pn;
	return getnm(id);
}

static bool player_troop_cost(const void* pv) {
	return false;
}

static void clear_troops_movement() {
	cancel_move(province, player);
}

static void apply_confirm() {
	modal_result = 1;
}

static bool troops_mobilization(int value, int defence) {
	pushvalue push_image(answers::resid, "Marching");
	pushvalue push_console(answers::prompt, (const char*)sb_buffer);
	modal_result = 0;
	while(!draw::isnext() && !modal_result) {
		clear_wave();
		province->makewave();
		update_provinces();
		sb.clear();
		army troops_army;
		troops_army.clear();
		troops_army.player = player;
		troops_army.province = province;
		for(auto& e : bsdata<moveorder>()) {
			if(e.player != player)
				continue;
			if(e.getto() == province)
				troops_army.units += e.count;
		}
		if(province->player == player)
			troops_army.act(sb, getnm("ArmyLogistic"));
		else
			troops_army.act(sb, getnm("ArmyMobilize"));
		an.clear();
		for(auto& e : bsdata<provincei>()) {
			if(e.player != player || &e == province)
				continue;
			if(!e.getunits())
				continue;
			if(e.getcost() > value)
				continue;
			an.add(&e, e.getname());
		}
		if(troops_army.get(Strenght) >= defence)
			an.add(apply_confirm, getnm("Confirm"));
		else if(defence)
			sb.addn(getnm("MinimalStrenghtRequired"), defence);
		if(!choose_answer()) {
			clear_troops_movement();
			break;
		} else if(bsdata<provincei>::have(answer_result)) {
			auto p = (provincei*)answer_result;
			if(p->units > 0 && p->player == player && p->getcost() <= value) {
				((provincei*)answer_result)->units--;
				add_move(p, province, player, 1);
			}
		} else
			standart_result();
		update_ui();
	}
	update_provinces();
	return modal_result != 0;
}

static bool can_recruit_troops(const void* object) {
	if(player->units >= player->income[Warfire])
		return false;
	auto province = (provincei*)object;
	if(province->recruit >= province->income[Recruit])
		return false;
	return true;
}

static void add_settlement_options() {
	collection<site> buildings;
	buildings.select(player_province_building);
	add_answers(choose_buildings, "Settlements", buildings.getcount(), "Building");
}

static void choose_province_options() {
	pushvalue push_image(answers::resid);
	pushvalue push_answers(answers::header);
	pushvalue push_input(input_province, choose_province);
	while(!draw::isnext() && province) {
		update_header(province->getname());
		answers::resid = province->landscape->id;
		an.clear();
		if(province->player == player)
			add_settlement_options();
		for(auto& e : bsdata<actioni>())
			add_answers(e);
		if(!choose_answer())
			province = 0;
		else
			standart_result();
	}
}

static void choose_game_options() {
	char temp[512]; stringbuilder sb(temp);
	pushvalue push_input(input_province, choose_province);
	an.clear();
	auto result = an.choose(0, getnm("EndTurn"), 1);
	if(!result)
		end_turn();
	else if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else
		((fnevent)result)();
}

static void show_messages(int bonus) {
	update_player(0);
	pushvalue header(answers::resid, (const char*)0);
	for(auto& e : bsdata<reporti>()) {
		if(e.turn == game.turn && (e.reciever & (1 << (player->getindex()))) != 0)
			message(e.text, e.header);
	}
}

void player_turn() {
	if(province)
		choose_province_options();
	else
		choose_game_options();
	if(!draw::isnext())
		draw::setnext(player_turn);
}

void show_messages() {
	show_messages(0);
	draw::setnext(player_turn);
}

bool fntestlist(int index, int bonus) {
	for(auto v : bsdata<listi>::elements[index].elements) {
		if(script_allow(v))
			return true;
	}
	return false;
}

static void apply_result(army& attacker, army& defender) {
	if(attacker.result[Death])
		attacker.units -= attacker.result[Death];
	if(attacker.result[Sword])
		defender.casualty += attacker.result[Sword];
	if(attacker.result[Shield])
		attacker.casualty -= attacker.result[Shield];
}

static void apply_casualty(army& attacker, army& defender) {
	if(attacker.casualty < 0)
		attacker.casualty = 0;
	attacker.result[Death] += attacker.casualty;
	attacker.units -= attacker.casualty;
	if(attacker.units < 0) {
		defender.result[Fame] += -attacker.units;
		attacker.units = 0;
	}
	attacker.result[Strenght] += attacker.units;
}

static bool is_spoils(costn v) {
	switch(v) {
	case Resources:
	case Influence:
	case Lore:
	case Fame:
	case Gold:
		return true;
	default:
		return false;
	}
}

static void gain_spoils(army& troops) {
	if(!troops.player)
		return;
	for(auto i = (costn)0; i <= Size; i = (costn)(i + 1)) {
		if(is_spoils(i))
			troops.player->resources[i] += troops.result[i];
	}
}

static bool battle_result(stringbuilder& sb, army& attacker, army& defender) {
	auto win_battle = false;
	province->units = defender.units;
	if(attacker && defender) {
		// Strenght only matter when defender or attackers have troops
		auto attacker_strenght = attacker.result[Strenght];
		auto defender_strenght = defender.result[Strenght];
		win_battle = (attacker_strenght > defender_strenght); // Attacker need more strenght to win defender
		if(win_battle) {
			attacker.result[Fame] += 1;
			attacker.act(sb, getnm("BattleTotal"));
		} else {
			defender.result[Fame] += 1;
			defender.act(sb, getnm("BattleTotal"));
		}
	} else if(attacker) {
		attacker.act(sb, getnm("AttackerWin"));
		win_battle = true;
	} else if(defender) {
		defender.act(sb, getnm("AttackerWin"));
		defender.result[Fame] += 1;
	} else
		attacker.act(sb, getnm("NoneWin"));
	gain_spoils(attacker);
	gain_spoils(defender);
	return win_battle;
}

static void print_result(stringbuilder& sb, army& troops) {
	if(isempthy(troops.result))
		return;
	troops.actn(sb, getnm("BattleSpoils"));
}

static void gain_control(const army& troops) {
	troops.province->player = troops.player;
	troops.province->units = troops.units;
}

static void retreat_army(army& troops) {
	neightbors provinces;
	provinces.selectn(troops.province);
	auto push_player = player; player = troops.player;
	provinces.match(is_province_player, true);
	provinces.match(is_province_ocean, false);
	auto random = provinces.random();
	if(random)
		random->units += troops.units;
	troops.units = 0;
	player = push_player;
}

static deck& combat_tactics(playeri* player) {
	if(player)
		return player->tactics;
	return neutral_tactics;
}

static void apply_tactics(stringbuilder& sb, army& troops) {
	auto& tactics = combat_tactics(troops.player);
	while(true) {
		auto card = tactics.pick();
		addvalue(troops.result, card->effect);
		troops.act(sb, getnm(ids(card->id, "Action")));
		if(!card->is(Discard))
			tactics.add(card);
		if(card->is(Shuffle))
			tactics.shuffle();
		if(card->is(Extend))
			continue;
		break;
	}
}

static void conquest(stringbuilder& sb, army& attacker, army& defender) {
	sb.addn("$image BattleField 0 'art/images'\n");
	attacker.act(sb, getnm("ArmyConquest"), province->getname());
	if(!defender) {
		attacker.act(sb, getnm("YouForceMeetNoResistance"));
		gain_control(attacker);
		return;
	}
	apply_tactics(sb, attacker);
	defender.act(sb, getnm("ArmyDefend"), province->getname());
	apply_tactics(sb, defender);
	apply_result(attacker, defender);
	apply_result(defender, attacker);
	apply_casualty(attacker, defender);
	apply_casualty(defender, attacker);
	auto win_battle = battle_result(sb, attacker, defender);
	print_result(sb, attacker);
	print_result(sb, defender);
	if(win_battle) {
		retreat_army(defender);
		gain_control(attacker);
	} else
		retreat_army(attacker);
}

static void add_unit(int bonus) {
	recruit(bonus);
}

static bool enemy_province(const void* pv) {
	auto p = (provincei*)pv;
	if(!p->isvisible() || p->iswater())
		return false;
	return p->player != player;
}

static bool friendly_province(const void* pv) {
	auto p = (provincei*)pv;
	if(p->player != player)
		return false;
	return true;
}

static bool friendly_province_site(const void* pv) {
	if(!friendly_province(pv))
		return false;
	return true;
}

static site* find_site_not_explored(const provincei* province) {
	for(auto& e : bsdata<site>()) {
		if(e.province == province && e.explore < 100)
			return &e;
	}
	return 0;
}

static bool visible_not_explored_province(const void* object) {
	auto province = (provincei*)object;
	if(!province->isvisible() || province->iswater())
		return false;
	auto ps = find_site_not_explored(province);
	if(!ps)
		return false;
	return true;
}

static void action_mobilize() {
	troops_mobilization(1, 0);
}

static void action_recruit() {
	recruit(1);
	province->recruit++;
}

static unsigned reciever(const playeri* player) {
	if(!player)
		return 0;
	return 1 << player->getindex();
}

static unsigned reciever(const playeri* p1, const playeri* p2) {
	return reciever(p1) | reciever(p2);
}

static void apply_battle() {
	char temp[4096]; stringbuilder sb(temp);
	army attacker, defender;
	attacker.clear();
	attacker.province = province;
	attacker.player = player;
	attacker.attackers();
	if(!attacker)
		return;
	defender.clear();
	defender.province = province;
	defender.player = province->player;
	defender.defenders();
	conquest(sb, attacker, defender);
	reporti::add(temp, 0, game.turn, reciever(attacker.player, defender.player));
}

static void action_conquest() {
	troops_mobilization(1, province->getstrenght());
}

static void action_explore() {
	province->explore(30);
}

static void action_heal() {
}

static bool special_action(const void* pv) {
	return false;
}

static bool assign_random_action_target() {
	if(!action)
		return false;
	collection<provincei> provinces;
	provinces.select(action->test);
	if(!provinces)
		return false;
	return true;
}

static void update_province_per_turn() {
	for(auto& e : bsdata<provincei>()) {
		if(!e)
			continue;
		e.recruit = 0;
		e.builded = 0;
	}
}

static bool is_per_turn(costn v) {
	switch(v) {
	case Resources:
	case Influence:
		return true;
	default:
		return false;
	}
}

static void expend_per_turn_resources() {
	for(auto i = Resources; i <= Limit; i = (costn)(i + 1)) {
		if(is_per_turn(i)) {
			for(auto& e : bsdata<playeri>())
				e.resources[i] = 0;
		}
	}
}

static void move_troops_per_provinces() {
	auto push_province = province;
	auto push_player = player;
	for(auto& e : bsdata<moveorder>()) {
		if(!e)
			continue;
		province = e.getto();
		if(province->player == e.player)
			province->units += e.count;
		else {
			player = e.player;
			apply_battle();
		}
		e.clear();
	}
	player = push_player;
	province = push_province;
}

static void select_provinces(entitya& source, playeri* player) {
	auto pb = bsdata<provincei>::begin();
	auto pe = bsdata<provincei>::end();
	auto ps = source.begin();
	for(; pb < pe; pb++) {
		if(!(*pb))
			continue;
		if(pb->player != player)
			continue;
		*ps++ = pb;
	}
	source.count = ps - source.begin();
}

static bool is_site_allow(const void* object) {
	auto p = (provincei*)object;
	if(p->sites >= p->get(Size))
		return false;
	return true;
}

static void add_new_location(playeri* player) {
	entitya source;
	select_provinces(source, player);
	source.match(is_site_allow, true);
}

static void add_new_location() {
	for(auto& e : bsdata<playeri>()) {
		if(!e)
			continue;
		e.explore -= e.provinces;
		while(e.explore <= 0) {
			e.explore += xrand(20, 40);
			add_new_location(&e);
		}
	}
}

void next_turn() {
	game.turn++;
	move_troops_per_provinces();
	update_province_per_turn();
	expend_per_turn_resources();
	update_relation();
	update_player(0);
	gain_income(0);
	add_new_location();
	draw::setnext(show_messages);
	game.write("autosave");
}

static void mark_start_provinces() {
	for(auto& e : bsdata<provincei>()) {
		if(e.player)
			e.makewave();
	}
}

static void add_neutral_troops() {
	for(auto& e : bsdata<provincei>()) {
		if(e.iswater() || e.player)
			continue;
		auto level = e.getcost();
		if(!level)
			continue;
		auto count = level;
		if(count > 8)
			count = 8;
		for(auto i = 0; i < count; i++) {
			if(d100() < 40)
				continue;
			e.units++;
		}
	}
}

static void neutral_dwelvers() {
	clear_wave();
	mark_start_provinces();
	add_neutral_troops();
}

static void add_cards(deck& source, variants& elements) {
	for(auto v : elements) {
		if(v.iskind<cardi>()) {
			auto count = script_count(v.counter);
			for(auto i = 0; i < count; i++)
				source.add(bsdata<cardi>::elements + v.value);
		}
	}
}

static void add_cards(deck& source) {
	auto p = bsdata<listi>::find("StartTactics");
	if(!p)
		return;
	source.clear();
	add_cards(source, p->elements);
	source.shuffle();
}

static void add_player_tactics() {
	add_cards(neutral_tactics);
	for(auto& e : bsdata<playeri>())
		add_cards(e.tactics);
}

void initialize_game() {
	game.year = 1410;
	game.maximum_fame = 400;
	neutral_dwelvers();
	add_player_tactics();
}

BSDATA(actioni) = {
	{"ActionConquer", action_conquest, enemy_province, 4},
	{"ActionExplore", action_explore, visible_not_explored_province, 1},
	{"ActionMobilize", action_mobilize, friendly_province, 5},
	{"ActionRecruit", action_recruit, can_recruit_troops, 5},
};
BSDATAF(actioni)
BSDATA(script) = {
	{"AddSite", add_site},
	{"Build", build, canbuild},
	{"GainIncome", gain_income},
	{"RandomSite", random_site},
	{"Recruit", recruit},
	{"ShowMessages", show_messages},
	{"Unit", add_unit},
	{"UpdatePlayer", update_player},
};
BSDATAF(script)