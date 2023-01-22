#include "army.h"
#include "building.h"
#include "collection.h"
#include "costitem.h"
#include "draw.h"
#include "draw_object.h"
#include "game.h"
#include "hero.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "script.h"
#include "statable.h"
#include "unit.h"

int get_income(cost_s v);

static int get_income(cost_s v, stringbuilder& sb) {
	auto push = lastcostitem;
	costitema source; lastcostitem = &source;
	auto result = get_income(v);
	add_cost_items(sb);
	lastcostitem = push;
	return result;
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
	if(n>1)
		sb.adds(":%2i:%1i", n, f);
	else
		sb.adds(":%1i:", f);
}

static void add_line(stringbuilder& sb, int f, int n, int nm) {
	if(!nm)
		return;
	sb.adds(":%2i:%1i/%3i", n, f, nm);
}

void add_line_upkeep(const provincei* province, stringbuilder& sb) {
	add_line(sb, 4, province->current[Gold]);
	add_line(sb, 6, province->current[Mana]);
	add_line(sb, 3, province->current[Faith]);
	add_line(sb, 0, province->current[Lore]);
	add_line(sb, 1, province->attack);
	add_line(sb, 2, province->defend);
	add_line(sb, 5, province->buildings);
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

static void add_description(const uniti* p, stringbuilder& sb) {
	sb.addn("##%1", getnm(p->id));
	auto need_line = true;
	for(auto v = (cost_s)0; v <= Limit; v = (cost_s)(v + 1)) {
		auto n = p->effect[v];
		if(!n)
			continue;
		auto pd = getdescription(str("%1UnitEffect", bsdata<costi>::elements[v].id));
		if(!pd)
			continue;
		if(need_line) {
			sb.addn("---\n");
			need_line = false;
		}
		sb.adds(pd, n);
	}
}

static void add_description(const char* id, stringbuilder& sb) {
	sb.addn("##%1", getnm(id));
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
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
	if(p->upgrade)
		sb.addn("%Exchange %1", p->upgrade->getname());
}

template<> void ftstatus<uniti>(const void* object, stringbuilder& sb) {
	auto p = (uniti*)object;
	add_description(p, sb);
	sb.addn("---");
	add_line(sb, "Cost", p->cost);
	add_line(sb, "Upkeep", p->upkeep);
}

template<> void ftstatus<troop>(const void* object, stringbuilder& sb) {
	auto p = (troop*)object;
	add_description(p->type, sb);
	sb.addn("---");
	add_line(sb, "Upkeep", p->type->upkeep);
}

template<> void ftstatus<building>(const void* object, stringbuilder& sb) {
	auto p = (building*)object;
	add_description(p->type, sb);
	sb.addn("---");
	add_line(sb, "Upkeep", p->type->upkeep);
}