#include "action.h"
#include "answers.h"
#include "army.h"
#include "building.h"
#include "collection.h"
#include "costitem.h"
#include "draw.h"
#include "draw_object.h"
#include "game.h"
#include "gender.h"
#include "list.h"
#include "report.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "rand.h"
#include "randomizer.h"
#include "script.h"
#include "statable.h"

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

void add_line(stringbuilder& sb, const costa& source);
void next_turn();
void player_turn();
void update_provinces_ui();

static int units_gold_upkeep = 3;
static char sb_buffer[4096];
static stringbuilder sb(sb_buffer);
static int troops_movement;
static int modal_result;
static costa rewards;

static void clear_rewards() {
	memset(rewards, 0, sizeof(rewards));
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

static bool build_upgrade() {
	for(auto& e : bsdata<building>()) {
		if(e.province == province && lastbuilding->upgrade == e.type) {
			e.type = lastbuilding;
			return true;
		}
	}
	return false;
}

static void* choose_province_header(const char* title) {
	auto push_header = answers::header;
	answers::header = province->getname();
	auto pu = an.choose(title, getnm("Cancel"));
	answers::header = push_header;
	return pu;
}

static bool have_builded(const buildingi* pv) {
	for(auto& e : bsdata<building>()) {
		if(e.province == province && e.type == pv)
			return true;
	}
	return false;
}

static bool have_exist(const sitei* pv) {
	for(auto& e : bsdata<site>()) {
		if(e.province == province && e.type == pv)
			return true;
	}
	return false;
}

static bool have_upgraded(const buildingi* pb, const buildingi* p) {
	while(pb->upgrade) {
		if(pb->upgrade == p)
			return true;
		pb = pb->upgrade;
	}
	return false;
}

static bool have_upgraded(const buildingi* p) {
	for(auto& e : bsdata<building>()) {
		if(e.province == province && have_upgraded(e.type, p))
			return true;
	}
	return false;
}

static bool is_coastal(int bonus) {
	auto ocean = bsdata<landscapei>::find("Ocean");
	if(!ocean)
		return false;
	neightbors source;
	source.selectn(province);
	for(auto p : source) {
		if(p->landscape == ocean)
			return true;
	}
	return false;
}

static bool canrecruit(int bonus) {
	return province->recruit < province->current[Recruit];
}

static bool is_province_player(const void* pv) {
	return ((provincei*)pv)->player == player;
}

static bool is_province_ocean(const void* pv) {
	return ((provincei*)pv)->iswater();
}

static bool canbuild(int bonus) {
	if(have_builded(lastbuilding))
		return false;
	if(lastbuilding->upgrade) {
		if(!have_builded(lastbuilding->upgrade))
			return false;
	} else {
		if(have_upgraded(lastbuilding))
			return false;
	}
	if(lastbuilding->conditions && !lastbuilding->isallow())
		return false;
	return true;
}

static void payunit(int bonus) {
	costa cost = {};
	subvalue(player->resources, cost);
}

static void recruit() {
	province->units++;
}

static void add_site(int bonus) {
	auto p = bsdata<site>::add();
	p->type = lastsite;
	p->province = province;
}

static void recruit(int bonus) {
	for(auto i = 0; i < bonus; i++)
		recruit();
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

static void standart_result(void* result) {
	if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else
		((fnevent)result)();
}

static void choose_province() {
	draw::buttonparam();
}

static bool player_building(const void* pv) {
	return ((building*)pv)->province && ((building*)pv)->province->player == player;
}

static bool player_province_building(const void* pv) {
	return ((building*)pv)->province == province;
}

static void add_answers(fnevent proc, const char* id, int count, const char* piece_id = 0) {
	if(count > 0) {
		if(piece_id)
			an.add(proc, "%1 (%2i %-*3)", getnm(id), count, getnm(piece_id));
		else
			an.add(proc, "%1 (%2i %Piece)", getnm(id), count);
	} else
		an.add(proc, getnm(id));
}

static void add_sites(fnevent proc, int count, int explore) {
	an.add(proc, "%Explore %3i%% (%1i %-*2)", count, getnm("Site"), explore);
}

static int get_buildings_upkeep(costn v) {
	auto result = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province && e.province->player == player)
			result += e.type->upkeep[v];
	}
	return get_value("BuildingsUpkeep", -result);
}

