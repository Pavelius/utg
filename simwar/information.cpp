#include "game.h"

static void add_description(const char* id, stringbuilder& sb) {
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
}

void get_cost_info(const void* object, stringbuilder& sb) {
	auto p = (costi*)object;
	auto v = (cost_s)(p - bsdata<costi>::elements);
	add_description(p->id, sb);
	sb.addn("---");
	game.getincome(v, &sb);
}