#include "building.h"
#include "province.h"
#include "stringbuilder.h"

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
		if(e.owner != player)
			continue;
		result += e.type->upkeep[v];
	}
	return get_value(-result, "BuildingsUpkeep", psb);
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

int get_income(const playeri* p, cost_s v, stringbuilder* psb) {
	auto result = get_provinces_income(p, v, psb);
	result += get_upkeep_buildings(p, v, psb);
	return result;
}

static int get_upgrade(const playeri* p, cost_s v, stringbuilder* psb) {
	return 0;
}