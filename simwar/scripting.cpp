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
#include "script.h"
#include "site.h"
#include "statable.h"

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

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

static int get_provinces_income(costn v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += e.landscape->income[v];
	}
	return get_value("ProvincesIncome", result);
}

int get_income(costn v) {
	auto result = get_provinces_income(v);
	result += get_buildings_income(v);
	result += get_value("FaithBonus", player->faith[v]);
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

static void update_player(int bonus) {
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

template<> void fnscript<sitei>(int value, int bonus) {
	last_site = bsdata<sitei>::elements + value;
	if(bonus > 0)
		build(bonus);
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
		auto maximum_build = 1;
		if(province->buildings < province->income[Size] && province->builded < maximum_build)
			an.add(add_building, getnm("AddBuilding"), maximum_build - province->builded);
		if(!choose_answer())
			break;
		if(bsdata<site>::have(answer_result))
			remove_building((site*)choose_answer);
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
			((provincei*)answer_result)->units--;
			add_move((provincei*)answer_result, province, player, 1);
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
		auto attacker_strenght = attacker.result[Strenght];
		auto defender_strenght = defender.result[Strenght];
		win_battle = (attacker_strenght > defender_strenght); // Need more strenght to win
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

static void retreat_attacker(const army& troops) {
}

static void retreat_defender(army& troops) {
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
		retreat_defender(defender);
		gain_control(attacker);
	} else
		retreat_attacker(attacker);
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

static bool friendly_province_vacant_army(const void* pv) {
	if(!friendly_province(pv))
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
	if(!troops_mobilization(1, province->getstrenght()))
		return;
	apply_battle();
	show_messages();
}

static void action_explore() {
	province->explore(30);
}

static void acth(stringbuilder& sb, const char* id, ...) {
	auto pn = getnme(id);
	if(!pn)
		return;
	sb.addsep(' ');
}

static void quest_result(stringbuilder& sb, const char* prefix, const variants& reward) {
	acth(sb, str("Investigate%1%2", prefix, location->type->id), getnm(location->type->id));
	script_run(reward);
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

void next_turn() {
	game.turn++;
	update_province_per_turn();
	expend_per_turn_resources();
	update_player(0);
	gain_income(0);
	draw::setnext(show_messages);
	game.write("autosave");
}

BSDATA(actioni) = {
	{"ActionConquer", action_conquest, enemy_province, 4, 1},
	{"ActionExplore", action_explore, visible_not_explored_province, 1, 0},
	// {"ActionMobilize", action_mobilize, friendly_province_vacant_army, 5, 2},
	{"ActionRecruit", action_recruit, can_recruit_troops, 5, 2},
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