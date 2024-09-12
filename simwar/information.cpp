#include "army.h"
#include "collection.h"
#include "costitem.h"
#include "draw.h"
#include "draw_object.h"
#include "game.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "pushvalue.h"
#include "site.h"
#include "stringbuilder.h"
#include "script.h"
#include "statable.h"

static int get_income(costn v, stringbuilder& sb) {
	auto push = lastcostitem;
	costitema source; lastcostitem = &source;
	auto result = get_income(v);
	result = get_income_modified(v, result);
	add_cost_items(sb);
	lastcostitem = push;
	return result;
}

static void add_line(stringbuilder& sb, const provincei* province, costn v, int n) {
	if(!n)
		return;
	auto& e = bsdata<costi>::elements[v];
	if(e.frame == -1)
		return;
	switch(v) {
	case Size:
		sb.adds(":%1i:%2i/%3i", e.frame, province->buildings, n);
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
	add_line(sb, 4, province->get(Resources));
	add_line(sb, 4, province->get(Influence));
	add_line(sb, 4, province->get(Gold));
	add_line(sb, 0, province->get(Lore));
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

static void add_description(const sitei* p, stringbuilder& sb) {
	sb.addn("##%1", getnm(p->id));
	auto need_line = true;
	for(auto v = (costn)0; v <= Limit; v = (costn)(v + 1)) {
		auto n = p->income[v];
		if(!n)
			continue;
		auto pd = getnme(ids(bsdata<costi>::elements[v].id, "Effect"));
		if(!pd)
			continue;
		if(need_line) {
			sb.addn("---\n");
			need_line = false;
		}
		sb.adds(pd, n);
	}
	//for(auto v : p->conditions) {
	//	auto id = v.getid();
	//	if(!id)
	//		continue;
	//	auto pd = getnme(ids(id, "Condition"));
	//	if(!pd)
	//		continue;
	//	sb.adds(pd);
	//}
}

static void add_description(const char* id, stringbuilder& sb) {
	sb.addn("##%1", getnm(id));
	auto pn = getnme(ids(id, "Info"));
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
}

template<> void ftinfo<costi>(const void* object, stringbuilder& sb) {
	auto p = (costi*)object;
	auto v = (costn)(p - bsdata<costi>::elements);
	add_description(p->id, sb);
	sb.addn("---");
	get_income(v, sb);
}

template<> void ftinfo<provincei>(const void* object, stringbuilder& sb) {
	auto p = (provincei*)object;
	add_description(p->id, sb);
	sb.addn("---");
	add_line_upkeep(p, sb);
}

template<> void ftinfo<sitei>(const void* object, stringbuilder& sb) {
	auto p = (sitei*)object;
	add_description(p, sb);
	sb.addn("---");
	add_line(sb, "Cost", p->cost);
	add_line(sb, "Upkeep", p->upkeep);
}

template<> void ftinfo<site>(const void* object, stringbuilder& sb) {
	auto p = (site*)object;
	add_description(p->type, sb);
	sb.addn("---");
	add_line(sb, "Upkeep", p->type->upkeep);
}