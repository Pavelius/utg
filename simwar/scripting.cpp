#include "answers.h"
#include "collection.h"
#include "draw.h"
#include "game.h"
#include "statable.h"
#include "building.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"
#include "unit.h"

static answers an;
void player_turn();

static void build(int bonus) {
	auto p = bsdata<building>::add();
	p->type = lastbuilding;
	p->province = province;
	p->player = player;
}

static bool have_builded(const buildingi* pv) {
	for(auto& e : bsdata<building>()) {
		if(e.province == province && e.type == pv)
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
	}
	return true;
}

static void recruit() {
	auto p = bsdata<troop>::add();
	p->type = lastunit;
	p->province = province;
	p->player = player;
}

static void recruit(int bonus) {
	for(auto i = 0; i < bonus; i++)
		recruit();
}

template<> void fnscript<costi>(int value, int bonus) {
	player->resources[value] += bonus;
}

template<> void fnscript<buildingi>(int value, int bonus) {
	lastbuilding = bsdata<buildingi>::elements + value;
	if(bonus > 0) {
		build(bonus);
	} else if(bonus < 0) {
	}
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
	return ((building*)pv)->player == player;
}

static bool player_province_building(const void* pv) {
	return ((building*)pv)->player == player && ((building*)pv)->province == province;
}

static void select_units(collection<troop>& source) {
}

static void accept_answers(void* result) {
	if(!result)
		province = 0;
	else if(bsdata<provincei>::have(result))
		province = (provincei*)result;
	else if(bsdata<troop>::have(result)) {
	} else if(bsdata<building>::have(result)) {
	} else
		((fnevent)result)();
	draw::setnext(player_turn);
}

static void choose_units() {
	pushvalue push_image(answers::resid, "Units");
	an.clear();
	for(auto& e : bsdata<troop>()) {
		if(e.province != province)
			continue;
		an.add(&e, e.type->getname());
	}
	auto push_header = answers::header;
	answers::header = province->getname();
	auto pu = an.choose(0, getnm("Cancel"));
	answers::header = push_header;
	if(!pu)
		return;
}

static void add_building() {
	an.clear();
	for(auto& e : bsdata<buildingi>()) {
	}
}

static void choose_buildings() {
	an.clear();
	collection<building> buildings; buildings.select(player_province_building);
	for(auto p : buildings)
		an.add(p, p->type->getname());
	an.add(add_building, getnm("AddBuilding"));
	pushvalue push_image(answers::resid, "Buildings");
	auto push_header = answers::header;
	answers::header = province->getname();
	auto pu = an.choose(0, getnm("Cancel"));
	answers::header = push_header;
	if(bsdata<building>::have(pu)) {

	} else
		accept_answers(pu);
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

static void province_options() {
	collection<troop> troops; troops.select(player_province_troop);
	if(troops)
		add_answers(choose_units, "Units", troops.getcount());
	collection<building> buildings; buildings.select(player_province_building);
	add_answers(choose_buildings, "Settlements", buildings.getcount(), "Building");
}

static void* choose_answers() {
	pushvalue push_answers(answers::header);
	pushvalue push_input(input_province, choose_province);
	pushvalue push_image(answers::resid);
	if(province) {
		answers::header = province->getname();
		answers::resid = province->landscape->id;
	}
	an.clear();
	if(province) {
		province_options();
		return an.choose(0, getnm("Cancel"));
	} else {
		an.add(end_turn, getnm("EndTurn"));
		return an.choose();
	}
}

void player_turn() {
	auto p = choose_answers();
	accept_answers(p);
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
		if(e.player != player)
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
		if(e.player != player || e.type!=b)
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

static int get_upkeep(const provincei* p, cost_s v, stringbuilder* psb) {
	auto result = p->landscape->income[v];
	return get_value(result, p->id, psb);
}

static int get_provinces_upkeep(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.owner != player)
			continue;
		result += get_upkeep(&e, v, 0);
	}
	return get_value(-result, "ProvincesUpkeep", psb);
}

int gamei::getincome(cost_s v, stringbuilder* psb) {
	auto result = get_provinces_income(player, v, psb);
	result += get_effect_buildings(player, v, psb);
	result += get_provinces_upkeep(player, v, psb);
	result += get_upkeep_buildings(player, v, psb);
	result += get_upkeep_units(player, v, psb);
	result += get_value(player->trade[v], "TradeBonus", psb);
	return result;
}

int get_units_warfire(const playeri* p) {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.player == p)
			result++;
	}
	return result;
}

static void update_income() {
	for(auto i = (cost_s)0; i <= Warfire; i = (cost_s)(i + 1))
		player->income[i] = game.getincome(i, 0);
}

static void update_player(int bonus) {
	update_income();
	player->resources[Warfire] = get_units_warfire(player);
}

static void gain_income(int bonus) {
	for(auto i = 0; i < Warfire; i++)
		player->resources[i] += player->income[i];
}

BSDATA(script) = {
	{"Build", build, canbuild},
	{"GainIncome", gain_income},
	{"Recruit", recruit},
	{"UpdatePlayer", update_player},
};
BSDATAF(script)