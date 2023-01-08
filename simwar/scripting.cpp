#include "building.h"
#include "player.h"
#include "province.h"
#include "script.h"
#include "unit.h"

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

int get_income(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = get_provinces_income(p, v, psb);
	result += get_effect_buildings(p, v, psb);
	result += get_provinces_upkeep(p, v, psb);
	result += get_upkeep_buildings(p, v, psb);
	result += get_upkeep_units(p, v, psb);
	result += get_value(p->trade[v], "TradeBonus", psb);
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
		player->income[i] = get_income(player, i, 0);
}

static void update_player(int bonus) {
	update_income();
	player->resources[Warfire] = get_units_warfire(player);
}

BSDATA(script) = {
	{"Build", build, canbuild},
	{"Recruit", recruit},
	{"UpdatePlayer", update_player},
};
BSDATAF(script)