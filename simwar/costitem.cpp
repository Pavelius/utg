#include "costitem.h"

costitema* lastcostitem;

static costitem* find_add(const char* id, int value) {
	for(auto& e : *lastcostitem) {
		if(e.id == id)
			return &e;
	}
	auto p = lastcostitem->add();
	p->id = id;
	p->value = 0;
	return p;
}

int get_value(const char* id, int value) {
	if(lastcostitem && value) {
		auto p = find_add(id, value);
		p->value += value;
	}
	return value;
}

void add_cost_items(stringbuilder& sb) {
	sb.clear();
	if(!lastcostitem)
		return;
	for(auto& e : *lastcostitem) {
		if(!e.value)
			continue;
		if(e.value > 0)
			sb.addn("[+%1i %2]", e.value, getnm(e.id));
		else
			sb.addn("[-%1i %2]", e.value, getnm(e.id));
	}
}