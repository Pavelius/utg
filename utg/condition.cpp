#include "bsreq.h"
#include "category.h"
#include "condition.h"

BSDATAC(conditioni, 64)
BSMETA(conditioni) = {
	BSREQ(id), BSREQ(param)
};

static conditioni* find(const char* id) {
	if(!id)
		return 0;
	for(auto& e : bsdata<conditioni>()) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

conditioni* conditioni::add(const char* id, fnproc proc, int param) {
	if(!proc)
		return 0;
	auto p = find(id);
	if(!p) {
		p = bsdata<conditioni>::add();
		p->id = szdup(id);
	}
	p->proc = proc;
	p->param = param;
	return p;
}

static bool if_entry(int index, int param) {
	if(index >= 0)
		return category::get(param + index) != 0;
	return category::get(param - index) == 0;
}

void conditioni::initialize() {
	conditioni::add("IfCategory", if_entry);
}