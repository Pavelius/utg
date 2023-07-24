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

int get_income(cost_s v);
int get_income_modified(cost_s v, int result);

static int get_income(cost_s v, stringbuilder& sb) {
	auto push = lastcostitem;
	costitema source; lastcostitem = &source;
	auto result = get_income(v);
	result = get_income_modified(v, result);
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
	add_line(sb, 0, province->current[Lore]);
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

void add_line(stringbuilder& sb, const costa& source) {
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

static void add_description(const char* id, stringbuilder& sb) {
	sb.addn("##%1", getnm(id));
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
}

void costi::getinfo(stringbuilder& sb) const {
	auto v = (cost_s)(this - bsdata<costi>::elements);
	add_description(id, sb);
	sb.addn("---");
	get_income(v, sb);
}

void provincei::getinfo(stringbuilder& sb) const {
	add_description(id, sb);
	sb.addn("---");
	add_line_upkeep(this, sb);
}

void buildingi::getinfo(stringbuilder& sb) const {
	add_description(this, sb);
	sb.addn("---");
	add_line(sb, "Cost", cost);
	add_line(sb, "Upkeep", upkeep);
	if(upgrade)
		sb.addn("%Exchange %1", upgrade->getname());
}

void building::getinfo(stringbuilder& sb) const {
	add_description(type, sb);
	sb.addn("---");
	add_line(sb, "Upkeep", type->upkeep);
}