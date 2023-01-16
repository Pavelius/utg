#include "answers.h"
#include "army.h"
#include "building.h"
#include "collection.h"
#include "costitem.h"
#include "draw.h"
#include "game.h"
#include "hero.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"
#include "statable.h"
#include "unit.h"

static answers an;
void player_turn();

static void add_description(const char* id, stringbuilder& sb) {
	sb.addn("##%1", getnm(id));
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
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

static void recruit() {
	auto p = bsdata<troop>::add();
	p->type = lastunit;
	p->province = province;
	p->player = player;
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
	auto push_unit = lastunit;
	player = 0;
	lastunit = bsdata<uniti>::find(id);
	if(lastunit)
		recruit(1);
	lastunit = push_unit;
	player = push_player;
}

static void clear_wave() {
	provincei::clearwave();
}

static void block_ocean() {
	for(auto& e : bsdata<provincei>()) {
		if(e.iswater())
			e.setblocked();
	}
}

template<> void fnscript<costi>(int value, int bonus) {
	player->resources[value] += bonus;
}

template<> void fnscript<uniti>(int value, int bonus) {
	lastunit = bsdata<uniti>::elements + value;
	if(bonus > 0) {
		recruit(bonus);
	} else if(bonus < 0) {
	}
}

extern fnevent input_province;

static void remove_per_turn() {
	for(auto i = (cost_s)0; i <= Limit; i = (cost_s)(i + 1)) {
		auto& e = bsdata<costi>::elements[i];
		if(e.is(PerTurn))
			player->resources[i] = 0;
	}
}

void next_turn() {
	remove_per_turn();
	script::run("UpdatePlayer", 0);
	script::run("GainIncome", 0);
	draw::setnext(player_turn);
}

static void end_turn() {
	game.turn++;
	draw::setnext(next_turn);
}

static void choose_province() {
	draw::buttonparam();
}

static bool player_troop(const void* pv) {
	return ((troop*)pv)->player == player;
}

static bool player_province_troop(const void* pv) {
	return ((troop*)pv)->player == player && ((troop*)pv)->province == province;
}

static bool player_building(const void* pv) {
	return ((building*)pv)->province && ((building*)pv)->province->player == player;
}

static bool player_province_building(const void* pv) {
	return ((building*)pv)->province == province;
}

static void select_units(collection<troop>& source) {
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

static int get_buildings_upkeep(cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province && e.province->player == player)
			result += e.type->upkeep[v];
	}
	return get_value("BuildingsUpkeep", -result);
}

static int get_buildings_effect(const buildingi* b, cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province && e.province->player == player && e.type == b)
			result += e.type->effect[v];
	}
	return get_value(b->id, result);
}

static int get_buildings_effect(cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<buildingi>())
		result += get_buildings_effect(&e, v);
	return result;
}

static int get_units_upkeep(cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.player == player)
			result += e.type->upkeep[v];
	}
	return get_value("UnitsUpkeep", -result);
}

static int get_provinces_income(cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += e.landscape->effect[v] + e.income[v];
	}
	return get_value("ProvincesIncome", result);
}

static int get_provinces_upkeep(cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.player == player)
			result += e.landscape->upkeep[v];
	}
	return get_value("ProvincesUpkeep", -result);
}

static int get_income(cost_s v) {
	auto result = get_provinces_income(v);
	result += get_buildings_effect(v);
	result += get_value("FaithBonus", player->faith[v]);
	result += get_provinces_upkeep(v);
	result += get_buildings_upkeep(v);
	result += get_units_upkeep(v);
	return result;
}

static int get_income(cost_s v, stringbuilder& sb) {
	auto push = lastcostitem;
	costitema source; lastcostitem = &source;
	auto result = get_income(v);
	add_cost_items(sb);
	lastcostitem = push;
	return result;
}

static void clear_current() {
	for(auto& e : bsdata<provincei>())
		e.clearcurrent();
}

static void update_provinces() {
	for(auto& e : bsdata<provincei>())
		e.update();
}

static void update_province_buildings() {
	for(auto& e : bsdata<building>())
		addvalue(e.province->current, e.type->effect);
}