static int get_buildings_effect(const buildingi* b, costn v) {
	auto result = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province && e.province->player == player && e.type == b)
			result += e.type->effect[v];
	}
	return get_value(b->id, result);
}

static int get_buildings_effect(costn v) {
	auto result = 0;
	for(auto& e : bsdata<buildingi>())
		result += get_buildings_effect(&e, v);
	return result;
}

static int get_units_upkeep(costn v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += e.units * units_gold_upkeep;
	}
	return get_value("UnitsUpkeep", -result);
}

static int get_provinces_income(costn v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += e.landscape->effect[v] + e.income[v];
	}
	return get_value("ProvincesIncome", result);
}

static int get_provinces_upkeep(costn v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += e.landscape->upkeep[v];
	}
	return get_value("ProvincesUpkeep", -result);
}

int get_income(costn v) {
	auto result = get_provinces_income(v);
	result += get_buildings_effect(v);
	result += get_value("FaithBonus", player->faith[v]);
	result += get_provinces_upkeep(v);
	result += get_buildings_upkeep(v);
	result += get_units_upkeep(v);
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

static void update_provinces() {
	for(auto& e : bsdata<provincei>())
		e.update();
	for(auto& e : bsdata<building>())
		addvalue(e.province->current, e.type->effect);
	//for(auto& e : bsdata<troop>()) {
	//	if(e.moveto && e.player == player) {
	//		if(e.moveto->player != player)
	//			e.moveto->attack += e.type->effect[Strenght];
	//	} else if(e.province)
	//		e.province->defend += e.type->effect[Strenght];
	//}
	//for(auto& e : bsdata<heroi>()) {
	//	if(!e.province)
	//		continue;
	//	if(e.player == player) {
	//		if(e.province->player == player)
	//			e.province->defend += e.get(Strenght);
	//		else
	//			e.province->attack += e.get(Strenght);
	//	}
	//}
}

static void mark_player_provinces() {
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			e.setzerocost();
	}
}

static void update_province_visibility(const provincei* province) {
}

static void update_province_visibility() {
	for(auto& e : bsdata<neighbor>()) {
		if(bsdata<provincei>::elements[e.n1].player == player
			|| bsdata<provincei>::elements[e.n2].player == player) {
			visibility.set(e.n1);
			visibility.set(e.n2);
		}
	}
	update_provinces_ui();
}

static void update_player_income() {
	memset(player->income, 0, sizeof(player->income));
	for(auto v = (costn)0; v < Limit; v = (costn)(v + 1))
		player->income[v] = get_income(v);
	for(auto v = (costn)0; v < Limit; v = (costn)(v + 1))
		player->income[v] = get_income_modified(v, player->income[v]);
}

static void update_player(int bonus) {
	update_provinces();
	update_player_income();
	visibility.clear();
	update_province_visibility();
}

static void gain_income(int bonus) {
	addvalue(player->resources, player->income);
}

static void random_site(int bonus) {
	collection<sitei> source;
	source.select();
	lastsite = source.random();
}

static void build(int bonus) {
	if(!build_upgrade()) {
		auto p = bsdata<building>::add();
		p->type = lastbuilding;
		p->province = province;
	}
	update_player(0);
}

template<> void fnscript<buildingi>(int value, int bonus) {
	lastbuilding = bsdata<buildingi>::elements + value;
	if(bonus > 0) {
		build(bonus);
	} else if(bonus < 0) {
	}
}

static void recruit_units() {
}

static void choose_troops() {
	pushvalue push_image(answers::resid, "Units");
	while(!draw::isnext()) {
		an.clear();
		if(province->recruit < province->current[Recruit])
			an.add(recruit_units, getnm("Recruit"), province->current[Recruit] - province->recruit);
		auto result = an.choose(0, getnm("Cancel"));
		if(!result)
			return;
		else
			((fnevent)result)();
	}
}

