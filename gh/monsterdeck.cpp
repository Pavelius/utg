#include "main.h"

BSDATAC(monsterdeck, 64)

monsterdeck& monsterdeck::get(const char* id) {
	for(auto& e : bsdata<monsterdeck>()) {
		if(equal(e.id, id))
			return e;
	}
	auto p = bsdata<monsterdeck>::add();
	if(!p)
		return bsdata<monsterdeck>::elements[0];
	p->id = szdup(id);
	p->clear();
	for(auto& e : bsdata<monstercardi>()) {
		if(equal(e.id, id)) {
			auto pv = p->add();
			if(pv)
				*pv = &e - bsdata<monstercardi>::elements;
		}
	}
	p->shuffle();
	return *p;
}