static void update_player(int bonus) {
	memset(player->income, 0, sizeof(player->income));
	clear_current();
	update_provinces();
	update_province_buildings();
	for(auto i = (cost_s)0; i <= Limit; i = (cost_s)(i + 1))
		player->income[i] = get_income(i);
}

static void gain_income(int bonus) {
	addvalue(player->resources, player->income);
}

static void random_site(int bonus) {
	collection<sitei> source;
	source.select();
	lastsite = source.random();
}

static void add_line(stringbuilder& sb, const provincei* province, cost_s v, int n) {
	if(!n)
		return;
	auto& e = bsdata<costi>::elements[v];
	if(e.frame == -1)
		return;
	switch(v) {
	case Size:
		sb.adds(":%1i:%2i/%3i", e.frame, province->getbuildings(), n);
		break;
	default:
		sb.adds(":%2i:%1i", n, e.frame);
		break;
	}
}

static void add_line(stringbuilder& sb, int f, int n) {
	if(!n)
		return;
	sb.adds(":%2i:%1i", n, f);
}

static void add_line(stringbuilder& sb, int f, int n, int nm) {
	if(!nm)
		return;
	sb.adds(":%2i:%1i/%3i", n, f, nm);
}

void add_line_upkeep(const provincei* province, stringbuilder& sb) {
	add_line(sb, 4, province->current[Gold]);
	add_line(sb, 6, province->current[Mana]);
	add_line(sb, 2, province->strenght);
	if(province->player != player || !player->resources[Build])
		add_line(sb, 5, province->buildings);
	else
		add_line(sb, 5, province->buildings, province->current[Size]);
}

void add_line(stringbuilder& sb, const costac& source) {
	for(auto i = 0; i <= Limit; i++) {
		auto& e = bsdata<costi>::elements[i];
		auto n = source[i];
		if(!n)
			continue;
		if(e.frame == -1)
			continue;
		add_line(sb, e.frame, n);
	}
}

static void add_line(stringbuilder& sb, const costa& source) {
	for(auto i = 0; i <= Limit; i++) {
		auto& e = bsdata<costi>::elements[i];
		auto n = source[i];
		if(!n)
			continue;
		if(e.frame == -1)
			continue;
		add_line(sb, e.frame, n);
	}
}

static void add_line(stringbuilder& sbo, const char* id, const costa& source) {
	char temp[512]; stringbuilder sb(temp); temp[0] = 0; add_line(sb, source);
	if(temp[0] == 0)
		return;
	sbo.addn("%1: ", getnm(id));
	sbo.add(temp);
}

static void add_line(stringbuilder& sbo, const char* id, const costac& source) {
	costa translate = {}; addvalue(translate, source);
	add_line(sbo, id, translate);
}

static void add_description(const buildingi* p, stringbuilder& sb) {
	sb.addn("##%1", getnm(p->id));
	auto need_line = true;
	for(auto v = (cost_s)0; v <= Limit; v = (cost_s)(v + 1)) {
		auto n = p->effect[v];
		if(!n)
			continue;
		auto pd = getdescription(str("%1Effect", bsdata<costi>::elements[v].id));
		if(!pd)
			continue;
		if(need_line) {
			sb.addn("---\n");
			need_line = false;
		}
		sb.adds(pd, n);
	}
	for(auto v : p->conditions) {
		auto id = v.getid();
		if(!id)
			continue;
		auto pd = getdescription(str("%1Condition", id));
		if(!pd)
			continue;
		sb.adds(pd);
	}
}

template<> void ftstatus<costi>(const void* object, stringbuilder& sb) {
	auto p = (costi*)object;
	auto v = (cost_s)(p - bsdata<costi>::elements);
	add_description(p->id, sb);
	sb.addn("---");
	get_income(v, sb);
}

template<> void ftstatus<provincei>(const void* object, stringbuilder& sb) {
	auto p = (provincei*)object;
	add_description(p->id, sb);
	sb.addn("---");
	add_line_upkeep(p, sb);
}