static void choose_build(int bonus) {
	an.clear();
	for(auto& e : bsdata<buildingi>()) {
		lastbuilding = &e;
		if(!canbuild(0))
			continue;
		if(!isenought(player->resources, e.cost))
			continue;
		an.add(lastbuilding, getnm(lastbuilding->id));
	}
	lastbuilding = (buildingi*)an.choose(getnm("WhatDoYouWantToBuild"), getnm("Cancel"));
}

static void paycost(int bonus) {
	subvalue(player->resources, lastbuilding->cost);
}

static void add_building() {
	auto push_building = lastbuilding;
	choose_build(0);
	if(lastbuilding) {
		paycost(0);
		build(0);
		province->builded++;
	}
	lastbuilding = push_building;
}

static void remove_building(building* pb) {
	if(!yesno(getnm("DoYouWantRemoveBuilding"), pb->type->getname()))
		return;
	pb->clear();
}

static void choose_buildings() {
	pushvalue push_image(answers::resid, "Buildings");
	while(!draw::isnext()) {
		update_header("%1 - %Settlements", province->getname());
		an.clear();
		collection<building> buildings; buildings.select(player_province_building);
		for(auto p : buildings)
			an.add(p, p->type->getname());
		auto maximum_build = 1;
		if(province->buildings < province->current[Size] && province->builded < maximum_build)
			an.add(add_building, getnm("AddBuilding"), maximum_build - province->builded);
		auto result = an.choose(0, getnm("Cancel"), 1);
		if(!result)
			break;
		if(bsdata<building>::have(result))
			remove_building((building*)result);
		else
			standart_result(result);
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
		auto result = an.choose(0, getnm("Cancel"), 1);
		if(!result)
			break;
		else if(bsdata<site>::have(result)) {
			location = (site*)result;
			choose_site_option(location);
		} else
			standart_result(result);
	}
}

static const char* header(const char* prefix, const char* id) {
	auto pn = getnme(str("%1%2", prefix, id));
	if(pn)
		return pn;
	return getnm(id);
}

static void add_province_units() {
}

static bool player_troop_cost(const void* pv) {
	return false;
}

static void clear_troops_movement() {
}

static void apply_confirm() {
	modal_result = 1;
}

static bool troops_mobilization(int value, int defence) {
	pushvalue push_image(answers::resid, "Marching");
	pushvalue push_console(answers::prompt, (const char*)sb_buffer);
	pushvalue push_movement(troops_movement, value);
	modal_result = 0;
	while(!draw::isnext() && !modal_result) {
		clear_wave();
		province->makewave();
		army troops_army;
		troops_army.clear();
		troops_army.province = province;
		update_provinces();
		sb.clear();
		troops_army.act(sb, getnm("ArmyMobilize"));
		an.clear();
		for(auto& e : bsdata<provincei>()) {
			if(e.player != player)
				continue;
			if(&e == province)
				continue;
			if(!e.units)
				continue;
			if(e.getcost() > value)
				continue;
		//	if(e.moveto)
		//		continue;
			an.add(&e, e.getname());
		}
		if(troops_army.get(Strenght) >= defence)
			an.add(apply_confirm, getnm("Confirm"));
		else if(defence)
			sb.addn(getnm("MinimalStrenghtRequired"), defence);
		auto result = an.choose(0, getnm("Cancel"));
		if(!result) {
			clear_troops_movement();
			break;
		} else
			standart_result(result);
	}
	update_provinces();
	return modal_result != 0;
}

