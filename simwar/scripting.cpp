#include "answers.h"
#include "building.h"
#include "collection.h"
#include "draw.h"
#include "game.h"
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

static void end_turn() {
	game.turn++;
	script::run("PlayerUpdate");
	script::run("GainIncome");
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
	return ((building*)pv)->province && ((building*)pv)->province->owner == player;
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

static int get_value(int value, const char* id, stringbuilder* psb) {
	if(psb) {
		if(value > 0)
			psb->addn("[+%1i\t%2]", value, getnm(id));
		else if(value < 0)
			psb->addn("[-%1i\t%2]", value, getnm(id));
	}
	return value;
}

static int get_upkeep_buildings(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province && e.province->owner != player)
			continue;
		result += e.type->upkeep[v];
	}
	return get_value(-result, "BuildingsUpkeep", psb);
}

static int get_upkeep_units(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.player != player)
			continue;
		result += e.type->upkeep[v];
	}
	return get_value(-result, "UnitsUpkeep", psb);
}

static int get_effect_buildings(const playeri* p, const buildingi* b, cost_s v, stringbuilder* psb) {
	auto result = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province && e.province->owner != player || e.type!=b)
			continue;
		result += e.type->effect[v];
	}
	return get_value(result, b->id, psb);
}

static int get_effect_buildings(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = 0;
	for(auto& e : bsdata<buildingi>())
		result += get_effect_buildings(p, &e, v, psb);
	return result;
}

static int get_income(const provincei* p, cost_s v, stringbuilder* psb) {
	auto result = p->landscape->effect[v];
	result += p->income[v];
	return get_value(result, p->id, psb);
}

static int get_provinces_income(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.owner != player)
			continue;
		result += get_income(&e, v, 0);
	}
	return get_value(result, "ProvincesIncome", psb);
}

static int get_provinces_upkeep(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.owner != player)
			continue;
		result += e.landscape->upkeep[v];
	}
	return get_value(-result, "ProvincesUpkeep", psb);
}

static int get_income(cost_s v, stringbuilder* psb) {
	auto result = get_provinces_income(player, v, psb);
	result += get_effect_buildings(player, v, psb);
	result += get_provinces_upkeep(player, v, psb);
	result += get_upkeep_buildings(player, v, psb);
	result += get_upkeep_units(player, v, psb);
	result += get_value(player->trade[v], "TradeBonus", psb);
	return result;
}

static int get_units_upkeep(const playeri* p, cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.player == p)
			result += e.type->upkeep[v];
	}
	return result;
}

static int get_units(const playeri* p, const provincei* province, cost_s v) {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.player == p && e.province==province)
			result += e.type->effect[v];
	}
	return result;
}

static void update_income() {
	for(auto i = (cost_s)0; i <= Warfire; i = (cost_s)(i + 1))
		player->income[i] = get_income(i, 0);
}

static void update_player(int bonus) {
	update_income();
}

static void gain_income(int bonus) {
	for(auto i = 0; i < Warfire; i++)
		player->resources[i] += player->income[i];
}

static void random_site(int bonus) {
	collection<sitei> source;
	source.select();
	lastsite = source.random();
}

int	provincei::get(cost_s v, stringbuilder* psb) const {
	auto result = income[v];
	result += landscape->effect[v];
	return get_value(result, id, psb);
}

static void add_line(stringbuilder& sb, const provincei* province, cost_s v, int n) {
	int i;
	switch(v) {
	case Size:
		sb.add("%Buildings %1i/%2i", province->getbuildings(), n);
		break;
	case Strenght:
		i = get_units(player, province, Strenght);
		if(i)
			sb.add("%Army %1i", i + province->get(v));
		break;
	default:
		sb.add("%1%+2i", getnm(bsdata<costi>::elements[v].id), n);
		break;
	}
}

static void add_line_upkeep(const provincei* province, stringbuilder& sb) {
	auto pb = sb.get();
	for(auto i = (cost_s)0; i < Limit; i = (cost_s)(i + 1)) {
		auto n = province->get(i);
		if(!n)
			continue;
		auto p0 = sb.get();
		if(!pb[0])
			sb.addn("---");
		else
			sb.add(", ");
		auto p1 = sb.get();
		add_line(sb, province, i, n);
		if(p1[0]==0)
			sb.set(p0);
	}
}

template<> void ftstatus<costi>(const void* object, stringbuilder& sb) {
	auto p = (costi*)object;
	auto v = (cost_s)(p - bsdata<costi>::elements);
	add_description(p->id, sb);
	sb.addn("---");
	get_income(v, &sb);
}

template<> void ftstatus<provincei>(const void* object, stringbuilder& sb) {
	auto p = (provincei*)object;
	add_description(p->id, sb);
	add_line_upkeep(p, sb);
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
		an.add(lastbuilding, getnm(lastbuilding->id));
	}
	lastbuilding = (buildingi*)an.choose(getnm("WhatDoYouWantToBuild"), getnm("Cancel"));
}

static void add_building() {
	auto push_building = lastbuilding;
	choose_build(0);
	if(lastbuilding)
		build(0);
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
		if(province->getbuildings() < province->get(Size))
			an.add(add_building, getnm("AddBuilding"));
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
		add_sites(choose_sites, province->getsites(), province->get(Explore));
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
	auto result = an.choose(0, getnm("EndTurn"), 1);
	if(!result)
		end_turn();
	else if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else
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

bool sitei::isallow(variant v) const {
	if(v.iskind<sitei>())
		return have_exist(bsdata<sitei>::elements + v.value);
	return true;
}

bool sitei::isallow() const {
	for(auto v : conditions) {
		if(!isallow(v))
			return false;
	}
	return true;
}

BSDATA(script) = {
	{"AddSite", add_site},
	{"Build", build, canbuild},
	{"GainIncome", gain_income},
	{"Recruit", recruit},
	{"RandomSite", random_site},
	{"UpdatePlayer", update_player},
};
BSDATAF(script)