template<> void ftstatus<buildingi>(const void* object, stringbuilder& sb) {
	auto p = (buildingi*)object;
	add_description(p, sb);
	sb.addn("---");
	add_line(sb, "Cost", p->cost);
	add_line(sb, "Upkeep", p->upkeep);
}

template<> void ftstatus<building>(const void* object, stringbuilder& sb) {
	auto p = (building*)object;
	add_description(p->type, sb);
	sb.addn("---");
	add_line(sb, "Upkeep", p->type->upkeep);
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

static void choose_units() {
	pushvalue push_image(answers::resid, "Units");
	an.clear();
	for(auto& e : bsdata<troop>()) {
		if(e.province != province)
			continue;
		an.add(&e, e.type->getname());
	}
	an.choose(0, getnm("Cancel"));
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
		player->resources[Build]--;
	}
	lastbuilding = push_building;
}

static void remove_building(building* pb) {
	if(!draw::yesno(getnm("DoYouWantRemoveBuilding"), pb->type->getname()))
		return;
	pb->clear();
}

static void choose_buildings() {
	pushvalue push_image(answers::resid, "Buildings");
	while(!draw::isnext()) {
		an.clear();
		collection<building> buildings; buildings.select(player_province_building);
		for(auto p : buildings)
			an.add(p, p->type->getname());
		if(province->buildings < province->current[Size] && player->resources[Build] > 0)
			an.add(add_building, getnm("AddBuilding"), player->resources[Build]);
		auto result = an.choose(0, getnm("Cancel"));
		if(!result)
			break;
		if(bsdata<building>::have(result))
			remove_building((building*)result);
		else
			((fnevent)result)();
	}
}

static void attack_site() {
}

static const char* get_site_header(const site* pv) {
	static char temp[260];
	stringbuilder sb(temp);
	sb.add("%1 - %2", pv->province->getname(), pv->type->getname());
	return temp;
}

static void choose_site_option(site* pv) {
	if(!pv)
		return;
	pushvalue push_header(answers::header, get_site_header(pv));
	pushvalue push_prompt(answers::prompt);
	pushvalue push_resid(answers::resid);
	if(pv->type->resid)
		answers::resid = pv->type->resid;
	answers::prompt = getdescription(pv->type->getid());
	an.clear();
	an.add(attack_site, getnm("Attack"));
	auto result = an.choose(0, getnm("Leave"), 1);
	if(!result)
		return;
	else
		((fnevent)result)();
}

static void choose_sites() {
	while(!draw::isnext()) {
		an.clear();
		for(auto& e : bsdata<site>()) {
			if(e.province != province)
				continue;
			an.add(&e, e.type->getname());
		}
		auto result = an.choose(0, getnm("Cancel"), 1);
		if(!result)
			break;
		else if(bsdata<site>::have(result))
			choose_site_option((site*)result);
		else
			((fnevent)result)();
	}
}

static void choose_province_options() {
	pushvalue push_input(input_province, choose_province);
	while(!draw::isnext() && province) {
		pushvalue push_answers(answers::header, province->getname());
		pushvalue push_image(answers::resid, province->landscape->id);
		an.clear();
		collection<troop> troops; troops.select(player_province_troop);
		if(troops)
			add_answers(choose_units, "Army", troops.getcount(), "Unit");
		collection<building> buildings; buildings.select(player_province_building);
		add_answers(choose_buildings, "Settlements", buildings.getcount(), "Building");
		add_sites(choose_sites, province->getsites(), province->current[Explore]);
		auto result = an.choose(0, getnm("Cancel"));
		if(!result) {
			province = 0;
			break;
		} else if(bsdata<provincei>::have(result))
			province = (provincei*)result;
		else {
			pushvalue push_input(input_province, (fnevent)0);
			((fnevent)result)();
		}
	}
}

static void choose_game_options() {
	pushvalue push_input(input_province, choose_province);
	an.clear();
	for(auto& e : bsdata<heroi>()) {
		an.add(&e, "#$left image '%1' 0 'art/avatars'\n###%2 - %-Level %3i", e.resid, e.getname(), e.effect[Strenght]);
	}
	auto result = an.choose(0, getnm("EndTurn"), 1);
	if(!result)
		end_turn();
	else if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else if(bsdata<heroi>::have(result)) {

	} else
		((fnevent)result)();
}