static void add_settlement_options() {
	collection<building> buildings; buildings.select(player_province_building);
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
		if(province->player == player) {
			add_province_units();
			add_settlement_options();
		}
		auto result = an.choose(0, getnm("Cancel"));
		if(!result)
			province = 0;
		else
			standart_result(result);
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

bool is_oneof(const sitei* p, variants& conditions) {
	for(auto v : conditions) {
		if(p->isallow(v))
			return true;
	}
	return false;
}

bool sitei::isallow(variant v) const {
	if(v.iskind<sitei>())
		return have_exist(bsdata<sitei>::elements + v.value);
	else if(v.iskind<script>())
		return fntest<script>(v.value, v.counter);
	else if(v.iskind<landscapei>())
		return province->landscape == (bsdata<landscapei>::elements + v.value);
	else if(v.iskind<listi>())
		return is_oneof(this, bsdata<listi>::elements[v.value].elements);
	return true;
}

bool sitei::isallow() const {
	for(auto v : conditions) {
		if(!isallow(v))
			return false;
	}
	return true;
}

static void battle_stage(stringbuilder& sb, army& attacker, army& defender, costn ability) {
	auto suffix = bsdata<costi>::elements[ability].id;
	army af = attacker; af.match(ability, true);
	army df = defender; df.match(ability, true);
	if(af && !df)
		return;
	sb.addn("---\n");
	sb.addn("$image Battle%1 0 'art/images'\n", suffix);
	if(af && df) {
		af.act(sb, getnm(str("Attacker%1", suffix)));
		df.act(sb, getnm(str("Defender%1", suffix)));
	} else if(af)
		af.act(sb, getnm(str("Attacker%1", suffix)));
	else
		df.act(sb, getnm(str("Attacker%1", suffix)));
	auto ad = af.get(ability);
	auto dd = df.get(ability);
	army ac; ac.setcasualty(attacker);
	army dc; dc.setcasualty(defender);
	attacker.damage(ac, dd);
	defender.damage(dc, ad);
	//if(ac) {
	//	sb.addsep('\n');
	//	ac.act(sb, getnm("CasualtiesTotal"));
	//	attacker.casualty(ac, defender);
	//}
	//if(dc) {
	//	sb.addsep('\n');
	//	dc.act(sb, getnm("CasualtiesTotal"));
	//	defender.casualty(dc, attacker);
	//}
}

static void prepare_battle(army& attacker, army& defender) {
	// Attacker and defender can disable tactic
	//if(defender.tactic && attacker.tactic) {
	//	if(attacker.tactic->disable.is(getbsi(defender.tactic)))
	//		defender.tactic = 0;
	//	else if(defender.tactic->disable.is(getbsi(attacker.tactic)))
	//		attacker.tactic = 0;
	//}
}

static bool battle_result(stringbuilder& sb, army& attacker, army& defender) {
	auto win_battle = false;
	sb.addn("---\n");
	sb.addn("$image BattleWin 0 'art/images'\n");
	if(attacker && defender) {
		attacker.act(sb, getnm("AttackerFinale"));
		sb.addsep('\n');
		defender.act(sb, getnm("DefenderFinale"));
		sb.addsep('\n');
		attacker.strenght = attacker.get(Strenght);
		defender.strenght = defender.get(Strenght);
		win_battle = attacker.strenght >= defender.strenght;
		if(win_battle) {
			attacker.spoils[Fame] += 1;
			attacker.act(sb, getnm("BattleTotal"));
		} else {
			defender.spoils[Fame] += 1;
			defender.act(sb, getnm("BattleTotal"));
		}
	} else if(attacker) {
		attacker.act(sb, getnm("AttackerWin"));
		attacker.spoils[Fame] += 3;
		win_battle = true;
	} else if(defender) {
		defender.act(sb, getnm("AttackerWin"));
		defender.spoils[Fame] += 1;
	}
	return win_battle;
}

static void gain_spoils(stringbuilder& sb, army& troops) {
	if(isempthy(troops.spoils))
		return;
	if(!troops.player)
		return;
	sb.addsep('\n');
	troops.act(sb, getnm("BattleSpoils"));
	addvalue(troops.player->resources, troops.spoils);
}

static void gain_control(const army& troops) {
	troops.province->player = troops.player;
}

static void retreat_attacker(const army& troops) {
}

static void retreat_defender(const army& troops) {
	neightbors provinces;
	provinces.selectn(troops.province);
	auto push_player = player; player = troops.player;
	provinces.match(is_province_player, true);
	provinces.match(is_province_ocean, false);
	if(provinces) {
		auto random = provinces.random();
		//for(auto p : troops) {
		//	auto pu = find_troop(p, troops.province);
		//	pu->province = random;
		//}
	} else {
		//for(auto p : troops) {
		//	auto pu = find_troop(p, troops.province);
		//	pu->clear();
		//}
	}
	player = push_player;
}

static void conquest(stringbuilder& sb, army& attacker, army& defender) {
	prepare_battle(attacker, defender);
	sb.addn("$image BattleField 0 'art/images'\n");
	attacker.act(sb, getnm("ArmyConquest"), province->getname());
	if(!defender) {
		sb.addsep(' ');
		attacker.act(sb, getnm("YouForceMeetNoResistance"));
		gain_control(attacker);
		return;
	}
	if(attacker.tactic) {
		sb.addsep(' ');
		attacker.act(sb, getnme(ids(attacker.tactic->id, "Info")));
	}
	sb.addsep(' ');
	defender.act(sb, getnm("ArmyDefend"), province->getname());
	if(defender.tactic) {
		sb.addsep(' ');
		defender.act(sb, getnme(ids(defender.tactic->id, "Info")));
	}
	auto win_battle = battle_result(sb, attacker, defender);
	if(win_battle) {
		retreat_defender(defender);
		gain_control(attacker);
		gain_spoils(sb, attacker);
	} else {
		retreat_attacker(attacker);
		gain_spoils(sb, defender);
	}
}

static void add_unit(int bonus) {
	province->units += bonus;
	if(province->units < 0)
		province->units = 0;
}

BSDATA(script) = {
	{"AddSite", add_site},
	{"Build", build, canbuild},
	{"Coastal", 0, is_coastal},
	{"GainIncome", gain_income},
	{"Recruit", recruit},
	{"RandomSite", random_site},
	{"ShowMessages", show_messages},
	{"Unit", add_unit},
	{"UpdatePlayer", update_player},
};
BSDATAF(script)

static bool enemy_province(const void* pv) {
	auto p = (provincei*)pv;
	if(!p->isvisible() || p->iswater())
		return false;
	return p->player != player && p->getcost() <= 1;
}

static bool friendly_province_vacant_army(const void* pv) {
	auto p = (provincei*)pv;
	if(p->player != player)
		return false;
	//troopa troops; troops.select(player_troop);
	//pushvalue push_province(province, p);
	//troops.match(player_province_troop, false);
	return true;
}

static bool friendly_province_site(const void* pv) {
	auto p = (provincei*)pv;
	if(p->player != player)
		return false;
	return true;
}

static bool visible_not_explored_province(const void* pv) {
	auto p = (provincei*)pv;
	if(!p->isvisible() || p->iswater())
		return false;
	return true;
}

static void action_mobilize() {
	//for(auto& e : bsdata<troop>()) {
	//	if(e.player == player && e.moveto == province) {
	//		e.province = e.moveto;
	//		e.moveto = 0;
	//	}
	//}
}

static unsigned reciever(const playeri* player) {
	if(!player)
		return 0;
	return 1 << player->getindex();
}

static unsigned reciever(const playeri* p1, const playeri* p2) {
	return reciever(p1) | reciever(p2);
}

static void action_conquest() {
	char temp[4096]; stringbuilder sb(temp);
	army attacker, defender;
	attacker.clear();
	attacker.select(province, player);
	attacker.province = province;
	attacker.player = player;
	if(!attacker)
		return;
	defender.clear();
	defender.select(province);
	defender.province = province;
	defender.player = province->player;
	conquest(sb, attacker, defender);
	reporti::add(temp, 0, game.turn, reciever(attacker.player, defender.player));
}

static void action_explore() {
	char temp[4096]; stringbuilder sb(temp); sb.clear();
	auto value = xrand(1, 4);
	province->explore(value);
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

static void update_player_per_turn() {
}

void next_turn() {
	game.turn++;
	update_province_per_turn();
	update_player_per_turn();
	update_player(0);
	gain_income(0);
	draw::setnext(show_messages);
	game.write("autosave");
}

BSDATA(actioni) = {
	{"ActionConquer", action_conquest, enemy_province, 0, 4, 1},
	{"ActionExplore", action_explore, visible_not_explored_province, "ActionExplore", 1, 0},
	{"ActionMobilize", action_mobilize, friendly_province_vacant_army, 0, 5, 2},
	{"DefaultActionHeal", action_heal, special_action, 0, 5, 0},
	{"DefaultActionExplore", action_explore, visible_not_explored_province, 0, 1, 0},
};
BSDATAF(actioni)