void player_turn() {
	if(province)
		choose_province_options();
	else
		choose_game_options();
	if(!draw::isnext())
		draw::setnext(player_turn);
}

bool fntestlist(int index, int bonus) {
	for(auto v : bsdata<listi>::elements[index].elements) {
		if(script::allow(v))
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

static void battle_stage(stringbuilder& sb, army& attacker, army& defender, cost_s ability) {
	auto suffix = bsdata<costi>::elements[ability].id;
	army af = attacker; af.match(ability, true);
	army df = defender; df.match(ability, true);
	if(!af && !df)
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
	if(ac) {
		sb.addsep('\n');
		ac.act(sb, getnm("CasualtiesTotal"));
		attacker.casualty(ac, defender);
	}
	if(dc) {
		sb.addsep('\n');
		dc.act(sb, getnm("CasualtiesTotal"));
		defender.casualty(dc, attacker);
	}
}

static void prepare_battle(army& attacker, army& defender) {
	if(!attacker.tactic)
		attacker.randomtactic();
	if(!defender.tactic)
		defender.randomtactic();
	// Same tactic - win attacker
	if(defender.tactic == attacker.tactic)
		defender.tactic = 0;
	// Attacker and defender can disable tactic
	if(defender.tactic && attacker.tactic) {
		if(attacker.tactic->disable.is(getbsi(defender.tactic)))
			defender.tactic = 0;
		else if(defender.tactic->disable.is(getbsi(attacker.tactic)))
			attacker.tactic = 0;
	}
	attacker.armor = attacker.get(Armor);
	defender.armor = defender.get(Armor);
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
	for(auto p : troops) {
		auto pu = find_troop(p, troops.province, troops.player);
		pu->province = pu->moveto;
		pu->moveto = 0;
	}
	troops.province->player = troops.player;
}

static void retreat_attacker(const army& troops) {
	for(auto p : troops) {
		auto pu = find_troop(p, troops.province, troops.player);
		pu->moveto = 0;
	}
}

static void retreat_defender(const army& troops) {
	neightbors provinces;
	provinces.selectn(troops.province);
	auto push_player = player; player = troops.player;
	provinces.match(is_province_player, true);
	provinces.match(is_province_ocean, false);
	if(provinces) {
		auto random = provinces.random();
		for(auto p : troops) {
			auto pu = find_troop(p, troops.province);
			pu->province = random;
		}
	} else {
		for(auto p : troops) {
			auto pu = find_troop(p, troops.province);
			pu->clear();
		}
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
		return;
	}
	if(attacker.tactic) {
		sb.addsep(' ');
		attacker.act(sb, getdescription(attacker.tactic->id));
	}
	sb.addsep(' ');
	defender.act(sb, getnm("ArmyDefend"), province->getname());
	if(defender.tactic) {
		sb.addsep(' ');
		defender.act(sb, getdescription(defender.tactic->id));
	}
	battle_stage(sb, attacker, defender, Archery);
	battle_stage(sb, attacker, defender, Charge);
	battle_stage(sb, attacker, defender, Damage);
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

static heroi* find_hero(const provincei* province, const playeri* player) {
	for(auto& e : bsdata<heroi>()) {
		if(e.province == province && e.player == player)
			return &e;
	}
	return 0;
}

void conquest() {
	char temp[4096]; stringbuilder sb(temp);
	army attacker, defender;
	attacker.clear();
	attacker.select(province, player);
	attacker.province = province;
	attacker.player = player;
	attacker.hero = find_hero(province, player);
	if(!attacker)
		return;
	defender.clear();
	defender.select(province);
	defender.province = province;
	defender.player = province->player;
	defender.hero = find_hero(province, province->player);
	conquest(sb, attacker, defender);
	answers::message(temp);
}

BSDATA(script) = {
	{"AddSite", add_site},
	{"Build", build, canbuild},
	{"Coastal", 0, is_coastal},
	{"GainIncome", gain_income},
	{"Recruit", recruit},
	{"RandomSite", random_site},
	{"UpdatePlayer", update_player},
};
BSDATAF(script)